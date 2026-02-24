import 'package:flutter/material.dart';
import '../services/api_client.dart';

class DiscoveryScreen extends StatefulWidget {
  final ApiClient api;
  const DiscoveryScreen({super.key, required this.api});

  @override
  State<DiscoveryScreen> createState() => _DiscoveryScreenState();
}

class _DiscoveryScreenState extends State<DiscoveryScreen> {
  List<int> _foundIds = [];
  bool _scanning = false;
  String? _error;

  Future<void> _startScan() async {
    setState(() { _scanning = true; _error = null; _foundIds = []; });
    try {
      final ids = await widget.api.triggerDiscovery();
      setState(() { _foundIds = ids; });
    } catch (e) {
      setState(() { _error = e.toString(); });
    } finally {
      setState(() { _scanning = false; });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Discovery RS-485'),
        backgroundColor: const Color(0xFF1a1a2e),
      ),
      backgroundColor: const Color(0xFF1a1a2e),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(
                backgroundColor: const Color(0xFFe94560),
                padding: const EdgeInsets.symmetric(vertical: 16),
              ),
              icon: _scanning
                  ? const SizedBox(
                      width: 20,
                      height: 20,
                      child: CircularProgressIndicator(color: Colors.white, strokeWidth: 2),
                    )
                  : const Icon(Icons.search),
              label: Text(_scanning ? 'Scansione in corso...' : 'SCANSIONE RS-485'),
              onPressed: _scanning ? null : _startScan,
            ),
            const SizedBox(height: 16),

            if (_error != null)
              Card(
                color: Colors.red.shade900,
                child: Padding(
                  padding: const EdgeInsets.all(12),
                  child: Text(_error!, style: const TextStyle(color: Colors.white)),
                ),
              ),

            if (_foundIds.isNotEmpty) ...[
              Text(
                'Trovati: ${_foundIds.length} dispositivi',
                style: const TextStyle(color: Colors.white70),
              ),
              const SizedBox(height: 8),
              Expanded(
                child: ListView.builder(
                  itemCount: _foundIds.length,
                  itemBuilder: (ctx, i) {
                    final id = _foundIds[i];
                    return Card(
                      color: const Color(0xFF16213e),
                      child: ListTile(
                        leading: const Icon(Icons.speaker, color: Color(0xFFe94560)),
                        title: Text(
                          'Dispositivo ID: $id',
                          style: const TextStyle(color: Colors.white),
                        ),
                        subtitle: const Text(
                          'Tocca per assegnare',
                          style: TextStyle(color: Colors.white54),
                        ),
                        onTap: () {
                          // TODO: naviga a schermata assegnazione per questo ID
                          ScaffoldMessenger.of(context).showSnackBar(
                            SnackBar(
                              content: Text('Assegna dispositivo $id (TODO)'),
                              backgroundColor: const Color(0xFF0f3460),
                            ),
                          );
                        },
                      ),
                    );
                  },
                ),
              ),
            ] else if (!_scanning)
              const Expanded(
                child: Center(
                  child: Text(
                    'Premi SCANSIONE per cercare\ni dispositivi sulla catena RS-485.',
                    textAlign: TextAlign.center,
                    style: TextStyle(color: Colors.white38),
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }
}
