import 'package:flutter/material.dart';

class VenueMap {
  final String name;
  final double widthMeters;
  final double heightMeters;
  final List<Map<String, double>> perimeterPoints;
  final List<VenueElement> elements;
  final Map<String, double> listenerPosition;

  VenueMap({
    required this.name,
    required this.widthMeters,
    required this.heightMeters,
    required this.perimeterPoints,
    required this.elements,
    required this.listenerPosition,
  });

  factory VenueMap.empty() => VenueMap(
        name: 'Nuova Mappa',
        widthMeters: 20.0,
        heightMeters: 15.0,
        perimeterPoints: [],
        elements: [],
        listenerPosition: {'x': 10.0, 'y': 7.5},
      );

  factory VenueMap.fromJson(Map<String, dynamic> json) => VenueMap(
        name: json['name'] as String? ?? 'Mappa',
        widthMeters: (json['widthMeters'] as num?)?.toDouble() ?? 20.0,
        heightMeters: (json['heightMeters'] as num?)?.toDouble() ?? 15.0,
        perimeterPoints: (json['perimeterPoints'] as List<dynamic>?)
                ?.map((e) => Map<String, double>.from(
                    (e as Map).map((k, v) => MapEntry(k as String, (v as num).toDouble()))))
                .toList() ??
            [],
        elements: (json['elements'] as List<dynamic>?)
                ?.map((e) => VenueElement.fromJson(e as Map<String, dynamic>))
                .toList() ??
            [],
        listenerPosition: json['listenerPosition'] != null
            ? Map<String, double>.from((json['listenerPosition'] as Map)
                .map((k, v) => MapEntry(k as String, (v as num).toDouble())))
            : {'x': 10.0, 'y': 7.5},
      );

  Map<String, dynamic> toJson() => {
        'name': name,
        'widthMeters': widthMeters,
        'heightMeters': heightMeters,
        'perimeterPoints': perimeterPoints,
        'elements': elements.map((e) => e.toJson()).toList(),
        'listenerPosition': listenerPosition,
      };
}

class VenueElement {
  final String id;
  final String type;
  final int deviceId;
  double posX;
  double posY;
  double rotation;
  final String label;

  VenueElement({
    required this.id,
    required this.type,
    required this.deviceId,
    required this.posX,
    required this.posY,
    this.rotation = 0.0,
    required this.label,
  });

  factory VenueElement.fromJson(Map<String, dynamic> json) => VenueElement(
        id: json['id'] as String? ?? '',
        type: json['type'] as String? ?? 'speaker_2way',
        deviceId: json['deviceId'] as int? ?? 0,
        posX: (json['posX'] as num?)?.toDouble() ?? 0.0,
        posY: (json['posY'] as num?)?.toDouble() ?? 0.0,
        rotation: (json['rotation'] as num?)?.toDouble() ?? 0.0,
        label: json['label'] as String? ?? '',
      );

  Map<String, dynamic> toJson() => {
        'id': id,
        'type': type,
        'deviceId': deviceId,
        'posX': posX,
        'posY': posY,
        'rotation': rotation,
        'label': label,
      };

  IconData get icon {
    switch (type) {
      case 'speaker_sub':
        return Icons.speaker;
      case 'par_rgb':
        return Icons.lightbulb;
      case 'moving_head':
        return Icons.highlight;
      default:
        return Icons.speaker_group;
    }
  }
}
