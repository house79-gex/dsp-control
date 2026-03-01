// Modello WledController (corrisponde alla struct C++ WledController)
class WledController {
  final int id;
  final String name;
  final String ip;
  final int numChannels;
  final bool online;
  final bool syncEnabled;
  final int lastSeenMs;

  const WledController({
    required this.id,
    required this.name,
    required this.ip,
    required this.numChannels,
    required this.online,
    required this.syncEnabled,
    required this.lastSeenMs,
  });

  factory WledController.fromJson(Map<String, dynamic> json) {
    return WledController(
      id:           json['id']           as int? ?? 0,
      name:         json['name']         as String? ?? '',
      ip:           json['ip']           as String? ?? '',
      numChannels:  json['num_channels'] as int? ?? 2,
      online:       json['online']       as bool? ?? false,
      syncEnabled:  json['sync_enabled'] as bool? ?? false,
      lastSeenMs:   json['last_seen_ms'] as int? ?? 0,
    );
  }

  Map<String, dynamic> toJson() => {
    'id':           id,
    'name':         name,
    'ip':           ip,
    'num_channels': numChannels,
    'online':       online,
    'sync_enabled': syncEnabled,
    'last_seen_ms': lastSeenMs,
  };
}
