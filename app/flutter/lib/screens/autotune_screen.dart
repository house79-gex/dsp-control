import 'dart:async';
import 'dart:math';
import 'package:flutter/material.dart';
import 'package:file_picker/file_picker.dart';
import '../services/api_client.dart';
import '../models/mic_calibration.dart';

enum AutotuneSource { localMic, smartphoneMic, usbMeasurementMic }

class AutotuneScreen extends StatefulWidget {
  final ApiClient api;
  const AutotuneScreen({super.key, required this.api});

  @override
  State<AutotuneScreen> createState() => _AutotuneScreenState();
}

class _AutotuneScreenState extends State<AutotuneScreen> {
  AutotuneSource _source = AutotuneSource.localMic;
  bool _running = false;
  double _progress = 0.0;
  String _statusMsg = 'Pronto per calibrazione';
  Map<String, dynamic>? _status;
  Timer? _pollTimer;
  MicCalibration? _micCal;
  bool _loadingCal = false;

  @override
  void dispose() {
    _pollTimer?.cancel();
    super.dispose();
  }

  Future<void> _startAutotune() async {
    setState(() {
      _running = true;
      _progress = 0.0;
      _statusMsg = 'Avvio autotune...';
    });

    try {
      if (_source == AutotuneSource.localMic) {
        await widget.api.startAutotune('single');
      } else if (_source == AutotuneSource.smartphoneMic) {
        await widget.api.startAutotuneRemote();
      } else {
        await widget.api.startAutotuneUsbMic();
      }
      _pollTimer =
          Timer.periodic(const Duration(milliseconds: 500), _pollStatus);
    } catch (e) {
      setState(() {
        _running = false;
        _statusMsg = 'Errore avvio: $e';
      });
    }
  }

  Future<void> _pollStatus(Timer t) async {
    try {
      final st = await widget.api.getAutotuneStatus();
      setState(() {
        _status = st;
        _progress = (st['progress'] as num?)?.toDouble() ?? 0.0;
        _statusMsg = st['message'] as String? ?? '';
        final stateNum = st['state'] as int? ?? 0;
        // State 6 = Complete, 7 = Error
        if (stateNum >= 6) {
          _running = false;
          _pollTimer?.cancel();
        }
      });
    } catch (_) {
      // Ignora errori polling
    }
  }

  Future<void> _applyResults() async {
    try {
      await widget.api.applyAutotuneResults();
      setState(() { _statusMsg = 'Correzioni applicate al DSP!'; });
    } catch (e) {
      setState(() { _statusMsg = 'Errore applicazione: $e'; });
    }
  }

  Future<void> _cancel() async {
    _pollTimer?.cancel();
    try {
      await widget.api.cancelAutotune();
    } catch (_) {}
    setState(() {
      _running = false;
      _progress = 0.0;
      _statusMsg = 'Annullato';
    });
  }

  Future<void> _loadCalFile() async {
    setState(() => _loadingCal = true);
    try {
      final result = await FilePicker.platform.pickFiles(
        type: FileType.custom,
        allowedExtensions: ['cal', 'txt'],
        withData: true,
      );
      if (result == null || result.files.isEmpty) {
        setState(() => _loadingCal = false);
        return;
      }
      final file = result.files.first;
      final content = String.fromCharCodes(file.bytes ?? []);
      final name = file.name.replaceAll('.cal', '').replaceAll('.txt', '');
      final cal = MicCalibration.fromCalFile(name, content);
      await widget.api.uploadMicCalibration(cal);
      setState(() {
        _micCal = cal;
        _statusMsg = 'Calibrazione caricata: ${cal.micName} (${cal.corrections.length} punti)';
      });
    } catch (e) {
      setState(() => _statusMsg = 'Errore caricamento calibrazione: $e');
    } finally {
      setState(() => _loadingCal = false);
    }
  }

  Future<void> _deleteCalibration() async {
    try {
      await widget.api.deleteMicCalibration();
      setState(() {
        _micCal = null;
        _statusMsg = 'Calibrazione rimossa';
      });
    } catch (e) {
      setState(() => _statusMsg = 'Errore rimozione calibrazione: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    final isComplete = (_status?['state'] as int? ?? 0) == 6;

    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213e),
        title:
            const Text('AutoTune', style: TextStyle(color: Colors.white)),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Selezione sorgente
            const Text('Sorgente microfono',
                style: TextStyle(
                    color: Colors.white,
                    fontSize: 16,
                    fontWeight: FontWeight.bold)),
            const SizedBox(height: 12),
            _SourceCard(
              title: 'Microfono Locale (ESP32)',
              subtitle:
                  'Usa l\'ingresso ADC ES8388 sul modulo',
              icon: Icons.mic,
              selected: _source == AutotuneSource.localMic,
              onTap: () =>
                  setState(() => _source = AutotuneSource.localMic),
            ),
            const SizedBox(height: 8),
            _SourceCard(
              title: 'Microfono Smartphone',
              subtitle:
                  'Cattura audio col microfono del telefono via WiFi',
              icon: Icons.smartphone,
              selected: _source == AutotuneSource.smartphoneMic,
              onTap: () =>
                  setState(() => _source = AutotuneSource.smartphoneMic),
            ),
            const SizedBox(height: 8),
            _SourceCard(
              title: 'Microfono USB Calibrato (Dayton iMM-6C)',
              subtitle:
                  'Microfono di misura USB-C con calibrazione individuale — Raccomandato',
              icon: Icons.science,
              selected: _source == AutotuneSource.usbMeasurementMic,
              onTap: () =>
                  setState(() => _source = AutotuneSource.usbMeasurementMic),
            ),
            const SizedBox(height: 16),

            // Sezione calibrazione (visibile solo per microfono USB)
            if (_source == AutotuneSource.usbMeasurementMic) ...[
              _CalibrationSection(
                micCal: _micCal,
                loading: _loadingCal,
                onLoad: _loadCalFile,
                onDelete: _deleteCalibration,
              ),
              const SizedBox(height: 16),
            ],

            // Istruzioni
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: const Color(0xFF16213e),
                borderRadius: BorderRadius.circular(10),
                border: Border.all(color: Colors.white12),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Text('Come funziona:',
                      style: TextStyle(
                          color: Colors.white,
                          fontWeight: FontWeight.bold)),
                  const SizedBox(height: 8),
                  _step('1', 'Il sistema genera un sweep 20Hz-20kHz'),
                  _step(
                      '2',
                      _source == AutotuneSource.localMic
                          ? 'Il microfono ES8388 cattura la risposta in frequenza'
                          : _source == AutotuneSource.smartphoneMic
                              ? 'Il microfono dello smartphone cattura la risposta'
                              : 'Il microfono USB iMM-6C cattura la risposta calibrata'),
                  _step('3', 'Analisi FFT e calcolo correzione EQ'),
                  _step('4', 'Applicazione EQ e delay al DSP CQ260D'),
                ],
              ),
            ),
            const SizedBox(height: 24),

            // Grafico risposta in frequenza (visibile dopo completamento)
            if (isComplete && _status != null) ...[
              const Text('Risposta in frequenza',
                  style: TextStyle(
                      color: Colors.white,
                      fontSize: 14,
                      fontWeight: FontWeight.bold)),
              const SizedBox(height: 8),
              SizedBox(
                height: 180,
                child: CustomPaint(
                  painter: _FrequencyResponsePainter(
                    status: _status!,
                    micCal: _micCal,
                  ),
                  child: Container(),
                ),
              ),
              const SizedBox(height: 24),
            ],

            // Barra progresso
            LinearProgressIndicator(
              value: _running
                  ? _progress
                  : (_progress > 0 ? _progress : null),
              backgroundColor: const Color(0xFF0f3460),
              valueColor: const AlwaysStoppedAnimation<Color>(
                  Color(0xFFe94560)),
              minHeight: 8,
            ),
            const SizedBox(height: 8),
            Text(_statusMsg,
                style: const TextStyle(
                    color: Colors.white70, fontSize: 13)),
            const SizedBox(height: 24),

            // Bottoni
            Row(
              children: [
                if (!_running)
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFFe94560),
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.tune),
                      label: const Text('AVVIA AUTOTUNE',
                          style: TextStyle(fontSize: 16)),
                      onPressed: _startAutotune,
                    ),
                  ),
                if (_running) ...[
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFF555555),
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.stop),
                      label: const Text('ANNULLA'),
                      onPressed: _cancel,
                    ),
                  ),
                ],
                if (isComplete) ...[
                  const SizedBox(width: 12),
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFF00e676),
                        foregroundColor: Colors.black,
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.check),
                      label: const Text('APPLICA CORREZIONE',
                          style: TextStyle(fontSize: 14)),
                      onPressed: _applyResults,
                    ),
                  ),
                ],
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _step(String num, String text) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 3),
      child: Row(
        children: [
          CircleAvatar(
            radius: 10,
            backgroundColor: const Color(0xFFe94560),
            child: Text(num,
                style: const TextStyle(
                    color: Colors.white, fontSize: 10)),
          ),
          const SizedBox(width: 8),
          Expanded(
              child: Text(text,
                  style: const TextStyle(
                      color: Colors.white70, fontSize: 13))),
        ],
      ),
    );
  }
}

// ======= Widget sezione calibrazione =======

class _CalibrationSection extends StatelessWidget {
  final MicCalibration? micCal;
  final bool loading;
  final VoidCallback onLoad;
  final VoidCallback onDelete;

  const _CalibrationSection({
    required this.micCal,
    required this.loading,
    required this.onLoad,
    required this.onDelete,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: const Color(0xFF16213e),
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: const Color(0xFF00b0ff).withOpacity(0.4)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Row(
            children: [
              Icon(Icons.science, color: Color(0xFF00b0ff), size: 18),
              SizedBox(width: 8),
              Text('Curva di calibrazione iMM-6C',
                  style: TextStyle(
                      color: Colors.white,
                      fontWeight: FontWeight.bold,
                      fontSize: 13)),
            ],
          ),
          const SizedBox(height: 8),
          Text(
            micCal == null
                ? 'Nessuna curva caricata'
                : 'Curva caricata: ${micCal!.micName} (${micCal!.corrections.length} punti)',
            style: TextStyle(
                color: micCal == null ? Colors.white38 : const Color(0xFF00e676),
                fontSize: 12),
          ),
          const SizedBox(height: 10),
          Row(
            children: [
              Expanded(
                child: OutlinedButton.icon(
                  style: OutlinedButton.styleFrom(
                    foregroundColor: const Color(0xFF00b0ff),
                    side: const BorderSide(color: Color(0xFF00b0ff)),
                    padding: const EdgeInsets.symmetric(vertical: 10),
                  ),
                  icon: loading
                      ? const SizedBox(
                          width: 16,
                          height: 16,
                          child: CircularProgressIndicator(strokeWidth: 2))
                      : const Icon(Icons.folder_open, size: 16),
                  label: const Text('📁 Carica file .cal', style: TextStyle(fontSize: 12)),
                  onPressed: loading ? null : onLoad,
                ),
              ),
              if (micCal != null) ...[
                const SizedBox(width: 8),
                OutlinedButton.icon(
                  style: OutlinedButton.styleFrom(
                    foregroundColor: Colors.redAccent,
                    side: const BorderSide(color: Colors.redAccent),
                    padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 12),
                  ),
                  icon: const Icon(Icons.delete_outline, size: 16),
                  label: const Text('🗑', style: TextStyle(fontSize: 12)),
                  onPressed: onDelete,
                ),
              ],
            ],
          ),
        ],
      ),
    );
  }
}

// ======= Grafico risposta in frequenza =======

class _FrequencyResponsePainter extends CustomPainter {
  final Map<String, dynamic> status;
  final MicCalibration? micCal;

  _FrequencyResponsePainter({required this.status, this.micCal});

  // Converte frequenza in posizione X (asse logaritmico 20Hz–20kHz)
  double _freqToX(double freq, double width) {
    const double minFreq = 20.0;
    const double maxFreq = 20000.0;
    return (log(freq / minFreq) / log(maxFreq / minFreq)) * width;
  }

  // Converte dB in posizione Y (asse lineare -30dB a +10dB)
  double _dbToY(double db, double height) {
    const double minDb = -30.0;
    const double maxDb = 10.0;
    return height - ((db - minDb) / (maxDb - minDb)) * height;
  }

  @override
  void paint(Canvas canvas, Size size) {
    final w = size.width;
    final h = size.height;

    // Sfondo
    canvas.drawRect(Rect.fromLTWH(0, 0, w, h),
        Paint()..color = const Color(0xFF0d1b2a));

    // Griglia asse Y (dB)
    final gridPaint = Paint()
      ..color = Colors.white12
      ..strokeWidth = 0.5;
    for (final db in [-30.0, -20.0, -10.0, 0.0, 10.0]) {
      final y = _dbToY(db, h);
      canvas.drawLine(Offset(0, y), Offset(w, y), gridPaint);
    }

    // Griglia asse X (frequenze)
    for (final freq in [50.0, 100.0, 500.0, 1000.0, 5000.0, 10000.0, 20000.0]) {
      final x = _freqToX(freq, w);
      canvas.drawLine(Offset(x, 0), Offset(x, h), gridPaint);
    }

    // Linea 0 dB
    final zeroPaint = Paint()
      ..color = Colors.white24
      ..strokeWidth = 1;
    canvas.drawLine(Offset(0, _dbToY(0, h)), Offset(w, _dbToY(0, h)), zeroPaint);

    // Curva risposta misurata (bianca)
    final results = status['results'] as List<dynamic>?;
    if (results != null && results.isNotEmpty) {
      final result = results[0] as Map<String, dynamic>?;
      if (result != null) {
        final freqs = (result['responseFreq'] as List<dynamic>?) ?? [];
        final mags = (result['responseMagnitude'] as List<dynamic>?) ?? [];
        if (freqs.length >= 2) {
          final measuredPath = Path();
          bool first = true;
          for (int i = 0; i < freqs.length; i++) {
            final freq = (freqs[i] as num).toDouble();
            final mag = (mags[i] as num).toDouble();
            final x = _freqToX(freq, w);
            final y = _dbToY(mag, h);
            if (first) {
              measuredPath.moveTo(x, y);
              first = false;
            } else {
              measuredPath.lineTo(x, y);
            }
          }
          canvas.drawPath(
              measuredPath,
              Paint()
                ..color = Colors.white
                ..strokeWidth = 1.5
                ..style = PaintingStyle.stroke);
        }
      }
    }

    // Curva di calibrazione (grigia tratteggiata)
    if (micCal != null && micCal!.corrections.isNotEmpty) {
      final sortedKeys = micCal!.corrections.keys.toList()..sort();
      final calPath = Path();
      bool first = true;
      for (final freq in sortedKeys) {
        final corr = micCal!.corrections[freq]!;
        final x = _freqToX(freq, w);
        final y = _dbToY(corr, h);
        if (first) {
          calPath.moveTo(x, y);
          first = false;
        } else {
          calPath.lineTo(x, y);
        }
      }
      // Simula linea tratteggiata con intervalli
      canvas.drawPath(
          calPath,
          Paint()
            ..color = Colors.white38
            ..strokeWidth = 1.0
            ..style = PaintingStyle.stroke
            ..strokeCap = StrokeCap.round);
    }
  }

  @override
  bool shouldRepaint(covariant _FrequencyResponsePainter oldDelegate) => true;
}

class _SourceCard extends StatelessWidget {
  final String title;
  final String subtitle;
  final IconData icon;
  final bool selected;
  final VoidCallback onTap;

  const _SourceCard({
    required this.title,
    required this.subtitle,
    required this.icon,
    required this.selected,
    required this.onTap,
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(16),
        decoration: BoxDecoration(
          color: selected
              ? const Color(0xFF1a0f1e)
              : const Color(0xFF16213e),
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color:
                selected ? const Color(0xFFe94560) : Colors.white12,
            width: selected ? 2 : 1,
          ),
        ),
        child: Row(
          children: [
            Icon(icon,
                color: selected
                    ? const Color(0xFFe94560)
                    : Colors.white54,
                size: 32),
            const SizedBox(width: 16),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    title,
                    style: TextStyle(
                      color: selected ? Colors.white : Colors.white70,
                      fontWeight: selected
                          ? FontWeight.bold
                          : FontWeight.normal,
                    ),
                  ),
                  Text(subtitle,
                      style: const TextStyle(
                          color: Colors.white54, fontSize: 12)),
                ],
              ),
            ),
            if (selected)
              const Icon(Icons.check_circle,
                  color: Color(0xFFe94560)),
          ],
        ),
      ),
    );
  }
}
