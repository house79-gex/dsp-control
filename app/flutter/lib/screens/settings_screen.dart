import 'package:flutter/material.dart';
import '../services/api_client.dart';

// ======= Schermata Impostazioni =======

class SettingsScreen extends StatefulWidget {
  final ApiClient api;
  final void Function(String newUrl)? onUrlChanged;
  const SettingsScreen({super.key, required this.api, this.onUrlChanged});

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  Map<String, dynamic> _sysInfo     = {};
  bool _expertMode                  = false;
  bool _usbConnected                = false;
  bool _loading                     = false;
  final _ipCtrl  = TextEditingController();
  final _passCtrl = TextEditingController();

  @override
  void initState() {
    super.initState();
    _ipCtrl.text = widget.api.baseUrl;
    _loadAll();
  }

  @override
  void dispose() {
    _ipCtrl.dispose();
    _passCtrl.dispose();
    super.dispose();
  }

  Future<void> _loadAll() async {
    setState(() { _loading = true; });
    try {
      final info = await widget.api.getSystemInfo();
      final usb  = await widget.api.getUsbStatus();
      final dsp  = await widget.api.getDspStatus();
      setState(() {
        _sysInfo      = info;
        _usbConnected = usb['connected'] as bool? ?? false;
        _expertMode   = dsp['expert_mode'] as bool? ?? false;
      });
    } catch (_) {}
    finally { setState(() { _loading = false; }); }
  }

  Future<void> _setExpertMode(bool expert) async {
    try {
      await widget.api.setSystemMode(expert ? 'expert' : 'base',
          password: expert && _passCtrl.text.isNotEmpty ? _passCtrl.text : null);
      setState(() { _expertMode = expert; });
      if (mounted) ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(expert ? 'Modalità Esperto attivata' : 'Modalità Base attivata')),
      );
    } catch (e) {
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
        title: const Text('⚙️ Impostazioni'),
        backgroundColor: const Color(0xFF1a1a2e),
        actions: [
          IconButton(icon: const Icon(Icons.refresh), onPressed: _loadAll),
        ],
      ),
      body: _loading
          ? const Center(child: CircularProgressIndicator(color: Color(0xFFe94560)))
          : ListView(
              padding: const EdgeInsets.all(16),
              children: [
                _buildInfoCard(),
                const SizedBox(height: 12),
                _buildConnectionCard(),
                const SizedBox(height: 12),
                _buildModeCard(),
                const SizedBox(height: 12),
                _buildUsbCard(),
              ],
            ),
    );
  }

  Widget _buildInfoCard() => Card(
        color: const Color(0xFF16213e),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text('ℹ️ Info Sistema', style: TextStyle(color: Color(0xFFe94560), fontWeight: FontWeight.bold)),
              const SizedBox(height: 10),
              _infoRow('Firmware', _sysInfo['firmware_version']?.toString() ?? '-'),
              _infoRow('Uptime',   '${((_sysInfo['uptime_ms'] as num?)?.toInt() ?? 0) ~/ 1000}s'),
              _infoRow('Heap libero', '${_sysInfo['free_heap'] ?? '-'} byte'),
              _infoRow('Chip', _sysInfo['chip_model']?.toString() ?? '-'),
              _infoRow('DSP', (_sysInfo['dsp_connected'] as bool? ?? false) ? '✅ Connesso' : '❌ Disconnesso'),
            ],
          ),
        ),
      );

  Widget _buildConnectionCard() => Card(
        color: const Color(0xFF16213e),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text('📡 Connessione WiFi', style: TextStyle(color: Color(0xFFe94560), fontWeight: FontWeight.bold)),
              const SizedBox(height: 10),
              TextField(
                controller: _ipCtrl,
                style: const TextStyle(color: Colors.white),
                decoration: const InputDecoration(
                  labelText: 'IP ESP32',
                  labelStyle: TextStyle(color: Colors.white54),
                  prefixIcon: Icon(Icons.wifi, color: Color(0xFFe94560)),
                  enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Colors.white24)),
                  focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color(0xFFe94560))),
                ),
              ),
              const SizedBox(height: 8),
              SizedBox(
                width: double.infinity,
                child: ElevatedButton(
                  style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
                  onPressed: () {
                    widget.api.baseUrl = _ipCtrl.text.trim();
                    widget.onUrlChanged?.call(_ipCtrl.text.trim());
                    _loadAll();
                  },
                  child: const Text('Connetti'),
                ),
              ),
            ],
          ),
        ),
      );

  Widget _buildModeCard() => Card(
        color: const Color(0xFF16213e),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text('🔐 Modalità Utente', style: TextStyle(color: Color(0xFFe94560), fontWeight: FontWeight.bold)),
              const SizedBox(height: 10),
              Row(
                children: [
                  const Icon(Icons.lock, color: Colors.white54, size: 20),
                  const SizedBox(width: 8),
                  const Text('Base', style: TextStyle(color: Colors.white70)),
                  const SizedBox(width: 12),
                  Switch(
                    value: _expertMode,
                    activeColor: const Color(0xFFe94560),
                    onChanged: (v) {
                      if (v) {
                        _showPasswordDialog();
                      } else {
                        _setExpertMode(false);
                      }
                    },
                  ),
                  const SizedBox(width: 12),
                  const Text('Esperto', style: TextStyle(color: Colors.white70)),
                  const Icon(Icons.lock_open, color: Colors.white54, size: 20),
                ],
              ),
              if (_expertMode)
                const Padding(
                  padding: EdgeInsets.only(top: 4),
                  child: Text('⚠️ Modalità Esperto attiva – accesso a tutti i parametri DSP',
                              style: TextStyle(color: Colors.orange, fontSize: 11)),
                ),
            ],
          ),
        ),
      );

  Widget _buildUsbCard() => Card(
        color: const Color(0xFF16213e),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text('💾 USB-C Import/Export', style: TextStyle(color: Color(0xFFe94560), fontWeight: FontWeight.bold)),
              const SizedBox(height: 10),
              Row(
                children: [
                  Container(
                    width: 10, height: 10, decoration: BoxDecoration(
                      shape: BoxShape.circle,
                      color: _usbConnected ? Colors.green : Colors.grey,
                    ),
                  ),
                  const SizedBox(width: 8),
                  Text(_usbConnected ? 'USB connessa' : 'USB non connessa',
                       style: const TextStyle(color: Colors.white70)),
                ],
              ),
              const SizedBox(height: 12),
              Wrap(spacing: 8, runSpacing: 8, children: [
                _actionButton('⬇️ Esporta Config', () => widget.api.exportConfig()),
                _actionButton('⬆️ Importa Config', () => widget.api.importConfig()),
                _actionButton('⬇️ Esporta Preset', () => widget.api.exportPresets()),
                _actionButton('⬇️ Esporta Scene',  () => widget.api.exportScenes()),
              ]),
            ],
          ),
        ),
      );

  Widget _actionButton(String label, Future<void> Function() action) => ElevatedButton(
        style: ElevatedButton.styleFrom(
          backgroundColor: const Color(0xFF0f3460),
          padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        ),
        onPressed: () async {
          try {
            await action();
            if (mounted) ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('$label completato')),
            );
          } catch (e) {
            if (mounted) ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('Errore: $e'), backgroundColor: Colors.red),
            );
          }
        },
        child: Text(label, style: const TextStyle(fontSize: 12)),
      );

  Widget _infoRow(String label, String value) => Padding(
        padding: const EdgeInsets.symmetric(vertical: 3),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(label, style: const TextStyle(color: Colors.white54, fontSize: 13)),
            Text(value,  style: const TextStyle(color: Colors.white,   fontSize: 13)),
          ],
        ),
      );

  void _showPasswordDialog() {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        backgroundColor: const Color(0xFF16213e),
        title: const Text('Password Modalità Esperto', style: TextStyle(color: Colors.white)),
        content: TextField(
          controller: _passCtrl,
          style: const TextStyle(color: Colors.white),
          obscureText: true,
          decoration: const InputDecoration(
            labelText: 'Password (lascia vuoto se non configurata)',
            labelStyle: TextStyle(color: Colors.white54),
            enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Colors.white24)),
            focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color(0xFFe94560))),
          ),
        ),
        actions: [
          TextButton(onPressed: () => Navigator.pop(ctx), child: const Text('Annulla', style: TextStyle(color: Colors.white54))),
          ElevatedButton(
            style: ElevatedButton.styleFrom(backgroundColor: const Color(0xFFe94560)),
            onPressed: () { Navigator.pop(ctx); _setExpertMode(true); },
            child: const Text('Sblocca'),
          ),
        ],
      ),
    );
  }
}
