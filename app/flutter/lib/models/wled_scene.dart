// Modello WledScene (corrisponde alla struct C++ WledScene)
class WledScene {
  final int id;
  final String name;
  final int effectId;
  final int r, g, b;
  final int r2, g2, b2;
  final int brightness;
  final int speed;
  final int intensity;
  final bool mirror;
  final bool reverse;

  const WledScene({
    required this.id,
    required this.name,
    required this.effectId,
    required this.r,
    required this.g,
    required this.b,
    required this.r2,
    required this.g2,
    required this.b2,
    required this.brightness,
    required this.speed,
    required this.intensity,
    required this.mirror,
    required this.reverse,
  });

  factory WledScene.fromJson(Map<String, dynamic> json) {
    return WledScene(
      id:         json['id']         as int? ?? 0,
      name:       json['name']       as String? ?? '',
      effectId:   json['effect_id']  as int? ?? 0,
      r:          json['r']          as int? ?? 255,
      g:          json['g']          as int? ?? 255,
      b:          json['b']          as int? ?? 255,
      r2:         json['r2']         as int? ?? 0,
      g2:         json['g2']         as int? ?? 0,
      b2:         json['b2']         as int? ?? 0,
      brightness: json['brightness'] as int? ?? 255,
      speed:      json['speed']      as int? ?? 128,
      intensity:  json['intensity']  as int? ?? 128,
      mirror:     json['mirror']     as bool? ?? false,
      reverse:    json['reverse']    as bool? ?? false,
    );
  }

  Map<String, dynamic> toJson() => {
    'id':         id,
    'name':       name,
    'effect_id':  effectId,
    'r': r, 'g': g, 'b': b,
    'r2': r2, 'g2': g2, 'b2': b2,
    'brightness': brightness,
    'speed':      speed,
    'intensity':  intensity,
    'mirror':     mirror,
    'reverse':    reverse,
  };
}
