import 'package:flutter/material.dart';
import '../models/venue_map.dart';
import '../services/api_client.dart';

class VenueMapScreen extends StatefulWidget {
  final ApiClient api;
  const VenueMapScreen({super.key, required this.api});

  @override
  State<VenueMapScreen> createState() => _VenueMapScreenState();
}

class _VenueMapScreenState extends State<VenueMapScreen> {
  VenueMap _map = VenueMap.empty();
  VenueElement? _selected;
  bool _loading = false;
  String _statusMsg = '';

  // Tipi di elemento da aggiungere
  static const _elementTypes = [
    ('speaker_2way', 'Cassa 2-vie', Icons.speaker_group),
    ('speaker_3way', 'Cassa 3-vie', Icons.speaker),
    ('speaker_sub', 'Subwoofer', Icons.speaker),
    ('par_rgb', 'PAR RGB', Icons.lightbulb),
    ('moving_head', 'Moving Head', Icons.highlight),
  ];

  @override
  void initState() {
    super.initState();
    _loadMap();
  }

  Future<void> _loadMap() async {
    setState(() { _loading = true; });
    try {
      final data = await widget.api.getVenueMap();
      if (data.isNotEmpty) {
        setState(() { _map = VenueMap.fromJson(data); });
      }
    } catch (_) {
      // Mappa non ancora salvata, usa default
    } finally {
      setState(() { _loading = false; });
    }
  }

  Future<void> _saveMap() async {
    try {
      await widget.api.saveVenueMap(_map.toJson());
      setState(() { _statusMsg = 'Mappa salvata'; });
    } catch (e) {
      setState(() { _statusMsg = 'Errore salvataggio: $e'; });
    }
  }

  Future<void> _calculateDelays() async {
    try {
      setState(() { _loading = true; });
      final speakers = _map.elements
          .where((e) => e.type.startsWith('speaker'))
          .map((e) => {'id': e.deviceId, 'x': e.posX, 'y': e.posY})
          .toList();
      final result = await widget.api.calculateVenueDelays(
        speakers: speakers,
        listenerX: _map.listenerPosition['x'] ?? 0,
        listenerY: _map.listenerPosition['y'] ?? 0,
      );
      setState(() {
        _statusMsg =
            'Delay calcolati e applicati: ${(result['delays'] as List?)?.length ?? 0} casse';
      });
    } catch (e) {
      setState(() { _statusMsg = 'Errore calcolo delay: $e'; });
    } finally {
      setState(() { _loading = false; });
    }
  }

  void _addElement(String type, String label) {
    final el = VenueElement(
      id: DateTime.now().millisecondsSinceEpoch.toString(),
      type: type,
      deviceId: _map.elements.length + 1,
      posX: _map.widthMeters / 2,
      posY: _map.heightMeters / 2,
      label: label,
    );
    setState(() {
      _map = VenueMap(
        name: _map.name,
        widthMeters: _map.widthMeters,
        heightMeters: _map.heightMeters,
        perimeterPoints: _map.perimeterPoints,
        elements: [..._map.elements, el],
        listenerPosition: _map.listenerPosition,
      );
      _selected = el;
    });
  }

  void _removeSelected() {
    if (_selected == null) return;
    final removedId = _selected!.id;
    setState(() {
      _map = VenueMap(
        name: _map.name,
        widthMeters: _map.widthMeters,
        heightMeters: _map.heightMeters,
        perimeterPoints: _map.perimeterPoints,
        elements: _map.elements.where((e) => e.id != removedId).toList(),
        listenerPosition: _map.listenerPosition,
      );
      _selected = null;
    });
  }

  @override
  Widget build(BuildContext context) {
    final isTablet = MediaQuery.of(context).size.width >= 600;

    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213e),
        title: Text('Mappa Venue: ${_map.name}',
            style: const TextStyle(color: Colors.white)),
        actions: [
          IconButton(
            icon: const Icon(Icons.save, color: Color(0xFFe94560)),
            tooltip: 'Salva mappa',
            onPressed: _saveMap,
          ),
          IconButton(
            icon: const Icon(Icons.calculate, color: Color(0xFFe94560)),
            tooltip: 'Calcola delay',
            onPressed: _calculateDelays,
          ),
        ],
      ),
      body: _loading
          ? const Center(
              child: CircularProgressIndicator(color: Color(0xFFe94560)))
          : isTablet
              ? _buildTabletLayout()
              : _buildPhoneLayout(),
    );
  }

  Widget _buildTabletLayout() {
    return Row(
      children: [
        Expanded(flex: 2, child: _buildMapCanvas()),
        Container(width: 1, color: Colors.white12),
        SizedBox(width: 280, child: _buildSidePanel()),
      ],
    );
  }

  Widget _buildPhoneLayout() {
    return Column(
      children: [
        Expanded(child: _buildMapCanvas()),
        Container(
          color: const Color(0xFF16213e),
          padding: const EdgeInsets.all(8),
          child: _buildControlBar(),
        ),
      ],
    );
  }

  Widget _buildMapCanvas() {
    return GestureDetector(
      onTapDown: (details) {
        final box = context.findRenderObject() as RenderBox?;
        if (box == null) return;
        final local = details.localPosition;
        final size = box.size;
        // Cerca elemento tap
        for (final el in _map.elements.reversed) {
          final ex = el.posX / _map.widthMeters * size.width;
          final ey = el.posY / _map.heightMeters * size.height;
          if ((local.dx - ex).abs() < 24 && (local.dy - ey).abs() < 24) {
            setState(() => _selected = el);
            return;
          }
        }
        setState(() => _selected = null);
      },
      child: CustomPaint(
        painter: _VenueMapPainter(map: _map, selected: _selected),
        child: Container(color: Colors.transparent),
      ),
    );
  }

  Widget _buildSidePanel() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(12),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text('Aggiungi elemento',
              style: TextStyle(
                  color: Colors.white, fontWeight: FontWeight.bold)),
          const SizedBox(height: 8),
          ..._elementTypes.map((t) => ListTile(
                leading: Icon(t.$3, color: const Color(0xFFe94560)),
                title: Text(t.$2,
                    style:
                        const TextStyle(color: Colors.white, fontSize: 13)),
                onTap: () => _addElement(t.$1, t.$2),
                dense: true,
              )),
          const Divider(color: Colors.white24),
          if (_selected != null) ...[
            Text('Selezionato: ${_selected!.label}',
                style:
                    const TextStyle(color: Colors.white70, fontSize: 12)),
            Text(
                'Pos: (${_selected!.posX.toStringAsFixed(1)}m, ${_selected!.posY.toStringAsFixed(1)}m)',
                style:
                    const TextStyle(color: Colors.white54, fontSize: 12)),
            Text('ID device: ${_selected!.deviceId}',
                style:
                    const TextStyle(color: Colors.white54, fontSize: 12)),
            const SizedBox(height: 8),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(
                  backgroundColor: const Color(0xFFCC0000)),
              icon: const Icon(Icons.delete, size: 16),
              label: const Text('Rimuovi'),
              onPressed: _removeSelected,
            ),
          ],
          const Divider(color: Colors.white24),
          const Text('Mappa',
              style: TextStyle(
                  color: Colors.white, fontWeight: FontWeight.bold)),
          const SizedBox(height: 4),
          Text(
              '${_map.widthMeters.toStringAsFixed(0)}m × ${_map.heightMeters.toStringAsFixed(0)}m',
              style:
                  const TextStyle(color: Colors.white54, fontSize: 12)),
          Text('${_map.elements.length} elementi',
              style:
                  const TextStyle(color: Colors.white54, fontSize: 12)),
          const SizedBox(height: 8),
          if (_statusMsg.isNotEmpty)
            Container(
              padding: const EdgeInsets.all(8),
              decoration: BoxDecoration(
                color: const Color(0xFF0f3460),
                borderRadius: BorderRadius.circular(6),
              ),
              child: Text(_statusMsg,
                  style:
                      const TextStyle(color: Colors.white70, fontSize: 12)),
            ),
        ],
      ),
    );
  }

  Widget _buildControlBar() {
    return Row(
      children: [
        Expanded(
          child: SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            child: Row(
              children: _elementTypes
                  .map((t) => Padding(
                        padding: const EdgeInsets.only(right: 8),
                        child: ElevatedButton.icon(
                          style: ElevatedButton.styleFrom(
                            backgroundColor: const Color(0xFF0f3460),
                            padding: const EdgeInsets.symmetric(
                                horizontal: 12, vertical: 8),
                          ),
                          icon: Icon(t.$3,
                              size: 16,
                              color: const Color(0xFFe94560)),
                          label: Text(t.$2,
                              style: const TextStyle(
                                  color: Colors.white, fontSize: 12)),
                          onPressed: () => _addElement(t.$1, t.$2),
                        ),
                      ))
                  .toList(),
            ),
          ),
        ),
        if (_selected != null)
          IconButton(
            icon: const Icon(Icons.delete, color: Color(0xFFCC0000)),
            onPressed: _removeSelected,
          ),
      ],
    );
  }
}

class _VenueMapPainter extends CustomPainter {
  final VenueMap map;
  final VenueElement? selected;

  _VenueMapPainter({required this.map, this.selected});

  @override
  void paint(Canvas canvas, Size size) {
    final bgPaint = Paint()..color = const Color(0xFF0f3460);
    canvas.drawRect(Offset.zero & size, bgPaint);

    // Griglia
    final gridPaint = Paint()
      ..color = Colors.white.withOpacity(0.08)
      ..strokeWidth = 0.5;
    final cellW = size.width / map.widthMeters;
    final cellH = size.height / map.heightMeters;
    for (int x = 0; x <= map.widthMeters.toInt(); x++) {
      canvas.drawLine(
          Offset(x * cellW, 0), Offset(x * cellW, size.height), gridPaint);
    }
    for (int y = 0; y <= map.heightMeters.toInt(); y++) {
      canvas.drawLine(
          Offset(0, y * cellH), Offset(size.width, y * cellH), gridPaint);
    }

    // Punto ascolto
    final lx =
        (map.listenerPosition['x'] ?? 0) / map.widthMeters * size.width;
    final ly =
        (map.listenerPosition['y'] ?? 0) / map.heightMeters * size.height;
    final listenerPaint = Paint()
      ..color = const Color(0xFF00e676)
      ..style = PaintingStyle.fill;
    canvas.drawCircle(Offset(lx, ly), 8, listenerPaint);

    // Elementi
    final textPainter = TextPainter(textDirection: TextDirection.ltr);
    for (final el in map.elements) {
      final ex = el.posX / map.widthMeters * size.width;
      final ey = el.posY / map.heightMeters * size.height;
      final isSelected = selected?.id == el.id;

      final elPaint = Paint()
        ..color = isSelected
            ? const Color(0xFFe94560)
            : (el.type.startsWith('speaker')
                ? const Color(0xFF4fc3f7)
                : const Color(0xFFffd740))
        ..style = PaintingStyle.fill;

      canvas.drawCircle(
          Offset(ex, ey), isSelected ? 20 : 16, elPaint);

      // Testo label
      textPainter.text = TextSpan(
        text: el.label.isNotEmpty ? el.label[0] : '?',
        style: const TextStyle(
            color: Colors.white,
            fontSize: 12,
            fontWeight: FontWeight.bold),
      );
      textPainter.layout();
      textPainter.paint(
          canvas,
          Offset(
              ex - textPainter.width / 2, ey - textPainter.height / 2));

      // Label sotto
      textPainter.text = TextSpan(
        text: el.label,
        style: const TextStyle(color: Colors.white70, fontSize: 9),
      );
      textPainter.layout();
      textPainter.paint(
          canvas, Offset(ex - textPainter.width / 2, ey + 20));
    }
  }

  @override
  bool shouldRepaint(_VenueMapPainter old) =>
      old.map != map || old.selected != selected;
}
