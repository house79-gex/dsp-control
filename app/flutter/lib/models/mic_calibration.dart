/// Modello curva di calibrazione microfono di misura
/// Supporta formato .cal standard (Dayton/UMIK/REW)
class MicCalibration {
  final String micName;
  final Map<double, double> corrections; // freq Hz → correzione dB

  MicCalibration({required this.micName, required this.corrections});

  /// Interpola la correzione per una frequenza qualsiasi
  double getCorrectionAt(double freqHz) {
    final sortedKeys = corrections.keys.toList()..sort();
    if (sortedKeys.isEmpty) return 0.0;
    if (freqHz <= sortedKeys.first) return corrections[sortedKeys.first]!;
    if (freqHz >= sortedKeys.last) return corrections[sortedKeys.last]!;

    for (int i = 0; i < sortedKeys.length - 1; i++) {
      if (freqHz >= sortedKeys[i] && freqHz <= sortedKeys[i + 1]) {
        final ratio =
            (freqHz - sortedKeys[i]) / (sortedKeys[i + 1] - sortedKeys[i]);
        return corrections[sortedKeys[i]]! +
            ratio *
                (corrections[sortedKeys[i + 1]]! - corrections[sortedKeys[i]]!);
      }
    }
    return 0.0;
  }

  /// Carica da file .cal (formato REW/Dayton/UMIK standard)
  /// Formato: una riga per punto, "frequenza_Hz  correzione_dB"
  /// Le righe vuote, che iniziano con " o * vengono ignorate
  factory MicCalibration.fromCalFile(String name, String calFileContent) {
    final corrections = <double, double>{};
    for (final line in calFileContent.split('\n')) {
      final trimmed = line.trim();
      if (trimmed.isEmpty ||
          trimmed.startsWith('"') ||
          trimmed.startsWith('*')) continue;
      final parts = trimmed.split(RegExp(r'\s+'));
      if (parts.length >= 2) {
        final freq = double.tryParse(parts[0]);
        final db = double.tryParse(parts[1]);
        if (freq != null && db != null) corrections[freq] = db;
      }
    }
    return MicCalibration(micName: name, corrections: corrections);
  }

  /// Converte in JSON per invio all'ESP32
  Map<String, dynamic> toJson() => {
        'mic_name': micName,
        'points': corrections.entries
            .map((e) => {'freq': e.key, 'correction': e.value})
            .toList(),
      };

  /// Costruisce da JSON (risposta ESP32)
  factory MicCalibration.fromJson(Map<String, dynamic> json) {
    final points = (json['points'] as List?) ?? [];
    final corrections = <double, double>{};
    for (final p in points) {
      corrections[(p['freq'] as num).toDouble()] =
          (p['correction'] as num).toDouble();
    }
    return MicCalibration(
      micName: json['mic_name'] as String? ?? 'Sconosciuto',
      corrections: corrections,
    );
  }
}
