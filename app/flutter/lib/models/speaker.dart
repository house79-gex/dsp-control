class Speaker {
  final int id;
  String name;
  String type;       // "2WAY", "3WAY", "SUB"
  String position;   // "SX1", "DX1", ecc.
  String role;       // "SX", "DX", "SUB", "MONO"
  double volume;     // 0–100
  bool online;

  Speaker({
    required this.id,
    required this.name,
    required this.type,
    required this.position,
    required this.role,
    this.volume = 80,
    this.online = true,
  });

  factory Speaker.fromJson(Map<String, dynamic> json) => Speaker(
        id:       json['id'] as int,
        name:     json['name']     ?? 'Cassa ${json['id']}',
        type:     json['type']     ?? '2WAY',
        position: json['position'] ?? 'P1',
        role:     json['role']     ?? 'MONO',
        volume:   (json['volume']  ?? 80).toDouble(),
        online:   json['online']   ?? true,
      );

  Map<String, dynamic> toJson() => {
        'id':       id,
        'name':     name,
        'type':     type,
        'position': position,
        'role':     role,
        'volume':   volume,
        'online':   online,
      };
}
