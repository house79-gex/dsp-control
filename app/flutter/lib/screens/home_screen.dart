import 'dart:async';
import 'package:flutter/material.dart';
import '../models/speaker.dart';
import '../services/api_client.dart';
import 'discovery_screen.dart';
import 'speaker_detail_screen.dart';

class HomeScreen extends StatefulWidget {
  final ApiClient api;
  const HomeScreen({super.key, required this.api});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  List<Speaker> _speakers = [];
  double _masterVolume = 80;
  bool _loading = false;
  String? _error;

  // VU meter
  double _vuLeft = 0.0;
  double _vuRight = 0.0;
  double _vuPeakDb = -60.0;
  Timer? _vuTimer;

  @override
  void initState() {
    super.initState();
    _loadSpeakers();
    _vuTimer = Timer.periodic(
        const Duration(milliseconds: 100), (_) => _pollVuMeter());
  }

  @override
  void dispose() {
    _vuTimer?.cancel();
    super.dispose();
  }

  Future<void> _pollVuMeter() async {
    try {
      final data = await widget.api.getAudioLevels();
      if (!mounted) return;
      setState(() {
        // Converti dB in percentuale lineare per la barra (min=-60, max=0)
        final leftDb = (data['left_db'] as num?)?.toDouble() ?? -60.0;
        final rightDb = (data['right_db'] as num?)?.toDouble() ?? -60.0;
        _vuPeakDb = (data['master_db'] as num?)?.toDouble() ?? -60.0;
        _vuLeft = ((leftDb + 60.0) / 60.0).clamp(0.0, 1.0);
        _vuRight = ((rightDb + 60.0) / 60.0).clamp(0.0, 1.0);
      });
    } catch (_) {
      // VU meter non disponibile, ignora
    }
  }

  Future<void> _loadSpeakers() async {
    setState(() { _loading = true; _error = null; });
    try {
      final list = await widget.api.getDevices();
      setState(() { _speakers = list; });
    } catch (e) {
      setState(() { _error = e.toString(); });
    } finally {
      setState(() { _loading = false; });
    }
  }

  Future<void> _onMasterVolumeChanged(double val) async {
    setState(() { _masterVolume = val; });
    try {
      await widget.api.setMasterVolume(val.round());
    } catch (_) {}
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('SISTEMA AUDIO PA'),
        backgroundColor: const Color(0xFF1a1a2e),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: _loadSpeakers,
            tooltip: 'Aggiorna',
          ),
        ],
      ),
      backgroundColor: const Color(0xFF1a1a2e),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // VU meter L/R
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        const Text('VU Meter',
                            style: TextStyle(
                                color: Colors.white70, fontSize: 13)),
                        const Spacer(),
                        Text(
                          '${_vuPeakDb.toStringAsFixed(1)} dBFS',
                          style: TextStyle(
                            color: _vuPeakDb > -3.0
                                ? Colors.red
                                : Colors.white54,
                            fontSize: 12,
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 6),
                    Row(
                      children: [
                        const Text('L',
                            style: TextStyle(
                                color: Colors.white54, fontSize: 12)),
                        const SizedBox(width: 6),
                        Expanded(
                          child: ClipRRect(
                            borderRadius: BorderRadius.circular(4),
                            child: LinearProgressIndicator(
                              value: _vuLeft,
                              minHeight: 12,
                              backgroundColor:
                                  const Color(0xFF0f3460),
                              valueColor: AlwaysStoppedAnimation<Color>(
                                _vuLeft > 0.9
                                    ? Colors.red
                                    : _vuLeft > 0.7
                                        ? Colors.orange
                                        : const Color(0xFF00e676),
                              ),
                            ),
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 4),
                    Row(
                      children: [
                        const Text('R',
                            style: TextStyle(
                                color: Colors.white54, fontSize: 12)),
                        const SizedBox(width: 6),
                        Expanded(
                          child: ClipRRect(
                            borderRadius: BorderRadius.circular(4),
                            child: LinearProgressIndicator(
                              value: _vuRight,
                              minHeight: 12,
                              backgroundColor:
                                  const Color(0xFF0f3460),
                              valueColor: AlwaysStoppedAnimation<Color>(
                                _vuRight > 0.9
                                    ? Colors.red
                                    : _vuRight > 0.7
                                        ? Colors.orange
                                        : const Color(0xFF00e676),
                              ),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),

            // Volume master
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Volume Master: ${_masterVolume.round()}%',
                      style: const TextStyle(color: Colors.white70),
                    ),
                    Slider(
                      value: _masterVolume,
                      min: 0,
                      max: 100,
                      activeColor: const Color(0xFFe94560),
                      onChanged: (v) =>
                          setState(() => _masterVolume = v),
                      onChangeEnd: _onMasterVolumeChanged,
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),

            // Errore
            if (_error != null)
              Card(
                color: Colors.red.shade900,
                child: Padding(
                  padding: const EdgeInsets.all(12),
                  child: Text(_error!,
                      style: const TextStyle(color: Colors.white)),
                ),
              ),

            const SizedBox(height: 8),
            const Text('Casse',
                style:
                    TextStyle(color: Colors.white70, fontSize: 16)),
            const SizedBox(height: 4),

            // Lista casse
            Expanded(
              child: _loading
                  ? const Center(
                      child: CircularProgressIndicator(
                          color: Color(0xFFe94560)))
                  : _speakers.isEmpty
                      ? const Center(
                          child: Text(
                            'Nessuna cassa trovata.\nAvvia la scansione RS-485.',
                            textAlign: TextAlign.center,
                            style: TextStyle(color: Colors.white38),
                          ),
                        )
                      : ListView.builder(
                          itemCount: _speakers.length,
                          itemBuilder: (ctx, i) {
                            final sp = _speakers[i];
                            return Card(
                              color: const Color(0xFF16213e),
                              child: ListTile(
                                leading: const Icon(Icons.speaker,
                                    color: Color(0xFFe94560)),
                                title: Text(sp.name,
                                    style: const TextStyle(
                                        color: Colors.white)),
                                subtitle: Text(
                                  '${sp.type} • ${sp.position}',
                                  style: const TextStyle(
                                      color: Colors.white54),
                                ),
                                trailing: const Icon(
                                    Icons.chevron_right,
                                    color: Colors.white38),
                                onTap: () async {
                                  await Navigator.push(
                                    context,
                                    MaterialPageRoute(
                                      builder: (_) =>
                                          SpeakerDetailScreen(
                                        speaker: sp,
                                        api: widget.api,
                                      ),
                                    ),
                                  );
                                  _loadSpeakers();
                                },
                              ),
                            );
                          },
                        ),
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton.extended(
        backgroundColor: const Color(0xFFe94560),
        icon: const Icon(Icons.search),
        label: const Text('Discovery'),
        onPressed: () async {
          await Navigator.push(
            context,
            MaterialPageRoute(
              builder: (_) => DiscoveryScreen(api: widget.api),
            ),
          );
          _loadSpeakers();
        },
      ),
    );
  }
}
