import 'package:flutter/material.dart';
import '../models/wled_controller.dart';
import '../models/wled_scene.dart';
import '../services/api_client.dart';

// ======= Nomi zone =======
const _zoneNames = ['Logo SX', 'Logo DX', 'Front Frame', 'Spare'];

// ======= Schermata Neon/WLED =======

class WledScreen extends StatefulWidget {
  final ApiClient api;
  const WledScreen({super.key, required this.api});

  @override
  State<WledScreen> createState() => _WledScreenState();
}

class _WledScreenState extends State<WledScreen> {
  List<WledController> _controllers = [];
  List<WledScene>      _scenes      = [];
  int    _selCtrl   = 0;
  int    _selZone   = 0;
  double _brightness = 100;
  double _speed      = 128;
  double _intensity  = 128;
  int    _r = 255, _g = 0, _b = 128;
  int    _effectId = 0;
  bool   _loading  = false;
  String? _error;

  static const _effectNames = [
    'Solid', 'Blink', 'Breathe', 'Wipe', 'Wipe Random',
    'Random Colors', 'Sweep', 'Dynamic', 'Colorloop',
    'Rainbow', 'Scan', 'Scan Dual', 'Fade', 'Theater',
    'Theater Rainbow', 'Running', 'Saw', 'Twinkle',
    'Dissolve', 'Dissolve Rnd', 'Sparkle', 'Sparkle Dark',
    'Sparkle+', 'Strobe', 'Strobe Rainbow', 'Multi Strobe',
    'Blink Rainbow', 'Android', 'Chase', 'Chase Random',
    'Chase Rainbow', 'Chase Flash', 'Chase Flash Rnd',
    'Rainbow Runner', 'Colorful', 'Traffic Light',
    'Color Sweep Rnd', 'Running 2', 'Aurora', 'Stream',
    'Scanner', 'Lighthouse', 'Fireworks', 'Rain',
    'Merry Christmas', 'Fire Flicker', 'Gradient', 'Loading',
    'Police', 'Police All', 'Two Dots', 'Two Areas',
    'Circus', 'Halloween', 'Tri Chase', 'Tri Wipe',
    'Tri Fade', 'Lightning', 'ICU', 'Multi Comet',
    'Scanner Dual', 'Stream 2', 'Oscillate', 'Pride 2015',
    'Juggle', 'Palette', 'Fire 2012', 'Colorwaves',
    'Bpm', 'Fill Noise', 'Noise 1', 'Noise 2', 'Noise 3',
    'Noise 4', 'Colortwinkles', 'Lake', 'Meteor',
    'Meteor Smooth', 'Railway', 'Ripple', 'Twinklefox',
    'Twinklecat', 'Halloween Eyes', 'Solid Pattern',
    'Solid Pattern Tri', 'Spots', 'Spots Fade',
    'Glitter', 'Candle', 'Starburst', 'Exploding Fireworks',
    'Bouncingballs', 'Sinelon', 'Sinelon Dual',
    'Sinelon Rainbow', 'Popcorn', 'Drip', 'Plasma',
    'Percent', 'Ripple Rainbow', 'Heartbeat',
    'Pacifica', 'Candle Multi', 'Solid Glitter',
    'Sunrise', 'Phased', 'Twinkleup', 'Noise Pal',
    'Sine', 'Phased Noise', 'Flow', 'Chunchun',
    'Dancing Shadows', 'Washing Machine',
  ];

  @override
  void initState() {
    super.initState();
    _loadAll();
  }

  Future<void> _loadAll() async {
    setState(() { _loading = true; _error = null; });
    try {
      final ctrlData  = await widget.api.getWledControllers();
      final sceneData = await widget.api.getWledScenes();
      final ctrls = (ctrlData['controllers'] as List<dynamic>? ?? [])
          .map((e) => WledController.fromJson(e as Map<String, dynamic>))
          .toList();
      final scenes = (sceneData['scenes'] as List<dynamic>? ?? [])
          .map((e) => WledScene.fromJson(e as Map<String, dynamic>))
          .toList();
      setState(() { _controllers = ctrls; _scenes = scenes; });
    } catch (e) {
      setState(() { _error = e.toString(); });
    } finally {
      setState(() { _loading = false; });
    }
  }

  Future<void> _blackout() async {
    await widget.api.wledBlackout();
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('BLACKOUT attivato'), backgroundColor: Colors.red),
      );
    }
  }

  Future<void> _applyColor() async {
    try {
      await widget.api.setWledColor(
          _selCtrl, _selZone, _r, _g, _b);
      await widget.api.setWledBrightness(
          _selCtrl, _selZone, _brightness.round());
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Errore: $e'), backgroundColor: Colors.red),
        );
      }
    }
  }

  Future<void> _applyEffect() async {
    try {
      await widget.api.setWledEffect(
          _selCtrl, _selZone, _effectId, _speed.round(), _intensity.round());
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Errore: $e'), backgroundColor: Colors.red),
        );
      }
    }
  }

  Future<void> _applyScene(int idx) async {
    await widget.api.applyWledScene(idx, all: true);
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Scena "${_scenes[idx].name}" applicata')),
      );
    }
  }

  Future<void> _discover() async {
    await widget.api.wledDiscover();
    await _loadAll();
  }

  // ——— Builder ———

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213e),
        title: const Text('Neon LED', style: TextStyle(color: Colors.white)),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh, color: Colors.white),
            tooltip: 'Ricarica',
            onPressed: _loadAll,
          ),
          IconButton(
            icon: const Icon(Icons.wifi_find, color: Colors.white),
            tooltip: 'Discover WLED',
            onPressed: _discover,
          ),
        ],
      ),
      body: _loading
          ? const Center(child: CircularProgressIndicator(color: Color(0xFFe94560)))
          : _error != null
              ? Center(child: Text(_error!, style: const TextStyle(color: Colors.red)))
              : _buildBody(),
      floatingActionButton: FloatingActionButton.extended(
        backgroundColor: Colors.red,
        icon: const Icon(Icons.power_off),
        label: const Text('BLACKOUT'),
        onPressed: _blackout,
      ),
    );
  }

  Widget _buildBody() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          _buildControllerList(),
          const SizedBox(height: 16),
          _buildZoneSelector(),
          const SizedBox(height: 16),
          _buildColorControls(),
          const SizedBox(height: 16),
          _buildEffectControls(),
          const SizedBox(height: 16),
          _buildSceneGrid(),
          const SizedBox(height: 80), // FAB clearance
        ],
      ),
    );
  }

  Widget _buildControllerList() {
    return Card(
      color: const Color(0xFF16213e),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Controller', style: TextStyle(color: Colors.white70, fontSize: 12)),
            const SizedBox(height: 8),
            if (_controllers.isEmpty)
              const Text('Nessun controller. Premi Discover.',
                  style: TextStyle(color: Colors.white38))
            else
              ...List.generate(_controllers.length, (i) {
                final c = _controllers[i];
                return RadioListTile<int>(
                  value: i,
                  groupValue: _selCtrl,
                  onChanged: (v) => setState(() => _selCtrl = v!),
                  activeColor: const Color(0xFFe94560),
                  title: Text(c.name, style: const TextStyle(color: Colors.white)),
                  subtitle: Text(
                    '${c.ip}  ${c.numChannels}CH',
                    style: const TextStyle(color: Colors.white54, fontSize: 12),
                  ),
                  secondary: Icon(
                    Icons.circle,
                    size: 12,
                    color: c.online ? Colors.green : Colors.red,
                  ),
                );
              }),
            const SizedBox(height: 4),
            OutlinedButton.icon(
              icon: const Icon(Icons.sync, color: Color(0xFFe94560)),
              label: const Text('Sync tutti', style: TextStyle(color: Color(0xFFe94560))),
              style: OutlinedButton.styleFrom(side: const BorderSide(color: Color(0xFFe94560))),
              onPressed: () async {
                await widget.api.wledSyncAll();
                if (mounted) {
                  ScaffoldMessenger.of(context).showSnackBar(
                    const SnackBar(content: Text('Sync inviato')),
                  );
                }
              },
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildZoneSelector() {
    return Card(
      color: const Color(0xFF16213e),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Zona', style: TextStyle(color: Colors.white70, fontSize: 12)),
            const SizedBox(height: 8),
            Wrap(
              spacing: 8,
              children: List.generate(4, (i) {
                return ChoiceChip(
                  label: Text(_zoneNames[i]),
                  selected: _selZone == i,
                  selectedColor: const Color(0xFFe94560),
                  labelStyle: TextStyle(
                      color: _selZone == i ? Colors.white : Colors.white54),
                  onSelected: (_) => setState(() => _selZone = i),
                );
              }),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildColorControls() {
    return Card(
      color: const Color(0xFF16213e),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Colore & Luminosità',
                style: TextStyle(color: Colors.white70, fontSize: 12)),
            const SizedBox(height: 8),
            Row(
              children: [
                Container(
                  width: 48, height: 48,
                  decoration: BoxDecoration(
                    color: Color.fromARGB(255, _r, _g, _b),
                    borderRadius: BorderRadius.circular(8),
                    border: Border.all(color: Colors.white24),
                  ),
                ),
                const SizedBox(width: 16),
                Expanded(
                  child: Column(
                    children: [
                      _rgbSlider('R', _r, Colors.red,    (v) => setState(() => _r = v)),
                      _rgbSlider('G', _g, Colors.green,  (v) => setState(() => _g = v)),
                      _rgbSlider('B', _b, Colors.blue,   (v) => setState(() => _b = v)),
                    ],
                  ),
                ),
              ],
            ),
            const SizedBox(height: 8),
            _labeledSlider('Luminosità', _brightness, 0, 255, Colors.amber,
                (v) => setState(() => _brightness = v)),
            const SizedBox(height: 8),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
              icon: const Icon(Icons.colorize),
              label: const Text('Applica Colore'),
              onPressed: _applyColor,
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildEffectControls() {
    return Card(
      color: const Color(0xFF16213e),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Effetto', style: TextStyle(color: Colors.white70, fontSize: 12)),
            const SizedBox(height: 8),
            DropdownButton<int>(
              value: _effectId,
              dropdownColor: const Color(0xFF16213e),
              style: const TextStyle(color: Colors.white),
              isExpanded: true,
              items: List.generate(_effectNames.length, (i) {
                return DropdownMenuItem(value: i, child: Text('$i – ${_effectNames[i]}'));
              }),
              onChanged: (v) => setState(() => _effectId = v!),
            ),
            const SizedBox(height: 8),
            _labeledSlider('Velocità',   _speed,     0, 255, Colors.cyan,
                (v) => setState(() => _speed = v)),
            _labeledSlider('Intensità',  _intensity, 0, 255, Colors.purple,
                (v) => setState(() => _intensity = v)),
            const SizedBox(height: 8),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFF0055CC)),
              icon: const Icon(Icons.flash_on),
              label: const Text('Applica Effetto'),
              onPressed: _applyEffect,
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildSceneGrid() {
    return Card(
      color: const Color(0xFF16213e),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Scene', style: TextStyle(color: Colors.white70, fontSize: 12)),
            const SizedBox(height: 8),
            if (_scenes.isEmpty)
              const Text('Nessuna scena salvata.',
                  style: TextStyle(color: Colors.white38))
            else
              GridView.count(
                crossAxisCount: 3,
                shrinkWrap: true,
                physics: const NeverScrollableScrollPhysics(),
                mainAxisSpacing: 8,
                crossAxisSpacing: 8,
                childAspectRatio: 2.5,
                children: List.generate(_scenes.length, (i) {
                  final s = _scenes[i];
                  return ElevatedButton(
                    style: ElevatedButton.styleFrom(
                      backgroundColor: Color.fromARGB(255, s.r, s.g, s.b)
                          .withOpacity(0.7),
                      foregroundColor: Colors.white,
                    ),
                    onPressed: () => _applyScene(i),
                    child: Text(s.name,
                        overflow: TextOverflow.ellipsis,
                        style: const TextStyle(fontSize: 12)),
                  );
                }),
              ),
          ],
        ),
      ),
    );
  }

  // ——— Widget helpers ———

  Widget _rgbSlider(String label, int value, Color color, ValueChanged<int> onChanged) {
    return Row(
      children: [
        SizedBox(width: 16,
            child: Text(label,
                style: TextStyle(color: color, fontSize: 11,
                    fontWeight: FontWeight.bold))),
        Expanded(
          child: Slider(
            value: value.toDouble(),
            min: 0, max: 255,
            activeColor: color,
            inactiveColor: color.withOpacity(0.2),
            onChanged: (v) => onChanged(v.round()),
          ),
        ),
        SizedBox(width: 28,
            child: Text(value.toString(),
                style: const TextStyle(color: Colors.white54, fontSize: 11))),
      ],
    );
  }

  Widget _labeledSlider(String label, double value, double min, double max,
      Color color, ValueChanged<double> onChanged) {
    return Row(
      children: [
        SizedBox(width: 80,
            child: Text(label,
                style: const TextStyle(color: Colors.white70, fontSize: 12))),
        Expanded(
          child: Slider(
            value: value,
            min: min, max: max,
            activeColor: color,
            inactiveColor: color.withOpacity(0.2),
            onChanged: onChanged,
          ),
        ),
        SizedBox(width: 36,
            child: Text(value.round().toString(),
                style: const TextStyle(color: Colors.white54, fontSize: 12))),
      ],
    );
  }
}
