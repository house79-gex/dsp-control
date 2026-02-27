import 'package:flutter/material.dart';
import '../models/dmx_fixture.dart';
import '../models/dmx_scene.dart';
import '../services/api_client.dart';

// ======= Schermata Luci/DMX =======

class DmxScreen extends StatefulWidget {
  final ApiClient api;
  const DmxScreen({super.key, required this.api});

  @override
  State<DmxScreen> createState() => _DmxScreenState();
}

class _DmxScreenState extends State<DmxScreen> with SingleTickerProviderStateMixin {
  late final TabController _tabCtrl;
  List<DmxFixture> _fixtures = [];
  List<DmxScene>   _scenes   = [];
  double  _masterDimmer      = 100;
  bool    _audioReactive     = false;
  List<double> _spectrumLevels = List.filled(6, 0);
  bool    _loading           = false;
  String? _error;

  @override
  void initState() {
    super.initState();
    _tabCtrl = TabController(length: 3, vsync: this);
    _loadAll();
    _startSpectrumPolling();
  }

  @override
  void dispose() {
    _tabCtrl.dispose();
    super.dispose();
  }

  Future<void> _loadAll() async {
    setState(() { _loading = true; _error = null; });
    try {
      final fixtures = await widget.api.getDmxFixtures();
      final scenes   = await widget.api.getDmxScenes();
      setState(() { _fixtures = fixtures; _scenes = scenes; });
    } catch (e) {
      setState(() { _error = e.toString(); });
    } finally {
      setState(() { _loading = false; });
    }
  }

  void _startSpectrumPolling() {
    Future.doWhile(() async {
      if (!mounted) return false;
      try {
        final data = await widget.api.getAudioSpectrum();
        final bands = (data['bands'] as List<dynamic>?)?.cast<num>() ?? [];
        if (mounted) {
          setState(() {
            _spectrumLevels = bands.map((b) => b.toDouble()).toList();
          });
        }
      } catch (_) {}
      await Future.delayed(const Duration(milliseconds: 100));
      return mounted;
    });
  }

  Future<void> _blackout() async {
    await widget.api.dmxBlackout();
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('BLACKOUT attivato'), backgroundColor: Colors.red),
      );
    }
  }

  Future<void> _toggleAudioReactive() async {
    final newState = !_audioReactive;
    await widget.api.enableAudioReactive(newState);
    setState(() { _audioReactive = newState; });
  }

  // ——— Selettore colore semplice ———
  Future<Color?> _pickColor(BuildContext context, Color initial) async {
    Color picked = initial;
    return showDialog<Color>(
      context: context,
      builder: (ctx) => AlertDialog(
        backgroundColor: const Color(0xFF16213e),
        title: const Text('Seleziona colore', style: TextStyle(color: Colors.white)),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            for (final color in [
              Colors.red, Colors.orange, Colors.yellow, Colors.green,
              Colors.cyan, Colors.blue, Colors.purple, Colors.white,
              Colors.amber, const Color(0xFFFF6B35),
            ])
              ListTile(
                leading: CircleAvatar(backgroundColor: color),
                title: Text(color.toString().split('(')[0], style: const TextStyle(color: Colors.white70)),
                onTap: () { picked = color; Navigator.pop(ctx, picked); },
              ),
          ],
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        title: const Text('💡 Luci DMX'),
        backgroundColor: const Color(0xFF1a1a2e),
        bottom: TabBar(
          controller: _tabCtrl,
          indicatorColor: const Color(0xFFe94560),
          tabs: const [
            Tab(text: 'Fixture'),
            Tab(text: 'Scene'),
            Tab(text: 'Audio-Reactive'),
          ],
        ),
        actions: [
          IconButton(icon: const Icon(Icons.refresh), onPressed: _loadAll),
        ],
      ),
      body: Column(
        children: [
          // Master dimmer + blackout
          Container(
            color: const Color(0xFF16213e),
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
            child: Row(
              children: [
                const Text('Master:', style: TextStyle(color: Colors.white70)),
                Expanded(
                  child: Slider(
                    value: _masterDimmer,
                    min: 0, max: 100,
                    activeColor: const Color(0xFFe94560),
                    onChanged: (v) => setState(() => _masterDimmer = v),
                    onChangeEnd: (v) => widget.api.setMasterDimmer(v.round() * 255 ~/ 100),
                  ),
                ),
                Text('${_masterDimmer.round()}%', style: const TextStyle(color: Colors.white70)),
                const SizedBox(width: 8),
                ElevatedButton(
                  style: ElevatedButton.styleFrom(backgroundColor: Colors.red.shade900, padding: const EdgeInsets.all(8)),
                  onPressed: _blackout,
                  child: const Text('⚫', style: TextStyle(fontSize: 18)),
                ),
              ],
            ),
          ),

          if (_error != null)
            Container(
              color: Colors.red.shade900,
              padding: const EdgeInsets.all(8),
              child: Text(_error!, style: const TextStyle(color: Colors.white)),
            ),

          Expanded(
            child: TabBarView(
              controller: _tabCtrl,
              children: [
                _buildFixtureTab(),
                _buildSceneTab(),
                _buildAudioReactiveTab(),
              ],
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildFixtureTab() {
    if (_loading) return const Center(child: CircularProgressIndicator(color: Color(0xFFe94560)));
    if (_fixtures.isEmpty) {
      return Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.lightbulb_outline, size: 64, color: Colors.white24),
            const SizedBox(height: 16),
            const Text('Nessuna fixture configurata', style: TextStyle(color: Colors.white38)),
            const SizedBox(height: 16),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
              icon: const Icon(Icons.add),
              label: const Text('Aggiungi fixture'),
              onPressed: () => _showAddFixtureDialog(),
            ),
          ],
        ),
      );
    }
    return RefreshIndicator(
      onRefresh: _loadAll,
      child: GridView.builder(
        padding: const EdgeInsets.all(12),
        gridDelegate: const SliverGridDelegateWithMaxCrossAxisExtent(
          maxCrossAxisExtent: 200,
          childAspectRatio: 0.85,
          crossAxisSpacing: 8,
          mainAxisSpacing: 8,
        ),
        itemCount: _fixtures.length + 1,
        itemBuilder: (ctx, i) {
          if (i == _fixtures.length) {
            return GestureDetector(
              onTap: _showAddFixtureDialog,
              child: Container(
                decoration: BoxDecoration(
                  color: const Color(0xFF16213e),
                  borderRadius: BorderRadius.circular(10),
                  border: Border.all(color: Colors.white24, style: BorderStyle.solid),
                ),
                child: const Center(child: Icon(Icons.add, size: 40, color: Colors.white38)),
              ),
            );
          }
          final fx = _fixtures[i];
          return _FixtureCard(fixture: fx, onColorTap: () async {
            final color = await _pickColor(context, Color.fromARGB(255, fx.colorR, fx.colorG, fx.colorB));
            if (color == null) return;
            await widget.api.setFixtureColor(fx.id, color.red, color.green, color.blue);
            await _loadAll();
          });
        },
      ),
    );
  }

  Widget _buildSceneTab() {
    if (_loading) return const Center(child: CircularProgressIndicator(color: Color(0xFFe94560)));
    return RefreshIndicator(
      onRefresh: _loadAll,
      child: ListView.builder(
        padding: const EdgeInsets.all(12),
        itemCount: _scenes.length,
        itemBuilder: (ctx, i) {
          final sc = _scenes[i];
          return Card(
            color: const Color(0xFF16213e),
            margin: const EdgeInsets.only(bottom: 8),
            child: ListTile(
              leading: const Icon(Icons.theaters, color: Color(0xFFe94560)),
              title: Text(sc.name, style: const TextStyle(color: Colors.white)),
              subtitle: Text(
                'Fade-in: ${sc.fadeInTime}s | Loop: ${sc.loop ? "Sì" : "No"}',
                style: const TextStyle(color: Colors.white54),
              ),
              trailing: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  IconButton(
                    icon: const Icon(Icons.play_arrow, color: Color(0xFFe94560)),
                    onPressed: () async {
                      await widget.api.playScene(sc.id);
                      if (mounted) {
                        ScaffoldMessenger.of(context).showSnackBar(
                          SnackBar(content: Text('Scena "${sc.name}" avviata')),
                        );
                      }
                    },
                  ),
                  if (!sc.isFactory)
                    IconButton(
                      icon: const Icon(Icons.delete_outline, color: Colors.white38),
                      onPressed: () async {
                        await widget.api.deleteDmxScene(sc.id);
                        await _loadAll();
                      },
                    ),
                ],
              ),
            ),
          );
        },
      ),
    );
  }

  Widget _buildAudioReactiveTab() {
    final bandNames = ['Sub Bass\n20-60Hz', 'Bass\n60-250Hz', 'Low Mid\n250-500Hz',
                       'Mid\n500-2kHz', 'High Mid\n2-6kHz', 'Treble\n6-20kHz'];
    final bandColors = [Colors.red, Colors.orange, Colors.yellow,
                        Colors.green, Colors.cyan, Colors.purple];

    return Padding(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            children: [
              const Text('Audio-Reactive:', style: TextStyle(color: Colors.white, fontSize: 16)),
              const SizedBox(width: 12),
              Switch(
                value: _audioReactive,
                activeColor: const Color(0xFFe94560),
                onChanged: (_) => _toggleAudioReactive(),
              ),
              Text(_audioReactive ? 'ATTIVO' : 'INATTIVO',
                   style: TextStyle(color: _audioReactive ? Colors.green : Colors.white38)),
            ],
          ),
          const SizedBox(height: 24),
          const Text('Spettro in tempo reale:', style: TextStyle(color: Colors.white70)),
          const SizedBox(height: 12),
          // Visualizzatore barre spettro
          SizedBox(
            height: 120,
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.end,
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: List.generate(_spectrumLevels.length, (i) {
                final level = i < _spectrumLevels.length ? _spectrumLevels[i] : 0.0;
                return Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 2),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.end,
                      children: [
                        AnimatedContainer(
                          duration: const Duration(milliseconds: 80),
                          height: 100 * level.clamp(0.0, 1.0),
                          decoration: BoxDecoration(
                            color: i < bandColors.length ? bandColors[i] : Colors.white,
                            borderRadius: BorderRadius.circular(3),
                          ),
                        ),
                        const SizedBox(height: 4),
                        Text(
                          i < bandNames.length ? bandNames[i] : '',
                          textAlign: TextAlign.center,
                          style: const TextStyle(color: Colors.white54, fontSize: 9),
                        ),
                      ],
                    ),
                  ),
                );
              }),
            ),
          ),
          const SizedBox(height: 24),
          const Text('Scenari disponibili:', style: TextStyle(color: Colors.white70)),
          const SizedBox(height: 8),
          Expanded(
            child: FutureBuilder<List<AudioReactiveConfig>>(
              future: widget.api.getAudioReactiveScenarios(),
              builder: (ctx, snap) {
                if (!snap.hasData) return const Center(child: CircularProgressIndicator(color: Color(0xFFe94560)));
                return ListView.builder(
                  itemCount: snap.data!.length,
                  itemBuilder: (ctx, i) {
                    final sc = snap.data![i];
                    return Card(
                      color: const Color(0xFF16213e),
                      child: ListTile(
                        title: Text(sc.name, style: const TextStyle(color: Colors.white)),
                        trailing: ElevatedButton(
                          style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
                          onPressed: () => widget.api.enableAudioReactive(true, scenarioId: sc.id),
                          child: const Text('Attiva'),
                        ),
                      ),
                    );
                  },
                );
              },
            ),
          ),
        ],
      ),
    );
  }

  // Trova il primo ID disponibile non usato tra le fixture esistenti
  int _nextFixtureId() {
    final usedIds = _fixtures.map((f) => f.id).toSet();
    for (int i = 1; i <= 512; i++) {
      if (!usedIds.contains(i)) return i;
    }
    return DateTime.now().millisecondsSinceEpoch;
  }

  void _showAddFixtureDialog() {
    final nameCtrl  = TextEditingController();
    final addrCtrl  = TextEditingController(text: '1');
    String type     = 'PAR_RGB';
    int numChannels = 3;

    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        backgroundColor: const Color(0xFF16213e),
        title: const Text('Aggiungi Fixture', style: TextStyle(color: Colors.white)),
        content: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              TextField(
                controller: nameCtrl,
                style: const TextStyle(color: Colors.white),
                decoration: const InputDecoration(
                  labelText: 'Nome fixture',
                  labelStyle: TextStyle(color: Colors.white54),
                  enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Colors.white24)),
                  focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color(0xFFe94560))),
                ),
              ),
              const SizedBox(height: 8),
              TextField(
                controller: addrCtrl,
                style: const TextStyle(color: Colors.white),
                keyboardType: TextInputType.number,
                decoration: const InputDecoration(
                  labelText: 'Indirizzo DMX (1-512)',
                  labelStyle: TextStyle(color: Colors.white54),
                  enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Colors.white24)),
                  focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color(0xFFe94560))),
                ),
              ),
            ],
          ),
        ),
        actions: [
          TextButton(onPressed: () => Navigator.pop(ctx), child: const Text('Annulla', style: TextStyle(color: Colors.white54))),
          ElevatedButton(
            style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
            onPressed: () async {
              final fixture = DmxFixture(
                id: _nextFixtureId(),
                name: nameCtrl.text.isEmpty ? 'Fixture' : nameCtrl.text,
                type: type,
                startAddress: int.tryParse(addrCtrl.text) ?? 1,
                numChannels: numChannels,
                channels: List.filled(numChannels, 0),
              );
              await widget.api.addDmxFixture(fixture);
              if (ctx.mounted) Navigator.pop(ctx);
              await _loadAll();
            },
            child: const Text('Aggiungi'),
          ),
        ],
      ),
    );
  }
}

// ——— Widget card fixture ———
class _FixtureCard extends StatelessWidget {
  final DmxFixture fixture;
  final VoidCallback onColorTap;

  const _FixtureCard({required this.fixture, required this.onColorTap});

  @override
  Widget build(BuildContext context) {
    final color = Color.fromARGB(255, fixture.colorR, fixture.colorG, fixture.colorB);
    return Card(
      color: const Color(0xFF16213e),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
      child: InkWell(
        onTap: onColorTap,
        borderRadius: BorderRadius.circular(10),
        child: Padding(
          padding: const EdgeInsets.all(12),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Container(
                width: 48, height: 48,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: color,
                  border: Border.all(color: Colors.white24, width: 2),
                  boxShadow: [BoxShadow(color: color.withOpacity(0.4), blurRadius: 8)],
                ),
              ),
              const SizedBox(height: 8),
              Text(fixture.name, textAlign: TextAlign.center,
                   style: const TextStyle(color: Colors.white, fontSize: 12, fontWeight: FontWeight.bold),
                   overflow: TextOverflow.ellipsis),
              Text(fixture.type, style: const TextStyle(color: Colors.white54, fontSize: 10)),
              Text('@${fixture.startAddress}', style: const TextStyle(color: Colors.white38, fontSize: 10)),
              const SizedBox(height: 4),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Container(
                    width: 8, height: 8, decoration: BoxDecoration(
                      shape: BoxShape.circle,
                      color: fixture.online ? Colors.green : Colors.red,
                    ),
                  ),
                  const SizedBox(width: 4),
                  Text(fixture.online ? 'Online' : 'Offline',
                       style: TextStyle(color: fixture.online ? Colors.green : Colors.red, fontSize: 10)),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }
}
