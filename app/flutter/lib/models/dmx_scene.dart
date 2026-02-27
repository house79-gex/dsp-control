// Modello scena DMX512
class DmxScene {
  final int id;
  final String name;
  final Map<int, List<int>> fixtureValues; // fixtureId → valori canali
  final double fadeInTime;
  final double fadeOutTime;
  final double holdTime;
  final bool loop;
  final double speed;
  final bool isAudioReactive;
  final bool isFactory;

  const DmxScene({
    required this.id,
    required this.name,
    this.fixtureValues = const {},
    this.fadeInTime    = 0,
    this.fadeOutTime   = 0,
    this.holdTime      = 0,
    this.loop          = false,
    this.speed         = 1.0,
    this.isAudioReactive = false,
    this.isFactory     = false,
  });

  factory DmxScene.fromJson(Map<String, dynamic> json) => DmxScene(
        id:             json['id']          as int? ?? 0,
        name:           json['name']        as String? ?? 'Scena',
        fadeInTime:     (json['fadeInTime'] as num?)?.toDouble() ?? 0,
        fadeOutTime:    (json['fadeOutTime'] as num?)?.toDouble() ?? 0,
        holdTime:       (json['holdTime']   as num?)?.toDouble() ?? 0,
        loop:           json['loop']        as bool? ?? false,
        speed:          (json['speed']      as num?)?.toDouble() ?? 1.0,
        isAudioReactive:json['isAudioReactive'] as bool? ?? false,
        isFactory:      json['isFactory']   as bool? ?? false,
      );

  Map<String, dynamic> toJson() => {
        'id':          id,
        'name':        name,
        'fadeInTime':  fadeInTime,
        'fadeOutTime': fadeOutTime,
        'holdTime':    holdTime,
        'loop':        loop,
        'speed':       speed,
        'isFactory':   isFactory,
      };
}
