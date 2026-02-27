import 'package:flutter/material.dart';
import '../services/api_client.dart';
import '../models/dsp_preset.dart';

// ======= Schermata DSP Avanzato =======

class DspAdvancedScreen extends StatefulWidget {
  final ApiClient api;
  final int speakerId;
  final String speakerName;
  const DspAdvancedScreen({
    super.key,
    required this.api,
    required this.speakerId,
    required this.speakerName,
  });

  @override
  State<DspAdvancedScreen> createState() => _DspAdvancedScreenState();
}

class _DspAdvancedScreenState extends State<DspAdvancedScreen> {
  double _gain    = 0;
  double _hpf     = 80;
  double _lpf     = 20000;
  double _delay   = 0;
  bool   _muted   = false;
  bool   _phase   = false;
  bool   _loading = false;

  // Compressore
  double _compThreshold = 128;
  double _compRatio     = 4;
  double _compAttack    = 10;
  double _compRelease   = 50;
  double _compMakeup    = 0;

  // Preset
  List<DspPreset> _presets = [];

  @override
  void initState() {
    super.initState();
    _loadPresets();
  }

  Future<void> _loadPresets() async {
    try {
      final p = await widget.api.getDspPresets();
      setState(() { _presets = p; });
    } catch (_) {}
  }

  Future<void> _applyParam(Future<void> Function() action) async {
    try { await action(); } catch (e) {
      if (mounted) ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Errore: $e'), backgroundColor: Colors.red),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        title: Text('DSP – ${widget.speakerName}'),
        backgroundColor: const Color(0xFF1a1a2e),
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          // ——— Preset ———
          _buildSection('📦 Preset', _buildPresetsSection()),
          // ——— Gain / Mute / Fase ———
          _buildSection('🔊 Gain & Controllo', _buildGainSection()),
          // ——— HPF / LPF ———
          _buildSection('🎚 Filtri HPF/LPF', _buildFiltersSection()),
          // ——— Delay ———
          _buildSection('⏱ Delay', _buildDelaySection()),
          // ——— Compressore ———
          _buildSection('🗜 Compressore', _buildCompressorSection()),
          // ——— AutoTune ———
          _buildSection('🎯 AutoTune', _buildAutotuneSection()),
        ],
      ),
    );
  }

  Widget _buildSection(String title, Widget content) => Card(
        color: const Color(0xFF16213e),
        margin: const EdgeInsets.only(bottom: 12),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(title, style: const TextStyle(color: Color(0xFFe94560), fontWeight: FontWeight.bold, fontSize: 15)),
              const SizedBox(height: 10),
              content,
            ],
          ),
        ),
      );

  Widget _buildPresetsSection() => Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Wrap(
            spacing: 8, runSpacing: 8,
            children: _presets.map((p) => ActionChip(
              backgroundColor: const Color(0xFF0f3460),
              label: Text(p.name, style: const TextStyle(color: Colors.white, fontSize: 12)),
              onPressed: () => _applyParam(() => widget.api.applyDspPreset(p.name, deviceId: widget.speakerId)),
            )).toList(),
          ),
        ],
      );

  Widget _buildGainSection() => Column(
        children: [
          _labeledSlider('Gain: ${_gain.toStringAsFixed(1)} dB', _gain, -60, 12,
              (v) => setState(() => _gain = v),
              (v) => _applyParam(() => widget.api.setSpeakerGain(widget.speakerId, v))),
          Row(
            children: [
              Expanded(child: Row(children: [
                const Text('Mute:', style: TextStyle(color: Colors.white70)),
                Switch(
                  value: _muted,
                  activeColor: const Color(0xFFe94560),
                  onChanged: (v) { setState(() => _muted = v); _applyParam(() => widget.api.setSpeakerMute(widget.speakerId, v)); },
                ),
              ])),
              Expanded(child: Row(children: [
                const Text('Fase inv.:', style: TextStyle(color: Colors.white70)),
                Switch(
                  value: _phase,
                  activeColor: const Color(0xFFe94560),
                  onChanged: (v) { setState(() => _phase = v); _applyParam(() => widget.api.setSpeakerPhase(widget.speakerId, v)); },
                ),
              ])),
            ],
          ),
        ],
      );

  Widget _buildFiltersSection() => Column(
        children: [
          _labeledSlider('HPF: ${_hpf.round()} Hz', _hpf, 20, 2000,
              (v) => setState(() => _hpf = v),
              (v) => _applyParam(() => widget.api.setSpeakerHpf(widget.speakerId, v))),
          _labeledSlider('LPF: ${_lpf.round()} Hz', _lpf, 200, 20000,
              (v) => setState(() => _lpf = v),
              (v) => _applyParam(() => widget.api.setSpeakerLpf(widget.speakerId, v))),
        ],
      );

  Widget _buildDelaySection() => _labeledSlider(
        'Delay: ${_delay.toStringAsFixed(1)} ms', _delay, 0, 100,
        (v) => setState(() => _delay = v),
        (v) => _applyParam(() => widget.api.setSpeakerDelay(widget.speakerId, v)),
      );

  Widget _buildCompressorSection() => Column(
        children: [
          _labeledSlider('Soglia: ${_compThreshold.round()}', _compThreshold, 0, 255,
              (v) => setState(() => _compThreshold = v), (_) => _applyCompressor()),
          _labeledSlider('Ratio: ${_compRatio.round()}:1', _compRatio, 1, 20,
              (v) => setState(() => _compRatio = v), (_) => _applyCompressor()),
          _labeledSlider('Attack: ${_compAttack.round()} ms', _compAttack, 0, 200,
              (v) => setState(() => _compAttack = v), (_) => _applyCompressor()),
          _labeledSlider('Release: ${_compRelease.round()} ms', _compRelease, 0, 500,
              (v) => setState(() => _compRelease = v), (_) => _applyCompressor()),
          _labeledSlider('Makeup: ${_compMakeup.round()} dB', _compMakeup, 0, 48,
              (v) => setState(() => _compMakeup = v), (_) => _applyCompressor()),
        ],
      );

  void _applyCompressor() => _applyParam(() => widget.api.setCompressor(
        widget.speakerId,
        threshold: _compThreshold.round(),
        ratio:     _compRatio.round(),
        attack:    _compAttack.round(),
        release:   _compRelease.round(),
        makeup:    _compMakeup.round(),
      ));

  Widget _buildAutotuneSection() => Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text('Calibrazione automatica risposta in frequenza',
                     style: TextStyle(color: Colors.white54, fontSize: 12)),
          const SizedBox(height: 8),
          Row(
            children: [
              ElevatedButton.icon(
                style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
                icon: const Icon(Icons.tune),
                label: const Text('Calibra questa cassa'),
                onPressed: () => _startAutotune(widget.speakerId),
              ),
            ],
          ),
        ],
      );

  Future<void> _startAutotune(int speakerId) async {
    await widget.api.startAutotune('single', targetId: speakerId);
    if (!mounted) return;
    showDialog(
      context: context,
      barrierDismissible: false,
      builder: (ctx) => _AutotuneProgressDialog(api: widget.api, speakerId: speakerId),
    );
  }

  Widget _labeledSlider(String label, double value, double min, double max,
      ValueChanged<double> onChanged, ValueChanged<double> onChangeEnd) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(label, style: const TextStyle(color: Colors.white70, fontSize: 12)),
        Slider(
          value: value.clamp(min, max),
          min: min, max: max,
          activeColor: const Color(0xFFe94560),
          inactiveColor: const Color(0xFF0f3460),
          onChanged: onChanged,
          onChangeEnd: onChangeEnd,
        ),
      ],
    );
  }
}

// ——— Dialog avanzamento AutoTune ———
class _AutotuneProgressDialog extends StatefulWidget {
  final ApiClient api;
  final int speakerId;
  const _AutotuneProgressDialog({required this.api, required this.speakerId});

  @override
  State<_AutotuneProgressDialog> createState() => _AutotuneProgressDialogState();
}

class _AutotuneProgressDialogState extends State<_AutotuneProgressDialog> {
  String  _message  = 'Avvio calibrazione...';
  double  _progress = 0;
  bool    _done     = false;
  String  _state    = '';

  @override
  void initState() {
    super.initState();
    _poll();
  }

  void _poll() async {
    while (mounted && !_done) {
      try {
        final data = await widget.api.getAutotuneStatus();
        if (!mounted) break;
        setState(() {
          _message  = data['message'] as String? ?? '';
          _progress = (data['progress'] as num?)?.toDouble() ?? 0;
          _state    = data['state'] as String? ?? '';
          _done     = _state == 'Complete' || _state == 'Error';
        });
      } catch (_) {}
      await Future.delayed(const Duration(milliseconds: 500));
    }
  }

  @override
  Widget build(BuildContext context) => AlertDialog(
        backgroundColor: const Color(0xFF16213e),
        title: const Text('AutoTune in corso', style: TextStyle(color: Colors.white)),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            LinearProgressIndicator(
              value: _progress,
              backgroundColor: const Color(0xFF0f3460),
              valueColor: const AlwaysStoppedAnimation<Color>(Color(0xFFe94560)),
            ),
            const SizedBox(height: 12),
            Text(_message, textAlign: TextAlign.center, style: const TextStyle(color: Colors.white70)),
          ],
        ),
        actions: _done ? [
          TextButton(onPressed: () => Navigator.pop(context), child: const Text('Chiudi')),
          if (_state == 'Complete')
            ElevatedButton(
              style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
              onPressed: () async {
                await widget.api.applyAutotuneResults();
                if (context.mounted) Navigator.pop(context);
              },
              child: const Text('Applica correzioni'),
            ),
        ] : [
          TextButton(
            onPressed: () async {
              await widget.api.cancelAutotune();
              if (context.mounted) Navigator.pop(context);
            },
            child: const Text('Annulla', style: TextStyle(color: Colors.red)),
          ),
        ],
      );
}
