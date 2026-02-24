import 'package:flutter/material.dart';
import '../models/speaker.dart';
import '../services/api_client.dart';

class SpeakerDetailScreen extends StatefulWidget {
  final Speaker speaker;
  final ApiClient api;

  const SpeakerDetailScreen({
    super.key,
    required this.speaker,
    required this.api,
  });

  @override
  State<SpeakerDetailScreen> createState() => _SpeakerDetailScreenState();
}

class _SpeakerDetailScreenState extends State<SpeakerDetailScreen> {
  late double _volume;
  late String _type;
  late String _role;
  late TextEditingController _nameCtrl;
  bool _saving = false;

  static const List<String> _types = ['2WAY', '3WAY', 'SUB'];
  static const List<String> _roles = ['SX', 'DX', 'SUB', 'MONO'];

  @override
  void initState() {
    super.initState();
    _volume  = widget.speaker.volume;
    _type    = widget.speaker.type;
    _role    = widget.speaker.role;
    _nameCtrl = TextEditingController(text: widget.speaker.name);
  }

  @override
  void dispose() {
    _nameCtrl.dispose();
    super.dispose();
  }

  Future<void> _save() async {
    setState(() { _saving = true; });
    try {
      await widget.api.setSpeakerVolume(widget.speaker.id, _volume.round());
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Impostazioni salvate'),
            backgroundColor: Color(0xFF0f3460),
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Errore: $e'),
            backgroundColor: Colors.red.shade900,
          ),
        );
      }
    } finally {
      setState(() { _saving = false; });
    }
  }

  @override
  Widget build(BuildContext context) {
    final sp = widget.speaker;

    return Scaffold(
      appBar: AppBar(
        title: Text(sp.name),
        backgroundColor: const Color(0xFF1a1a2e),
      ),
      backgroundColor: const Color(0xFF1a1a2e),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            // Info base
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Row(
                  children: [
                    const Icon(Icons.speaker, color: Color(0xFFe94560), size: 40),
                    const SizedBox(width: 12),
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text('ID: ${sp.id}',
                            style: const TextStyle(color: Colors.white70)),
                        Text('Posizione: ${sp.position}',
                            style: const TextStyle(color: Colors.white70)),
                        Text('Online: ${sp.online ? "✅" : "❌"}',
                            style: const TextStyle(color: Colors.white70)),
                      ],
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 12),

            // Volume
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Volume: ${_volume.round()}%',
                        style: const TextStyle(color: Colors.white70)),
                    Slider(
                      value: _volume,
                      min: 0,
                      max: 100,
                      activeColor: const Color(0xFFe94560),
                      onChanged: (v) => setState(() => _volume = v),
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 12),

            // Tipo cassa
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Row(
                  children: [
                    const Text('Tipo:', style: TextStyle(color: Colors.white70)),
                    const SizedBox(width: 12),
                    DropdownButton<String>(
                      value: _type,
                      dropdownColor: const Color(0xFF0f3460),
                      style: const TextStyle(color: Colors.white),
                      items: _types
                          .map((t) => DropdownMenuItem(value: t, child: Text(t)))
                          .toList(),
                      onChanged: (v) => setState(() => _type = v!),
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 12),

            // Ruolo
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Row(
                  children: [
                    const Text('Ruolo:', style: TextStyle(color: Colors.white70)),
                    const SizedBox(width: 12),
                    DropdownButton<String>(
                      value: _role,
                      dropdownColor: const Color(0xFF0f3460),
                      style: const TextStyle(color: Colors.white),
                      items: _roles
                          .map((r) => DropdownMenuItem(value: r, child: Text(r)))
                          .toList(),
                      onChanged: (v) => setState(() => _role = v!),
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 12),

            // Nome personalizzato
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: TextField(
                  controller: _nameCtrl,
                  style: const TextStyle(color: Colors.white),
                  decoration: const InputDecoration(
                    labelText: 'Nome personalizzato',
                    labelStyle: TextStyle(color: Colors.white54),
                    enabledBorder: UnderlineInputBorder(
                      borderSide: BorderSide(color: Colors.white24),
                    ),
                    focusedBorder: UnderlineInputBorder(
                      borderSide: BorderSide(color: Color(0xFFe94560)),
                    ),
                  ),
                ),
              ),
            ),
            const SizedBox(height: 20),

            // Pulsante salva
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(
                backgroundColor: const Color(0xFFe94560),
                padding: const EdgeInsets.symmetric(vertical: 16),
              ),
              icon: _saving
                  ? const SizedBox(
                      width: 20,
                      height: 20,
                      child: CircularProgressIndicator(color: Colors.white, strokeWidth: 2),
                    )
                  : const Icon(Icons.save),
              label: Text(_saving ? 'Salvataggio...' : 'SALVA'),
              onPressed: _saving ? null : _save,
            ),
          ],
        ),
      ),
    );
  }
}
