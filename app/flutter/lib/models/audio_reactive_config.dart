// Configurazione banda frequenza per audio-reactive
class BandConfig {
  final double minFreq;
  final double maxFreq;
  final List<int> targetFixtureIds;
  final int colorR;
  final int colorG;
  final int colorB;
  final double sensitivity;
  final double attackMs;
  final double decayMs;
  final String blendMode; // "override", "add", "multiply"

  const BandConfig({
    required this.minFreq,
    required this.maxFreq,
    this.targetFixtureIds = const [],
    this.colorR      = 255,
    this.colorG      = 0,
    this.colorB      = 0,
    this.sensitivity = 1.0,
    this.attackMs    = 10.0,
    this.decayMs     = 200.0,
    this.blendMode   = 'override',
  });

  factory BandConfig.fromJson(Map<String, dynamic> json) => BandConfig(
        minFreq:  (json['minFreq']  as num?)?.toDouble() ?? 0,
        maxFreq:  (json['maxFreq']  as num?)?.toDouble() ?? 1000,
        colorR:   json['colorR']    as int? ?? 255,
        colorG:   json['colorG']    as int? ?? 0,
        colorB:   json['colorB']    as int? ?? 0,
        sensitivity:(json['sensitivity'] as num?)?.toDouble() ?? 1.0,
        attackMs: (json['attackMs'] as num?)?.toDouble() ?? 10.0,
        decayMs:  (json['decayMs']  as num?)?.toDouble() ?? 200.0,
        blendMode:json['blendMode'] as String? ?? 'override',
      );

  Map<String, dynamic> toJson() => {
        'minFreq': minFreq, 'maxFreq': maxFreq,
        'colorR':  colorR,  'colorG':  colorG,  'colorB': colorB,
        'sensitivity': sensitivity,
        'attackMs': attackMs, 'decayMs': decayMs,
        'blendMode': blendMode,
      };
}

// Configurazione scenario audio-reactive completo
class AudioReactiveConfig {
  final int id;
  final String name;
  final Map<String, BandConfig> bands; // chiave: "sub_bass", "bass", "low_mid", "mid", "high_mid", "treble"
  final bool isFactory;

  const AudioReactiveConfig({
    required this.id,
    required this.name,
    this.bands     = const {},
    this.isFactory = false,
  });

  factory AudioReactiveConfig.fromJson(Map<String, dynamic> json) => AudioReactiveConfig(
        id:        json['id']        as int? ?? 0,
        name:      json['name']      as String? ?? '',
        isFactory: json['isFactory'] as bool? ?? false,
      );

  Map<String, dynamic> toJson() => {
        'id': id, 'name': name, 'isFactory': isFactory,
      };
}
