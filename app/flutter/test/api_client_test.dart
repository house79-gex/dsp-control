// api_client_test.dart – Unit test per modelli e serializzazione del sistema DSP-Control
//
// Eseguire con: flutter test test/api_client_test.dart
//
// Testa la deserializzazione dei modelli e la configurazione dell'ApiClient.

import 'package:flutter_test/flutter_test.dart';
import '../lib/models/dmx_fixture.dart';
import '../lib/models/dsp_preset.dart';
import '../lib/services/api_client.dart';

void main() {
  group('DmxFixture.fromJson', () {
    test('deserializza correttamente una fixture PAR RGB', () {
      final json = {
        'id': 1,
        'name': 'PAR Front L',
        'type': 'PAR_RGB',
        'startAddress': 1,
        'numChannels': 3,
        'groupId': 0,
        'groupName': '',
        'channels': [255, 128, 0],
        'online': true,
      };
      final fixture = DmxFixture.fromJson(json);
      expect(fixture.id, 1);
      expect(fixture.name, 'PAR Front L');
      expect(fixture.type, 'PAR_RGB');
      expect(fixture.startAddress, 1);
      expect(fixture.numChannels, 3);
      expect(fixture.channels[0], 255);
      expect(fixture.channels[1], 128);
      expect(fixture.channels[2], 0);
      expect(fixture.online, true);
    });

    test('usa valori di default per campi mancanti', () {
      final json = {'id': 2, 'name': 'MH Center', 'type': 'MOVING_HEAD',
                    'startAddress': 10, 'numChannels': 12};
      final fixture = DmxFixture.fromJson(json);
      expect(fixture.groupId, 0);
      expect(fixture.groupName, '');
      expect(fixture.online, true);
    });

    test('toJson produce JSON coerente con fromJson', () {
      final original = DmxFixture(
        id: 3, name: 'WASH Back L', type: 'WASH',
        startAddress: 25, numChannels: 7,
        channels: List.filled(7, 0),
      );
      final json = original.toJson();
      final restored = DmxFixture.fromJson(json);
      expect(restored.id, original.id);
      expect(restored.name, original.name);
      expect(restored.startAddress, original.startAddress);
    });
  });

  group('DspPreset.fromJson', () {
    test('deserializza preset di fabbrica', () {
      final json = {
        'name': '2WAY_STD',
        'description': 'Preset standard 2-vie',
        'isFactory': true,
      };
      final preset = DspPreset.fromJson(json);
      expect(preset.name, '2WAY_STD');
      expect(preset.isFactory, true);
    });
  });

  group('ApiClient configurazione', () {
    test('usa baseUrl di default 192.168.4.1', () {
      final client = ApiClient();
      expect(client.baseUrl, 'http://192.168.4.1');
    });

    test('accetta baseUrl personalizzato', () {
      final client = ApiClient(baseUrl: 'http://10.0.0.1');
      expect(client.baseUrl, 'http://10.0.0.1');
    });
  });
}

