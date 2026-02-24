import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'services/api_client.dart';
import 'screens/home_screen.dart';

void main() {
  runApp(const DspControlApp());
}

class DspControlApp extends StatelessWidget {
  const DspControlApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'DSP Control',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorScheme: ColorScheme.dark(
          primary: const Color(0xFFe94560),
          surface: const Color(0xFF16213e),
        ),
        scaffoldBackgroundColor: const Color(0xFF1a1a2e),
        useMaterial3: true,
      ),
      home: const _IpSetupScreen(),
    );
  }
}

// Schermata iniziale per configurare l'IP dell'ESP32
class _IpSetupScreen extends StatefulWidget {
  const _IpSetupScreen();

  @override
  State<_IpSetupScreen> createState() => _IpSetupScreenState();
}

class _IpSetupScreenState extends State<_IpSetupScreen> {
  final _ipCtrl = TextEditingController(text: 'http://192.168.4.1');
  bool _loading = true;

  @override
  void initState() {
    super.initState();
    _loadSavedIp();
  }

  @override
  void dispose() {
    _ipCtrl.dispose();
    super.dispose();
  }

  Future<void> _loadSavedIp() async {
    final prefs = await SharedPreferences.getInstance();
    final saved = prefs.getString('esp32_ip');
    if (saved != null) _ipCtrl.text = saved;
    setState(() { _loading = false; });
  }

  Future<void> _connect() async {
    final ip = _ipCtrl.text.trim();
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('esp32_ip', ip);

    if (!mounted) return;
    Navigator.of(context).pushReplacement(
      MaterialPageRoute(
        builder: (_) => HomeScreen(api: ApiClient(baseUrl: ip)),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    if (_loading) {
      return const Scaffold(
        body: Center(child: CircularProgressIndicator(color: Color(0xFFe94560))),
      );
    }

    return Scaffold(
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(32),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              const Icon(Icons.speaker_group, size: 80, color: Color(0xFFe94560)),
              const SizedBox(height: 24),
              const Text(
                'DSP Control',
                style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold, color: Colors.white),
              ),
              const SizedBox(height: 8),
              const Text(
                'Inserisci l\'indirizzo IP dell\'ESP32\n(default: http://192.168.4.1)',
                textAlign: TextAlign.center,
                style: TextStyle(color: Colors.white54),
              ),
              const SizedBox(height: 24),
              TextField(
                controller: _ipCtrl,
                style: const TextStyle(color: Colors.white),
                decoration: const InputDecoration(
                  labelText: 'Indirizzo ESP32',
                  labelStyle: TextStyle(color: Colors.white54),
                  prefixIcon: Icon(Icons.wifi, color: Color(0xFFe94560)),
                  enabledBorder: OutlineInputBorder(
                    borderSide: BorderSide(color: Colors.white24),
                  ),
                  focusedBorder: OutlineInputBorder(
                    borderSide: BorderSide(color: Color(0xFFe94560)),
                  ),
                ),
                keyboardType: TextInputType.url,
                onSubmitted: (_) => _connect(),
              ),
              const SizedBox(height: 20),
              SizedBox(
                width: double.infinity,
                child: ElevatedButton.icon(
                  style: ElevatedButton.styleFrom(
                    backgroundColor: const Color(0xFFe94560),
                    padding: const EdgeInsets.symmetric(vertical: 16),
                  ),
                  icon: const Icon(Icons.login),
                  label: const Text('CONNETTI', style: TextStyle(fontSize: 16)),
                  onPressed: _connect,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
