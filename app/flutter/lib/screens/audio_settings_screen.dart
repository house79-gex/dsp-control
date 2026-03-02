import 'package:flutter/material.dart';
import '../services/api_client.dart';

// ======= Schermata Impostazioni Audio =======
// Configura input gain ADC ES8388, ottimizzato per Denon DJ SC LIVE 4

class AudioSettingsScreen extends StatefulWidget {
  final ApiClient api;
  const AudioSettingsScreen({super.key, required this.api});

  @override
  State<AudioSettingsScreen> createState() => _AudioSettingsScreenState();
}

class _AudioSettingsScreenState extends State<AudioSettingsScreen> {
  double _inputGainDb = -14.0;
  int    _sampleRate  = 44100;
  int    _bitDepth    = 24;
  bool   _isLoading   = true;

  @override
  void initState() {
    super.initState();
    _loadAudioConfig();
  }

  Future<void> _loadAudioConfig() async {
    try {
      final response = await widget.api.getInputGain();
      if (response['success'] == true) {
        setState(() {
          _inputGainDb = (response['inputGainDb'] as num).toDouble();
          _sampleRate  = (response['sampleRate']  as num?)?.toInt() ?? 44100;
          _bitDepth    = (response['bitDepth']    as num?)?.toInt() ?? 24;
        });
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Errore caricamento config audio: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isLoading = false);
    }
  }

  Future<void> _setInputGain(double gainDb) async {
    try {
      await widget.api.setInputGain(gainDb);
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Input gain impostato a ${gainDb.toStringAsFixed(1)} dB')),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Errore: $e'), backgroundColor: Colors.red),
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    if (_isLoading) {
      return Scaffold(
        backgroundColor: const Color(0xFF1a1a2e),
        appBar: AppBar(
          title: const Text('🎛️ Audio Settings'),
          backgroundColor: const Color(0xFF1a1a2e),
        ),
        body: const Center(child: CircularProgressIndicator(color: Color(0xFFe94560))),
      );
    }

    return Scaffold(
      backgroundColor: const Color(0xFF1a1a2e),
      appBar: AppBar(
        title: const Text('🎛️ Audio Settings'),
        backgroundColor: const Color(0xFF1a1a2e),
        actions: [
          IconButton(icon: const Icon(Icons.refresh), onPressed: _loadAudioConfig),
        ],
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Info Card
            Card(
              color: const Color(0xFF16213e),
              child: Padding(
                padding: const EdgeInsets.all(16),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Configurazione Audio',
                        style: TextStyle(color: Color(0xFFe94560),
                            fontSize: 16, fontWeight: FontWeight.bold)),
                    const SizedBox(height: 12),
                    _buildInfoRow('Sample Rate', '$_sampleRate Hz'),
                    _buildInfoRow('Bit Depth',   '$_bitDepth bit'),
                    _buildInfoRow('Ottimizzato per', 'Denon DJ SC LIVE 4'),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 24),

            // Input Gain
            const Text('Input Gain (ADC)',
                style: TextStyle(color: Colors.white, fontSize: 16,
                    fontWeight: FontWeight.bold)),
            const SizedBox(height: 6),
            Text(
              'Regola se il segnale è troppo alto (distorsione) o troppo basso (rumore).',
              style: TextStyle(fontSize: 13, color: Colors.grey[500]),
            ),
            const SizedBox(height: 16),

            // Slider
            Row(
              children: [
                const Text('-96 dB', style: TextStyle(color: Colors.white54, fontSize: 11)),
                Expanded(
                  child: Slider(
                    value: _inputGainDb,
                    min: -96.0,
                    max: 24.0,
                    divisions: 240,
                    label: '${_inputGainDb.toStringAsFixed(1)} dB',
                    activeColor: const Color(0xFFe94560),
                    onChanged: (v) => setState(() => _inputGainDb = v),
                    onChangeEnd: _setInputGain,
                  ),
                ),
                const Text('+24 dB', style: TextStyle(color: Colors.white54, fontSize: 11)),
              ],
            ),

            Center(
              child: Text(
                '${_inputGainDb.toStringAsFixed(1)} dB',
                style: const TextStyle(
                    color: Colors.white, fontSize: 32, fontWeight: FontWeight.bold),
              ),
            ),

            const SizedBox(height: 24),

            // Preset rapidi
            const Text('Preset Rapidi',
                style: TextStyle(color: Colors.white, fontSize: 16,
                    fontWeight: FontWeight.bold)),
            const SizedBox(height: 12),
            Wrap(
              spacing: 8,
              runSpacing: 8,
              children: [
                _buildPresetButton('SC LIVE 4 (+18 dBu)', -14.0),
                _buildPresetButton('Line Level (+4 dBu)',   0.0),
                _buildPresetButton('Consumer (-10 dBu)',   14.0),
              ],
            ),

            const SizedBox(height: 24),

            // Avviso gain alto
            if (_inputGainDb > 10.0)
              Card(
                color: Colors.orange[900],
                child: Padding(
                  padding: const EdgeInsets.all(12),
                  child: Row(
                    children: [
                      const Icon(Icons.warning_amber, color: Colors.orange),
                      const SizedBox(width: 12),
                      Expanded(
                        child: Text(
                          'Gain elevato: può introdurre rumore. '
                          'Usa solo per segnali di ingresso molto bassi.',
                          style: TextStyle(color: Colors.orange[100], fontSize: 13),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }

  Widget _buildInfoRow(String label, String value) => Padding(
        padding: const EdgeInsets.symmetric(vertical: 3),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(label, style: const TextStyle(color: Colors.white54, fontSize: 13)),
            Text(value,  style: const TextStyle(color: Colors.white,   fontSize: 13,
                fontWeight: FontWeight.bold)),
          ],
        ),
      );

  Widget _buildPresetButton(String label, double gainDb) {
    final bool isActive = (_inputGainDb - gainDb).abs() < 0.5;
    return ElevatedButton(
      onPressed: () {
        setState(() => _inputGainDb = gainDb);
        _setInputGain(gainDb);
      },
      style: ElevatedButton.styleFrom(
        backgroundColor: isActive ? const Color(0xFFe94560) : const Color(0xFF0f3460),
        foregroundColor: Colors.white,
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
      ),
      child: Text(label, style: const TextStyle(fontSize: 12)),
    );
  }
}
