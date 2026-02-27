// Modello preset DSP CQ260D
class DspPreset {
  final String name;
  final String description;
  final Map<String, dynamic> parameters;
  final bool isFactory;
  final bool isModifiable;

  const DspPreset({
    required this.name,
    this.description   = '',
    this.parameters    = const {},
    this.isFactory     = false,
    this.isModifiable  = true,
  });

  factory DspPreset.fromJson(Map<String, dynamic> json) => DspPreset(
        name:        json['name']        as String? ?? '',
        description: json['description'] as String? ?? '',
        parameters:  json['parameters']  as Map<String, dynamic>? ?? {},
        isFactory:   json['isFactory']   as bool? ?? false,
        isModifiable:!(json['isFactory'] as bool? ?? false),
      );

  Map<String, dynamic> toJson() => {
        'name':        name,
        'description': description,
        'parameters':  parameters,
        'isFactory':   isFactory,
      };
}
