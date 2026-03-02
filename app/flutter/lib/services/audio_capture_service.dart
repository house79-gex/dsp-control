import 'dart:async';
import 'dart:math';
import 'dart:typed_data';
import 'package:record/record.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:io';

// ======= Servizio Cattura Audio Smartphone + FFT =======
// Cattura audio dal microfono dello smartphone, calcola FFT a 32 bande
// logaritmiche 20Hz–20kHz e fornisce i dati per l'AutoTune remoto.

class AudioCaptureService {
  final AudioRecorder _recorder = AudioRecorder();
  bool _isCapturing = false;
  StreamController<List<double>>? _fftController;

  // Numero bande FFT logaritmiche
  static const int numBands = 32;
  static const int sampleRate = 44100;
  static const int fftSize = 2048;

  /// Avvia la cattura audio e calcola la FFT ogni [intervalMs] ms.
  /// Chiama [onFftReady] con i livelli normalizzati 0.0–1.0 per ogni banda.
  Future<void> startCapture(
    void Function(List<double> fftBands) onFftReady, {
    int intervalMs = 100,
  }) async {
    if (_isCapturing) return;

    final hasPermission = await _recorder.hasPermission();
    if (!hasPermission) {
      throw Exception('Permesso microfono non concesso');
    }

    _isCapturing = true;
    final tmpDir = await getTemporaryDirectory();

    while (_isCapturing) {
      final path = '${tmpDir.path}/autotune_capture.wav';

      // Registra [intervalMs] ms di audio
      await _recorder.start(
        const RecordConfig(
          encoder: AudioEncoder.wav,
          sampleRate: sampleRate,
          numChannels: 1,
          bitRate: sampleRate * 16,  // bitRate = sampleRate × bitsPerSample × numChannels
        ),
        path: path,
      );

      await Future.delayed(Duration(milliseconds: intervalMs));

      if (!_isCapturing) {
        await _recorder.stop();
        break;
      }

      await _recorder.stop();

      // Leggi campioni dal file WAV e calcola FFT
      try {
        final samples = await _loadWavSamples(path);
        if (samples.isNotEmpty) {
          final bands = _computeFftBands(samples);
          onFftReady(bands);
        }
      } catch (_) {
        // In caso di errore lettura, continua il loop
      }
    }
  }

  /// Ferma la cattura audio.
  Future<void> stopCapture() async {
    _isCapturing = false;
    if (await _recorder.isRecording()) {
      await _recorder.stop();
    }
  }

  bool get isCapturing => _isCapturing;

  void dispose() {
    _isCapturing = false;
    _recorder.dispose();
    _fftController?.close();
  }

  // ——— Lettura campioni PCM da file WAV ———
  Future<List<double>> _loadWavSamples(String path) async {
    final file = File(path);
    if (!file.existsSync()) return [];

    final bytes = await file.readAsBytes();
    // WAV header: 44 byte standard
    if (bytes.length < 44) return [];

    final samples = <double>[];
    // Campioni PCM 16bit little-endian da offset 44
    for (int i = 44; i + 1 < bytes.length; i += 2) {
      final raw = bytes[i] | (bytes[i + 1] << 8);
      // Converte uint16 → int16 signed
      final signed = raw > 32767 ? raw - 65536 : raw;
      samples.add(signed / 32768.0);
    }
    return samples;
  }

  // ——— FFT (DFT reale semplificata, adeguata per analisi spettrale) ———
  List<double> _computeFftBands(List<double> samples) {
    final n = min(fftSize, samples.length);
    // Applica finestra di Hann
    final windowed = List<double>.generate(n, (i) {
      final w = 0.5 * (1.0 - cos(2.0 * pi * i / (n - 1)));
      return samples[i] * w;
    });

    // DFT reale → magnitudine spettrale (primi n/2 bin)
    final numBins = n ~/ 2;
    final magnitudes = List<double>.filled(numBins, 0.0);
    for (int k = 0; k < numBins; k++) {
      double re = 0.0, im = 0.0;
      for (int t = 0; t < n; t++) {
        final angle = 2.0 * pi * k * t / n;
        re += windowed[t] * cos(angle);
        im -= windowed[t] * sin(angle);
      }
      magnitudes[k] = sqrt(re * re + im * im) / n;
    }

    // Binning logaritmico in [numBands] bande (20Hz–20kHz)
    return _extractLogBands(magnitudes, numBins);
  }

  // ——— Distribuzione bande logaritmiche 20Hz–20kHz ———
  List<double> _extractLogBands(List<double> magnitudes, int numBins) {
    final bands = List<double>.filled(numBands, 0.0);
    const fMin = 20.0;
    const fMax = 20000.0;
    final logMin = log(fMin);
    final logMax = log(fMax);
    final logStep = (logMax - logMin) / numBands;
    final freqResolution = sampleRate / (numBins * 2.0);

    for (int b = 0; b < numBands; b++) {
      final fLow  = exp(logMin + b * logStep);
      final fHigh = exp(logMin + (b + 1) * logStep);
      final kLow  = max(0, (fLow  / freqResolution).floor());
      final kHigh = min(numBins - 1, (fHigh / freqResolution).ceil());

      if (kHigh < kLow) {
        bands[b] = 0.0;
        continue;
      }

      double sum = 0.0;
      for (int k = kLow; k <= kHigh; k++) {
        sum += magnitudes[k];
      }
      // Media dei bin nella banda, convertita in dB normalizzato 0–1
      final avg = sum / (kHigh - kLow + 1);
      final db = avg > 0.0 ? (20.0 * log(avg) / ln10) : -80.0;
      // Normalizza da -80dB → 0dB a 0.0 → 1.0
      bands[b] = ((db + 80.0) / 80.0).clamp(0.0, 1.0);
    }
    return bands;
  }

  /// Applica curva di calibrazione microfono ai dati FFT.
  /// [calibration]: mappa { bandIndex → correctionDb }
  List<double> applyCalibration(
    List<double> fftBands,
    Map<int, double> calibration,
  ) {
    final corrected = List<double>.from(fftBands);
    calibration.forEach((band, correctionDb) {
      if (band >= 0 && band < corrected.length) {
        final correctedDb = (corrected[band] * 80.0 - 80.0) + correctionDb;
        corrected[band] = ((correctedDb + 80.0) / 80.0).clamp(0.0, 1.0);
      }
    });
    return corrected;
  }
}
