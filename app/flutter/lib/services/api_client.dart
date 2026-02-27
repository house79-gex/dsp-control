import 'dart:convert';
import 'package:http/http.dart' as http;
import '../models/speaker.dart';
import '../models/dmx_fixture.dart';
import '../models/dmx_scene.dart';
import '../models/dsp_preset.dart';
import '../models/audio_reactive_config.dart';
import '../models/mic_calibration.dart';

class ApiClient {
  String baseUrl;
  static const _timeout = Duration(seconds: 5);

  ApiClient({this.baseUrl = 'http://192.168.4.1'});

  // ——— Helper interni ———

  Future<Map<String, dynamic>> _get(String path) async {
    final res = await http.get(Uri.parse('$baseUrl$path')).timeout(_timeout);
    if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}: ${res.body}');
    return jsonDecode(res.body) as Map<String, dynamic>;
  }

  Future<List<dynamic>> _getList(String path) async {
    final res = await http.get(Uri.parse('$baseUrl$path')).timeout(_timeout);
    if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}: ${res.body}');
    return jsonDecode(res.body) as List<dynamic>;
  }

  Future<Map<String, dynamic>> _post(String path, [Map<String, dynamic>? body]) async {
    final res = await http.post(
      Uri.parse('$baseUrl$path'),
      headers: {'Content-Type': 'application/json'},
      body: body != null ? jsonEncode(body) : null,
    ).timeout(_timeout);
    if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}: ${res.body}');
    return jsonDecode(res.body) as Map<String, dynamic>;
  }

  Future<Map<String, dynamic>> _delete(String path, [Map<String, dynamic>? body]) async {
    final req = http.Request('DELETE', Uri.parse('$baseUrl$path'));
    req.headers['Content-Type'] = 'application/json';
    if (body != null) req.body = jsonEncode(body);
    final streamed = await req.send().timeout(_timeout);
    final res = await http.Response.fromStream(streamed);
    if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}: ${res.body}');
    return jsonDecode(res.body) as Map<String, dynamic>;
  }

  // ——— Endpoint originali ———

  Future<List<Speaker>> getDevices() async {
    final data = await _getList('/api/devices');
    return data.map((e) => Speaker.fromJson(e as Map<String, dynamic>)).toList();
  }

  Future<void> setAudioMode(String mode) => _post('/api/audio-mode', {'mode': mode});

  Future<void> setMasterVolume(int value) => _post('/api/master-volume', {'value': value});

  Future<void> setSpeakerVolume(int id, int value) =>
      _post('/api/speaker-volume', {'id': id, 'value': value});

  Future<List<int>> triggerDiscovery() async {
    final res = await http.post(Uri.parse('$baseUrl/api/discovery'))
        .timeout(const Duration(seconds: 30));
    if (res.statusCode != 200) throw Exception('HTTP ${res.statusCode}: ${res.body}');
    final data = jsonDecode(res.body) as Map<String, dynamic>;
    return (data['devices'] as List<dynamic>?)?.cast<int>() ?? [];
  }

  // ——— DSP avanzato ———

  Future<Map<String, dynamic>> getDspStatus() => _get('/api/dsp/status');

  Future<Map<String, dynamic>> getDspLevels() => _get('/api/dsp/levels');

  Future<List<DspPreset>> getDspPresets() async {
    final data = await _get('/api/dsp/presets');
    final list = data['presets'] as List<dynamic>? ?? [];
    return list.map((e) => DspPreset.fromJson(e as Map<String, dynamic>)).toList();
  }

  Future<void> applyDspPreset(String name, {int deviceId = 255}) =>
      _post('/api/dsp/preset/apply', {'name': name, 'deviceId': deviceId});

  Future<void> saveDspPreset(String name, String description) =>
      _post('/api/dsp/preset/save', {'name': name, 'description': description});

  Future<void> deleteDspPreset(String name) =>
      _delete('/api/dsp/preset', {'name': name});

  Future<void> setSpeakerGain(int id, double db) =>
      _post('/api/dsp/gain', {'id': id, 'value_db': db});

  Future<void> setSpeakerHpf(int id, double freq, {int slope = 1}) =>
      _post('/api/dsp/hpf', {'id': id, 'freq': freq, 'slope': slope});

  Future<void> setSpeakerLpf(int id, double freq, {int slope = 1}) =>
      _post('/api/dsp/lpf', {'id': id, 'freq': freq, 'slope': slope});

  Future<void> setSpeakerDelay(int id, double delayMs) =>
      _post('/api/dsp/delay', {'id': id, 'delay_ms': delayMs});

  Future<void> setSpeakerMute(int id, bool muted) =>
      _post('/api/dsp/mute', {'id': id, 'muted': muted});

  Future<void> setSpeakerPhase(int id, bool inverted) =>
      _post('/api/dsp/phase', {'id': id, 'inverted': inverted});

  Future<void> setEqBand(int id, int band, int freq, double gain, double q, int type) =>
      _post('/api/dsp/eq', {'id': id, 'band': band, 'freq': freq, 'gain': gain, 'q': q, 'type': type});

  Future<void> setCompressor(int id, {required int threshold, required int ratio,
      required int attack, required int release, required int makeup}) =>
      _post('/api/dsp/compressor', {
        'id': id, 'threshold': threshold, 'ratio': ratio,
        'attack': attack, 'release': release, 'makeup': makeup,
      });

  Future<void> setNoiseGate(int id, {required int threshold, required int attack, required int decay}) =>
      _post('/api/dsp/noise-gate', {'id': id, 'threshold': threshold, 'attack': attack, 'decay': decay});

  Future<void> setGroupControl(String groupType, String param, double value) =>
      _post('/api/dsp/group-control', {'groupType': groupType, 'param': param, 'value': value});

  // ——— AutoTune ———

  Future<void> startAutotune(String mode, {int? targetId}) =>
      _post('/api/dsp/autotune/start', {'mode': mode, if (targetId != null) 'targetId': targetId});

  Future<Map<String, dynamic>> getAutotuneStatus() => _get('/api/dsp/autotune/status');

  Future<void> applyAutotuneResults() => _post('/api/dsp/autotune/apply');

  Future<void> cancelAutotune() => _post('/api/dsp/autotune/cancel');

  // ——— AutoTune remoto ———
  Future<void> startAutotuneRemote({int? targetId}) =>
      _post('/api/autotune/start-remote',
          {if (targetId != null) 'targetId': targetId});

  Future<void> uploadAutotuneFFT(List<double> bands) =>
      _post('/api/autotune/upload-fft', {'bands': bands});

  Future<Map<String, dynamic>> getAutotuneSweepStatus() =>
      _get('/api/autotune/sweep-status');

  // ——— AutoTune con microfono USB di misura ———
  Future<void> startAutotuneUsbMic({int targetId = 0}) =>
      _post('/api/autotune/start-usb-mic', {'target_id': targetId});

  // ——— Calibrazione microfono di misura ———
  Future<void> uploadMicCalibration(MicCalibration cal) =>
      _post('/api/autotune/calibration', cal.toJson());

  Future<MicCalibration?> getMicCalibration() async {
    final data = await _get('/api/autotune/calibration');
    if (data['valid'] == false) return null;
    return MicCalibration.fromJson(data);
  }

  Future<void> deleteMicCalibration() =>
      _delete('/api/autotune/calibration');

  // ——— Upload dati FFT dallo smartphone (microfono USB o integrato) ———
  Future<void> uploadFftData(List<double> bands) =>
      _post('/api/autotune/upload-fft',
          {'bands': bands, 'num_bands': bands.length});

  // ——— Audio levels / spectrum ———
  Future<Map<String, dynamic>> getAudioLevels() => _get('/api/audio/levels');

  Future<Map<String, dynamic>> getAudioSpectrum() =>
      _get('/api/audio/spectrum');

  // ——— Venue map ———
  Future<Map<String, dynamic>> getVenueMap() => _get('/api/venue/map');

  Future<void> saveVenueMap(Map<String, dynamic> map) =>
      _post('/api/venue/map', map);

  Future<Map<String, dynamic>> calculateVenueDelays({
    required List<Map<String, dynamic>> speakers,
    required double listenerX,
    required double listenerY,
  }) =>
      _post('/api/venue/calculate-delays', {
        'speakers': speakers,
        'listenerPosition': {'x': listenerX, 'y': listenerY},
      });

  // ——— Gruppi ———
  Future<Map<String, dynamic>> getGroups() => _get('/api/groups');

  // ——— DMX Fixture ———

  Future<List<DmxFixture>> getDmxFixtures() async {
    final data = await _getList('/api/dmx/fixtures');
    return data.map((e) => DmxFixture.fromJson(e as Map<String, dynamic>)).toList();
  }

  Future<void> addDmxFixture(DmxFixture fixture) =>
      _post('/api/dmx/fixture', fixture.toJson());

  Future<void> removeDmxFixture(int id) =>
      _delete('/api/dmx/fixture', {'id': id});

  Future<void> setFixtureColor(int fixtureId, int r, int g, int b, {int w = 0}) =>
      _post('/api/dmx/fixture/color', {'fixtureId': fixtureId, 'r': r, 'g': g, 'b': b, 'w': w});

  Future<void> setFixtureDimmer(int fixtureId, int value) =>
      _post('/api/dmx/fixture/dimmer', {'fixtureId': fixtureId, 'value': value});

  Future<void> setFixtureChannel(int fixtureId, int channel, int value) =>
      _post('/api/dmx/fixture/channel', {'fixtureId': fixtureId, 'channel': channel, 'value': value});

  // ——— DMX Gruppi ———

  Future<List<dynamic>> getDmxGroups() async {
    final data = await _getList('/api/dmx/groups');
    return data;
  }

  Future<void> setGroupColor(int groupId, int r, int g, int b, {int w = 0, int? dimmer}) =>
      _post('/api/dmx/group/control', {
        'groupId': groupId, 'r': r, 'g': g, 'b': b, 'w': w,
        if (dimmer != null) 'dimmer': dimmer,
      });

  // ——— DMX Blackout / Master ———

  Future<void> dmxBlackout() => _post('/api/dmx/blackout');

  Future<void> setMasterDimmer(int value) =>
      _post('/api/dmx/master-dimmer', {'value': value});

  // ——— DMX Scene ———

  Future<List<DmxScene>> getDmxScenes() async {
    final data = await _getList('/api/dmx/scenes');
    return data.map((e) => DmxScene.fromJson(e as Map<String, dynamic>)).toList();
  }

  Future<void> playScene(int sceneId) =>
      _post('/api/dmx/scene/play', {'sceneId': sceneId});

  Future<void> stopScene() => _post('/api/dmx/scene/stop');

  Future<void> saveDmxScene(DmxScene scene) =>
      _post('/api/dmx/scene/save', scene.toJson());

  Future<void> deleteDmxScene(int sceneId) =>
      _delete('/api/dmx/scene', {'sceneId': sceneId});

  // ——— Audio-Reactive ———

  Future<Map<String, dynamic>> getAudioReactiveStatus() =>
      _get('/api/dmx/audio-reactive/status');

  Future<void> enableAudioReactive(bool enabled, {int? scenarioId}) =>
      _post('/api/dmx/audio-reactive/enable', {
        'enabled': enabled,
        if (scenarioId != null) 'scenarioId': scenarioId,
      });

  Future<List<AudioReactiveConfig>> getAudioReactiveScenarios() async {
    final data = await _getList('/api/dmx/audio-reactive/scenarios');
    return data.map((e) => AudioReactiveConfig.fromJson(e as Map<String, dynamic>)).toList();
  }

  Future<Map<String, dynamic>> getAudioReactiveSpectrum() =>
      _get('/api/dmx/audio-reactive/spectrum');

  // ——— USB Import/Export ———

  Future<void> exportConfig() async { await _get('/api/export/config'); }
  Future<void> importConfig() async { await _post('/api/import/config'); }
  Future<void> exportPresets() async { await _get('/api/export/presets'); }
  Future<void> exportScenes() async { await _get('/api/export/scenes'); }

  // ——— Sistema ———

  Future<Map<String, dynamic>> getSystemInfo() => _get('/api/system/info');

  Future<void> setSystemMode(String mode, {String? password}) =>
      _post('/api/system/mode', {'mode': mode, if (password != null) 'password': password});

  Future<Map<String, dynamic>> getUsbStatus() => _get('/api/system/usb-status');
}
