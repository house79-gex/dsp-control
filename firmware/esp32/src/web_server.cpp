#include "web_server.h"
#include "config.h"
#include "rs485.h"
#include "audio_mode.h"
#include "storage.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

static AsyncWebServer s_server(80);

// Lista assegnazioni in RAM (aggiornata da discovery e salvata su NVS)
extern std::vector<SpeakerAssignment> g_assignments;

// ======= Pagina HTML inline =======
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sistema Audio PA</title>
  <style>
    body { font-family: sans-serif; background: #1a1a2e; color: #eee; margin: 0; padding: 16px; }
    h1   { color: #e94560; text-align: center; }
    .card { background: #16213e; border-radius: 10px; padding: 16px; margin-bottom: 12px; }
    label { display: block; margin-bottom: 4px; font-size: 0.9em; color: #aaa; }
    input[type=range] { width: 100%; accent-color: #e94560; }
    button {
      background: #e94560; color: white; border: none;
      padding: 10px 20px; border-radius: 6px; cursor: pointer;
      margin: 4px; font-size: 1em;
    }
    button:active { opacity: 0.7; }
    #log { background: #0f3460; border-radius: 6px; padding: 8px;
           font-size: 0.8em; max-height: 120px; overflow-y: auto; }
  </style>
</head>
<body>
  <h1>🔊 Sistema Audio PA</h1>

  <div class="card">
    <label>Volume Master: <span id="masterVal">80</span>%</label>
    <input type="range" id="masterVol" min="0" max="100" value="80"
           oninput="document.getElementById('masterVal').textContent=this.value"
           onchange="setMasterVol(this.value)">
  </div>

  <div class="card">
    <label>Modalità Audio</label>
    <button onclick="setMode('mixer')">🎚 MixerPassThrough</button>
    <button onclick="setMode('test')">🔉 TestTone 1kHz</button>
  </div>

  <div class="card">
    <label>Discovery RS-485</label>
    <button onclick="doDiscovery()">🔍 Scansione</button>
  </div>

  <div class="card" id="speakerList">
    <label>Casse</label>
    <p id="noSpeakers">Avvia scansione per mostrare le casse</p>
  </div>

  <div class="card">
    <label>Log</label>
    <div id="log"></div>
  </div>

  <script>
    const BASE = '';

    function log(msg) {
      const el = document.getElementById('log');
      el.innerHTML += '<div>' + new Date().toLocaleTimeString() + ' – ' + msg + '</div>';
      el.scrollTop = el.scrollHeight;
    }

    async function setMasterVol(val) {
      await fetch(BASE + '/api/master-volume', {
        method: 'POST', headers: {'Content-Type':'application/json'},
        body: JSON.stringify({value: parseInt(val)})
      });
      log('Master volume → ' + val + '%');
    }

    async function setMode(mode) {
      await fetch(BASE + '/api/audio-mode', {
        method: 'POST', headers: {'Content-Type':'application/json'},
        body: JSON.stringify({mode: mode})
      });
      log('Modalità → ' + mode);
    }

    async function doDiscovery() {
      log('Scansione RS-485...');
      const res = await fetch(BASE + '/api/discovery', {method: 'POST'});
      const data = await res.json();
      log('Trovati: ' + JSON.stringify(data.devices));
      await loadSpeakers();
    }

    async function setSpeakerVol(id, val) {
      await fetch(BASE + '/api/speaker-volume', {
        method: 'POST', headers: {'Content-Type':'application/json'},
        body: JSON.stringify({id: id, value: parseInt(val)})
      });
    }

    async function loadSpeakers() {
      const res = await fetch(BASE + '/api/devices');
      const speakers = await res.json();
      const list = document.getElementById('speakerList');
      const noSp = document.getElementById('noSpeakers');
      if (speakers.length === 0) { noSp.style.display = 'block'; return; }
      noSp.style.display = 'none';
      speakers.forEach(sp => {
        let existing = document.getElementById('sp_' + sp.deviceId);
        if (!existing) {
          const div = document.createElement('div');
          div.id = 'sp_' + sp.deviceId;
          div.innerHTML = `<label>${sp.name || ('Cassa ' + sp.deviceId)} [${sp.type}] ${sp.position}:
            <span id="spVal_${sp.deviceId}">80</span>%</label>
            <input type="range" min="0" max="100" value="80"
              oninput="document.getElementById('spVal_${sp.deviceId}').textContent=this.value"
              onchange="setSpeakerVol(${sp.deviceId}, this.value)">`;
          list.appendChild(div);
        }
      });
    }

    // Carica stato iniziale
    loadSpeakers();
  </script>
</body>
</html>
)rawliteral";

// ======= Variabili di stato globali =======
static int s_masterVolume = 80;
static std::vector<int> s_speakerVolumes(128, 80);  // volume per ID (0–127)

// Converte SpeakerRole → stringa (locale a questo modulo)
static const char* roleToStrWeb(SpeakerRole role) {
    switch (role) {
        case SpeakerRole::SX:   return "SX";
        case SpeakerRole::DX:   return "DX";
        case SpeakerRole::SUB:  return "SUB";
        case SpeakerRole::MONO: return "MONO";
        default:                return "MONO";
    }
}

// ======= Serializzazione assignments → JSON =======
static String buildDevicesJson() {
    StaticJsonDocument<4096> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (const auto& a : g_assignments) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]       = a.deviceId;
        obj["position"] = a.position.c_str();
        obj["type"]     = a.type.c_str();
        obj["role"]     = roleToStrWeb(a.role);
        obj["name"]     = a.name.c_str();
        obj["volume"]   = s_speakerVolumes[a.deviceId];
    }
    String out;
    serializeJson(doc, out);
    return out;
}

// ======= Setup WebServer =======
void web_server_init() {
    // WiFi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    Serial.printf("[WIFI] AP avviato: SSID=%s IP=%s\n",
                  WIFI_SSID, WiFi.softAPIP().toString().c_str());

    // Pagina principale
    s_server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", INDEX_HTML);
    });

    // GET /api/devices
    s_server.on("/api/devices", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", buildDevicesJson());
    });

    // POST /api/audio-mode
    s_server.on("/api/audio-mode", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<64> doc;
            if (deserializeJson(doc, data, len)) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }
            const char* mode = doc["mode"] | "mixer";
            if (strcmp(mode, "test") == 0) {
                setAudioMode(AudioMode::TestTone);
            } else {
                setAudioMode(AudioMode::MixerPassThrough);
            }
            req->send(200, "application/json", "{\"ok\":true}");
        });

    // POST /api/master-volume
    s_server.on("/api/master-volume", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<64> doc;
            if (deserializeJson(doc, data, len)) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }
            s_masterVolume = doc["value"] | 80;
            s_masterVolume = constrain(s_masterVolume, 0, 100);
            Serial.printf("[WEB] Master volume → %d\n", s_masterVolume);
            req->send(200, "application/json", "{\"ok\":true}");
        });

    // POST /api/speaker-volume
    s_server.on("/api/speaker-volume", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<64> doc;
            if (deserializeJson(doc, data, len)) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }
            int id  = doc["id"]    | 0;
            int val = doc["value"] | 80;
            if (id >= 0 && id < 128) {
                s_speakerVolumes[id] = constrain(val, 0, 100);
                Serial.printf("[WEB] Volume cassa ID=%d → %d\n", id, val);
            }
            req->send(200, "application/json", "{\"ok\":true}");
        });

    // POST /api/discovery
    s_server.on("/api/discovery", HTTP_POST, [](AsyncWebServerRequest* req) {
        // Avvia scansione RS-485 (sincrona – breve timeout)
        auto found = rs485_scan_devices();
        g_assignments.clear();
        for (const auto& dev : found) {
            SpeakerAssignment a;
            a.deviceId = dev.id;
            a.type     = dev.type;
            a.position = "P" + std::to_string(dev.id);
            a.role     = SpeakerRole::MONO;
            a.name     = "Cassa " + std::to_string(dev.id);
            g_assignments.push_back(a);
        }

        StaticJsonDocument<256> doc;
        JsonArray arr = doc.createNestedArray("devices");
        for (const auto& dev : found) arr.add(dev.id);
        String out;
        serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // 404 catch-all
    s_server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "text/plain", "Not found");
    });

    s_server.begin();
    Serial.println("[WEB] Server HTTP avviato su porta 80");
}

void web_server_handle() {
    // ESPAsyncWebServer è completamente asincrono: nulla da fare nel loop
}
