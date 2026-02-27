import 'dart:async';
import 'package:flutter/material.dart';
import '../services/api_client.dart';

enum AutotuneSource { localMic, smartphoneMic }

class AutotuneScreen extends StatefulWidget {
  final ApiClient api;
  const AutotuneScreen({super.key, required this.api});

  @override
  State<AutotuneScreen> createState() => _AutotuneScreenState();
}

class _AutotuneScreenState extends State<AutotuneScreen> {
  AutotuneSource _source = AutotuneSource.localMic;
  bool _running = false;
  double _progress = 0.0;
  String _statusMsg = 'Pronto per calibrazione';
  Map<String, dynamic>? _status;
  Timer? _pollTimer;

  @override
  void dispose() {
    _pollTimer?.cancel();
    super.dispose();
  }

  Future<void> _startAutotune() async {
    setState(() {
      _running = true;
      _progress = 0.0;
      _statusMsg = 'Avvio autotune...';
    });

    try {
      if (_source == AutotuneSource.localMic) {
        await widget.api.startAutotune('single');
      } else {
        await widget.api.startAutotuneRemote();
      }
      _pollTimer =
          Timer.periodic(const Duration(milliseconds: 500), _pollStatus);
    } catch (e) {
      setState(() {
        _running = false;
        _statusMsg = 'Errore avvio: $e';
      });
    }
  }

  Future<void> _pollStatus(Timer t) async {
    try {
      final st = await widget.api.getAutotuneStatus();
      setState(() {
        _status = st;
        _progress = (st['progress'] as num?)?.toDouble() ?? 0.0;
        _statusMsg = st['message'] as String? ?? '';
        final stateNum = st['state'] as int? ?? 0;
        // State 6 = Complete, 7 = Error
        if (stateNum >= 6) {
          _running = false;
          _pollTimer?.cancel();
        }
      });
    } catch (_) {
      // Ignora errori polling
    }
  }

  Future<void> _applyResults() async {
    try {
      await widget.api.applyAutotuneResults();
      setState(() { _statusMsg = 'Correzioni applicate al DSP!'; });
    } catch (e) {
      setState(() { _statusMsg = 'Errore applicazione: $e'; });
    }
  }

  Future<void> _cancel() async {
    _pollTimer?.cancel();
    try {
      await widget.api.cancelAutotune();
    } catch (_) {}
    setState(() {
      _running = false;
      _progress = 0.0;
      _statusMsg = 'Annullato';
    });
  }

  @override
  Widget build(BuildContext context) {
    final isComplete = (_status?['state'] as int? ?? 0) == 6;

    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213e),
        title:
            const Text('AutoTune', style: TextStyle(color: Colors.white)),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Selezione sorgente
            const Text('Sorgente microfono',
                style: TextStyle(
                    color: Colors.white,
                    fontSize: 16,
                    fontWeight: FontWeight.bold)),
            const SizedBox(height: 12),
            _SourceCard(
              title: 'Microfono Locale (ESP32)',
              subtitle:
                  'Usa l\'ingresso ADC ES8388 sul modulo',
              icon: Icons.mic,
              selected: _source == AutotuneSource.localMic,
              onTap: () =>
                  setState(() => _source = AutotuneSource.localMic),
            ),
            const SizedBox(height: 8),
            _SourceCard(
              title: 'Microfono Smartphone',
              subtitle:
                  'Cattura audio col microfono del telefono via WiFi',
              icon: Icons.smartphone,
              selected: _source == AutotuneSource.smartphoneMic,
              onTap: () =>
                  setState(() => _source = AutotuneSource.smartphoneMic),
            ),
            const SizedBox(height: 24),

            // Istruzioni
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: const Color(0xFF16213e),
                borderRadius: BorderRadius.circular(10),
                border: Border.all(color: Colors.white12),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Text('Come funziona:',
                      style: TextStyle(
                          color: Colors.white,
                          fontWeight: FontWeight.bold)),
                  const SizedBox(height: 8),
                  _step('1', 'Il sistema genera un sweep 20Hz-20kHz'),
                  _step(
                      '2',
                      _source == AutotuneSource.localMic
                          ? 'Il microfono ES8388 cattura la risposta in frequenza'
                          : 'Il microfono dello smartphone cattura la risposta'),
                  _step('3', 'Analisi FFT e calcolo correzione EQ'),
                  _step(
                      '4', 'Applicazione EQ e delay al DSP CQ260D'),
                ],
              ),
            ),
            const SizedBox(height: 24),

            // Barra progresso
            LinearProgressIndicator(
              value: _running
                  ? _progress
                  : (_progress > 0 ? _progress : null),
              backgroundColor: const Color(0xFF0f3460),
              valueColor: const AlwaysStoppedAnimation<Color>(
                  Color(0xFFe94560)),
              minHeight: 8,
            ),
            const SizedBox(height: 8),
            Text(_statusMsg,
                style: const TextStyle(
                    color: Colors.white70, fontSize: 13)),
            const SizedBox(height: 24),

            // Bottoni
            Row(
              children: [
                if (!_running)
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFFe94560),
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.tune),
                      label: const Text('AVVIA AUTOTUNE',
                          style: TextStyle(fontSize: 16)),
                      onPressed: _startAutotune,
                    ),
                  ),
                if (_running) ...[
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFF555555),
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.stop),
                      label: const Text('ANNULLA'),
                      onPressed: _cancel,
                    ),
                  ),
                ],
                if (isComplete) ...[
                  const SizedBox(width: 12),
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFF00e676),
                        foregroundColor: Colors.black,
                        padding:
                            const EdgeInsets.symmetric(vertical: 16),
                      ),
                      icon: const Icon(Icons.check),
                      label: const Text('APPLICA CORREZIONE',
                          style: TextStyle(fontSize: 14)),
                      onPressed: _applyResults,
                    ),
                  ),
                ],
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _step(String num, String text) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 3),
      child: Row(
        children: [
          CircleAvatar(
            radius: 10,
            backgroundColor: const Color(0xFFe94560),
            child: Text(num,
                style: const TextStyle(
                    color: Colors.white, fontSize: 10)),
          ),
          const SizedBox(width: 8),
          Expanded(
              child: Text(text,
                  style: const TextStyle(
                      color: Colors.white70, fontSize: 13))),
        ],
      ),
    );
  }
}

class _SourceCard extends StatelessWidget {
  final String title;
  final String subtitle;
  final IconData icon;
  final bool selected;
  final VoidCallback onTap;

  const _SourceCard({
    required this.title,
    required this.subtitle,
    required this.icon,
    required this.selected,
    required this.onTap,
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(16),
        decoration: BoxDecoration(
          color: selected
              ? const Color(0xFF1a0f1e)
              : const Color(0xFF16213e),
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color:
                selected ? const Color(0xFFe94560) : Colors.white12,
            width: selected ? 2 : 1,
          ),
        ),
        child: Row(
          children: [
            Icon(icon,
                color: selected
                    ? const Color(0xFFe94560)
                    : Colors.white54,
                size: 32),
            const SizedBox(width: 16),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    title,
                    style: TextStyle(
                      color: selected ? Colors.white : Colors.white70,
                      fontWeight: selected
                          ? FontWeight.bold
                          : FontWeight.normal,
                    ),
                  ),
                  Text(subtitle,
                      style: const TextStyle(
                          color: Colors.white54, fontSize: 12)),
                ],
              ),
            ),
            if (selected)
              const Icon(Icons.check_circle,
                  color: Color(0xFFe94560)),
          ],
        ),
      ),
    );
  }
}
