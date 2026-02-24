import 'dart:convert';
import 'package:http/http.dart' as http;
import '../models/speaker.dart';

class ApiClient {
  String baseUrl;

  ApiClient({this.baseUrl = 'http://192.168.4.1'});

  // GET /api/devices → lista casse assegnate
  Future<List<Speaker>> getDevices() async {
    try {
      final res = await http
          .get(Uri.parse('$baseUrl/api/devices'))
          .timeout(const Duration(seconds: 5));
      if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}');
      final List<dynamic> data = jsonDecode(res.body) as List<dynamic>;
      return data
          .map((e) => Speaker.fromJson(e as Map<String, dynamic>))
          .toList();
    } catch (e) {
      throw Exception('getDevices fallito: $e');
    }
  }

  // POST /api/audio-mode  body: {"mode": "mixer" | "test"}
  Future<void> setAudioMode(String mode) async {
    try {
      await http
          .post(
            Uri.parse('$baseUrl/api/audio-mode'),
            headers: {'Content-Type': 'application/json'},
            body: jsonEncode({'mode': mode}),
          )
          .timeout(const Duration(seconds: 5));
    } catch (e) {
      throw Exception('setAudioMode fallito: $e');
    }
  }

  // POST /api/master-volume  body: {"value": 0–100}
  Future<void> setMasterVolume(int value) async {
    try {
      await http
          .post(
            Uri.parse('$baseUrl/api/master-volume'),
            headers: {'Content-Type': 'application/json'},
            body: jsonEncode({'value': value}),
          )
          .timeout(const Duration(seconds: 5));
    } catch (e) {
      throw Exception('setMasterVolume fallito: $e');
    }
  }

  // POST /api/speaker-volume  body: {"id": N, "value": 0–100}
  Future<void> setSpeakerVolume(int id, int value) async {
    try {
      await http
          .post(
            Uri.parse('$baseUrl/api/speaker-volume'),
            headers: {'Content-Type': 'application/json'},
            body: jsonEncode({'id': id, 'value': value}),
          )
          .timeout(const Duration(seconds: 5));
    } catch (e) {
      throw Exception('setSpeakerVolume fallito: $e');
    }
  }

  // POST /api/discovery → {"devices": [id, ...]}
  Future<List<int>> triggerDiscovery() async {
    try {
      final res = await http
          .post(Uri.parse('$baseUrl/api/discovery'))
          .timeout(const Duration(seconds: 30));
      if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}');
      final Map<String, dynamic> data =
          jsonDecode(res.body) as Map<String, dynamic>;
      final List<dynamic> ids = data['devices'] as List<dynamic>? ?? [];
      return ids.map((e) => e as int).toList();
    } catch (e) {
      throw Exception('triggerDiscovery fallito: $e');
    }
  }
}
