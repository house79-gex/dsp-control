// Modello fixture DMX512
class DmxFixture {
  final int id;
  final String name;
  final String type;
  final int startAddress;
  final int numChannels;
  final int groupId;
  final String groupName;
  final List<int> channels;
  final List<String> channelNames;
  final bool online;

  const DmxFixture({
    required this.id,
    required this.name,
    required this.type,
    required this.startAddress,
    required this.numChannels,
    this.groupId = 0,
    this.groupName = '',
    required this.channels,
    this.channelNames = const [],
    this.online = true,
  });

  factory DmxFixture.fromJson(Map<String, dynamic> json) => DmxFixture(
        id:           json['id']           as int? ?? 0,
        name:         json['name']         as String? ?? 'Fixture',
        type:         json['type']         as String? ?? 'PAR_RGB',
        startAddress: json['startAddress'] as int? ?? 1,
        numChannels:  json['numChannels']  as int? ?? 3,
        groupId:      json['groupId']      as int? ?? 0,
        groupName:    json['groupName']    as String? ?? '',
        channels:     (json['channels'] as List<dynamic>?)?.cast<int>() ?? List.filled(json['numChannels'] as int? ?? 3, 0),
        channelNames: (json['channelNames'] as List<dynamic>?)?.cast<String>() ?? [],
        online:       json['online']       as bool? ?? true,
      );

  Map<String, dynamic> toJson() => {
        'id':           id,
        'name':         name,
        'type':         type,
        'startAddress': startAddress,
        'numChannels':  numChannels,
        'groupId':      groupId,
        'groupName':    groupName,
        'channels':     channels,
        'online':       online,
      };

  // Colore RGB corrente (dai canali 0,1,2)
  int get colorR => channels.isNotEmpty ? channels[0] : 0;
  int get colorG => channels.length > 1 ? channels[1] : 0;
  int get colorB => channels.length > 2 ? channels[2] : 0;

  DmxFixture copyWith({
    int? id, String? name, String? type, int? startAddress,
    int? numChannels, int? groupId, String? groupName,
    List<int>? channels, List<String>? channelNames, bool? online,
  }) => DmxFixture(
        id:           id           ?? this.id,
        name:         name         ?? this.name,
        type:         type         ?? this.type,
        startAddress: startAddress ?? this.startAddress,
        numChannels:  numChannels  ?? this.numChannels,
        groupId:      groupId      ?? this.groupId,
        groupName:    groupName    ?? this.groupName,
        channels:     channels     ?? this.channels,
        channelNames: channelNames ?? this.channelNames,
        online:       online       ?? this.online,
      );
}
