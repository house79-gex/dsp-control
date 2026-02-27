#include "web_server.h"
#include "config.h"
#include "rs485.h"
#include "audio_mode.h"
#include "storage.h"
#include "dsp_control.h"
#include "dsp_protocol.h"
#include "dmx512.h"
#include "audio_reactive.h"
#include "autotune.h"
#include "usb_storage.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <math.h>

static AsyncWebServer s_server(80);

// Lista assegnazioni in RAM
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
    * { box-sizing: border-box; }
    body { font-family: sans-serif; background: #1a1a2e; color: #eee; margin: 0; padding: 16px; }
    h1 { color: #e94560; text-align: center; margin-bottom: 16px; }
    h2 { color: #0f3460; font-size: 1em; margin: 0 0 8px; }
    .tabs { display: flex; gap: 4px; margin-bottom: 16px; flex-wrap: wrap; }
    .tab { background: #16213e; color: #aaa; border: none; padding: 8px 16px;
           border-radius: 6px; cursor: pointer; font-size: 0.9em; }
    .tab.active { background: #e94560; color: white; }
    .panel { display: none; }
    .panel.active { display: block; }
    .card { background: #16213e; border-radius: 10px; padding: 16px; margin-bottom: 12px; }
    label { display: block; margin-bottom: 4px; font-size: 0.85em; color: #aaa; }
    input[type=range] { width: 100%; accent-color: #e94560; }
    input[type=text], select { background: #0f3460; color: #eee; border: 1px solid #333;
      padding: 6px 10px; border-radius: 4px; width: 100%; margin-bottom: 8px; }
    button { background: #e94560; color: white; border: none; padding: 8px 16px;
      border-radius: 6px; cursor: pointer; margin: 4px; font-size: 0.9em; }
    button.secondary { background: #0f3460; }
    button:active { opacity: 0.7; }
    .led { display: inline-block; width: 10px; height: 10px; border-radius: 50%;
           background: #555; margin-right: 6px; }
    .led.green { background: #00e676; }
    .led.red   { background: #e94560; }
    #log { background: #0f3460; border-radius: 6px; padding: 8px;
           font-size: 0.75em; max-height: 100px; overflow-y: auto; font-family: monospace; }
    .vu-bar { background: #0f3460; border-radius: 3px; overflow: hidden; height: 12px; margin: 2px 0; }
    .vu-fill { height: 100%; background: linear-gradient(90deg, #00e676, #e94560); transition: width 0.1s; }
    .fixture-card { background: #0f3460; border-radius: 8px; padding: 10px; margin: 6px; display: inline-block; min-width: 140px; }
    .color-preview { width: 40px; height: 40px; border-radius: 50%; display: inline-block; border: 2px solid #333; }
    .scene-card { background: #16213e; border-radius: 8px; padding: 10px; margin: 6px; display: inline-block; cursor: pointer; }
    .scene-card:hover { border: 1px solid #e94560; }
    .band-bar { background: #0f3460; border-radius: 3px; overflow: hidden; width: 20px; display: inline-block; height: 60px; vertical-align: bottom; margin: 1px; }
    .band-fill { background: #e94560; width: 100%; transition: height 0.05s; }
    @media (max-width: 480px) { .tabs { font-size: 0.8em; } }
  </style>
</head>
<body>
  <h1>🔊 Sistema Audio PA</h1>
  <div class="tabs">
    <button class="tab active" onclick="showTab('audio')">🎚 Audio</button>
    <button class="tab" onclick="showTab('luci')">💡 Luci</button>
    <button class="tab" onclick="showTab('preset')">📦 Preset</button>
    <button class="tab" onclick="showTab('impostazioni')">⚙️ Impostazioni</button>
  </div>

  <!-- ===== TAB AUDIO ===== -->
  <div id="tab-audio" class="panel active">
    <div class="card">
      <label>Volume Master: <span id="masterVal">80</span>%</label>
      <input type="range" id="masterVol" min="0" max="100" value="80"
             oninput="masterVal.textContent=this.value" onchange="setMasterVol(this.value)">
    </div>
    <div class="card">
      <label>Modalità Audio</label>
      <button onclick="setMode('mixer')">🎚 MixerPassThrough</button>
      <button onclick="setMode('test')">🔉 TestTone 1kHz</button>
    </div>
    <div class="card">
      <label>Stato DSP CQ260D <span id="dspLed" class="led"></span><span id="dspStatus">-</span></label>
      <button class="secondary" onclick="loadDspStatus()">Aggiorna</button>
    </div>
    <div class="card">
      <label>VU Meter</label>
      <div id="vuMeter"></div>
      <button class="secondary" onclick="startVuPoll()">▶ Avvia polling</button>
    </div>
    <div class="card">
      <label>Discovery RS-485</label>
      <button onclick="doDiscovery()">🔍 Scansione</button>
    </div>
    <div class="card" id="speakerList"><label>Casse</label>
      <p id="noSpeakers">Avvia scansione per mostrare le casse</p>
    </div>
    <div class="card">
      <label>Controllo Cassa (ID)</label>
      <input type="text" id="spId" placeholder="ID cassa (es. 1)" style="width:80px">
      <button onclick="muteSpk()">Mute</button>
      <button onclick="unmuteSpk()">Unmute</button>
      <label>Gain (dB): <span id="gainVal">0</span></label>
      <input type="range" id="gainSlider" min="-60" max="12" value="0"
             oninput="gainVal.textContent=this.value" onchange="setGain(this.value)">
      <label>HPF (Hz): <span id="hpfVal">80</span></label>
      <input type="range" id="hpfSlider" min="20" max="2000" value="80"
             oninput="hpfVal.textContent=this.value" onchange="setHpf(this.value)">
      <label>LPF (Hz): <span id="lpfVal">20000</span></label>
      <input type="range" id="lpfSlider" min="200" max="20000" value="20000"
             oninput="lpfVal.textContent=this.value" onchange="setLpf(this.value)">
      <label>Delay (ms): <span id="delayVal">0</span></label>
      <input type="range" id="delaySlider" min="0" max="100" value="0"
             oninput="delayVal.textContent=this.value" onchange="setDelay(this.value)">
    </div>
    <div class="card">
      <label>AutoTune</label>
      <button onclick="startAutotune('single')">🎯 Singola cassa</button>
      <button onclick="startAutotune('all')">🎯 Impianto completo</button>
      <div id="autotuneStatus" style="margin-top:8px;font-size:0.85em;color:#aaa"></div>
    </div>
  </div>

  <!-- ===== TAB LUCI ===== -->
  <div id="tab-luci" class="panel">
    <div class="card">
      <label>Master Dimmer: <span id="masterDimVal">100</span>%</label>
      <input type="range" id="masterDim" min="0" max="100" value="100"
             oninput="masterDimVal.textContent=this.value"
             onchange="setMasterDimmer(Math.round(this.value*2.55))">
      <button style="background:#c00;margin-top:8px" onclick="blackout()">⚫ BLACKOUT</button>
    </div>
    <div class="card">
      <label>Fixture <span id="fixtureCount">0</span></label>
      <div id="fixtureGrid"></div>
      <button class="secondary" onclick="loadFixtures()">Aggiorna</button>
    </div>
    <div class="card">
      <label>Scene</label>
      <div id="sceneGrid"></div>
      <button class="secondary" onclick="loadScenes()">Aggiorna</button>
      <button onclick="stopScene()">⏹ Ferma scena</button>
    </div>
    <div class="card">
      <label>Audio-Reactive <span id="arLed" class="led"></span></label>
      <button onclick="toggleAudioReactive()">Toggle ON/OFF</button>
      <div id="spectrumBars" style="margin-top:8px"></div>
    </div>
  </div>

  <!-- ===== TAB PRESET ===== -->
  <div id="tab-preset" class="panel">
    <div class="card">
      <label>Preset DSP</label>
      <div id="presetList"></div>
      <button class="secondary" onclick="loadPresets()">Aggiorna</button>
    </div>
    <div class="card">
      <label>Salva preset corrente come:</label>
      <input type="text" id="newPresetName" placeholder="Nome preset">
      <input type="text" id="newPresetDesc" placeholder="Descrizione">
      <button onclick="savePreset()">💾 Salva</button>
    </div>
  </div>

  <!-- ===== TAB IMPOSTAZIONI ===== -->
  <div id="tab-impostazioni" class="panel">
    <div class="card">
      <label>Info Sistema</label>
      <div id="sysInfo" style="font-size:0.85em;color:#aaa"></div>
      <button class="secondary" onclick="loadSysInfo()">Aggiorna</button>
    </div>
    <div class="card">
      <label>USB-C Import/Export</label>
      <button onclick="exportConfig()">⬇️ Esporta Config</button>
      <button onclick="importConfig()">⬆️ Importa Config</button>
    </div>
    <div class="card">
      <label>Modalità utente</label>
      <button onclick="setMode2('base')">🔒 Base</button>
      <button onclick="setMode2('expert')">🔓 Esperto</button>
    </div>
    <div class="card"><label>Log</label><div id="log"></div></div>
  </div>

  <script>
    function showTab(name) {
      document.querySelectorAll('.panel').forEach(p => p.classList.remove('active'));
      document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
      document.getElementById('tab-' + name).classList.add('active');
      event.target.classList.add('active');
      if (name === 'luci') { loadFixtures(); loadScenes(); }
      if (name === 'preset') loadPresets();
      if (name === 'impostazioni') loadSysInfo();
    }
    function log(msg) {
      const el = document.getElementById('log');
      el.innerHTML = '<div>' + new Date().toLocaleTimeString() + ' – ' + msg + '</div>' + el.innerHTML;
      el.innerHTML = el.innerHTML.slice(0, 3000);
    }
    async function api(method, path, body) {
      try {
        const opts = { method, headers: body ? {'Content-Type':'application/json'} : {} };
        if (body) opts.body = JSON.stringify(body);
        const r = await fetch(path, opts);
        return await r.json();
      } catch(e) { log('Errore: ' + e); return null; }
    }
    async function setMasterVol(v) { await api('POST','/api/master-volume',{value:parseInt(v)}); log('Volume→'+v+'%'); }
    async function setMode(m) { await api('POST','/api/audio-mode',{mode:m}); log('Modalità→'+m); }
    async function doDiscovery() {
      log('Scansione RS-485...');
      const d = await api('POST','/api/discovery');
      if (d) { log('Trovati: '+JSON.stringify(d.devices)); loadSpeakers(); }
    }
    async function loadSpeakers() {
      const s = await api('GET','/api/devices');
      if (!s) return;
      const list = document.getElementById('speakerList');
      document.getElementById('noSpeakers').style.display = s.length ? 'none' : 'block';
      s.forEach(sp => {
        if (!document.getElementById('sp_'+sp.id)) {
          const d = document.createElement('div');
          d.id = 'sp_'+sp.id;
          d.innerHTML = `<label>${sp.name||('Cassa '+sp.id)} [${sp.type}]: <span id="spV_${sp.id}">80</span>%</label>
          <input type="range" min="0" max="100" value="80"
            oninput="document.getElementById('spV_${sp.id}').textContent=this.value"
            onchange="setSpeakerVol(${sp.id},this.value)">`;
          list.appendChild(d);
        }
      });
    }
    async function setSpeakerVol(id,v) { await api('POST','/api/speaker-volume',{id:id,value:parseInt(v)}); }
    async function loadDspStatus() {
      const d = await api('GET','/api/dsp/status');
      if (!d) return;
      const led = document.getElementById('dspLed');
      led.className = 'led ' + (d.connected ? 'green' : 'red');
      document.getElementById('dspStatus').textContent = d.connected ? 'Connesso' : 'Disconnesso';
    }
    let vuInterval = null;
    function startVuPoll() {
      if (vuInterval) { clearInterval(vuInterval); vuInterval = null; return; }
      vuInterval = setInterval(async () => {
        const d = await api('GET','/api/dsp/levels');
        if (!d) return;
        const vm = document.getElementById('vuMeter');
        vm.innerHTML = '';
        (d.inputLevels||[]).forEach((v,i) => {
          const pct = Math.max(0, Math.min(100, (v+60)*100/72));
          vm.innerHTML += `<div>In ${i+1}: <div class="vu-bar"><div class="vu-fill" style="width:${pct}%"></div></div></div>`;
        });
        (d.outputLevels||[]).forEach((v,i) => {
          const pct = Math.max(0, Math.min(100, (v+60)*100/72));
          vm.innerHTML += `<div>Out ${i+1}: <div class="vu-bar"><div class="vu-fill" style="width:${pct}%"></div></div></div>`;
        });
      }, 100);
    }
    function getSpId() { return parseInt(document.getElementById('spId').value)||1; }
    async function muteSpk()   { await api('POST','/api/dsp/mute',{id:getSpId(),muted:true}); log('Mute '+getSpId()); }
    async function unmuteSpk() { await api('POST','/api/dsp/mute',{id:getSpId(),muted:false}); log('Unmute '+getSpId()); }
    async function setGain(v)  { await api('POST','/api/dsp/gain',{id:getSpId(),channel:0,value_db:parseFloat(v)}); }
    async function setHpf(v)   { await api('POST','/api/dsp/hpf',{id:getSpId(),freq:parseFloat(v),slope:1}); }
    async function setLpf(v)   { await api('POST','/api/dsp/lpf',{id:getSpId(),freq:parseFloat(v),slope:1}); }
    async function setDelay(v) { await api('POST','/api/dsp/delay',{id:getSpId(),delay_ms:parseFloat(v)}); }
    async function startAutotune(m) {
      const d = await api('POST','/api/dsp/autotune/start',{mode:m});
      log('Autotune avviato: '+JSON.stringify(d));
      checkAutotune();
    }
    async function checkAutotune() {
      const d = await api('GET','/api/dsp/autotune/status');
      if (!d) return;
      document.getElementById('autotuneStatus').textContent = d.message + ' (' + Math.round(d.progress*100) + '%)';
      if (d.state !== 'Complete' && d.state !== 'Idle' && d.state !== 'Error') {
        setTimeout(checkAutotune, 500);
      }
    }
    // LUCI
    async function loadFixtures() {
      const d = await api('GET','/api/dmx/fixtures');
      if (!d) return;
      document.getElementById('fixtureCount').textContent = d.length;
      const grid = document.getElementById('fixtureGrid');
      grid.innerHTML = '';
      d.forEach(fx => {
        const r = fx.channels[0]||0, g = fx.channels[1]||0, b = fx.channels[2]||0;
        grid.innerHTML += `<div class="fixture-card">
          <div class="color-preview" style="background:rgb(${r},${g},${b})"></div>
          <div style="font-size:0.8em;margin-top:4px">${fx.name}</div>
          <div style="font-size:0.75em;color:#aaa">${fx.type} @${fx.startAddress}</div>
          <input type="color" onchange="setFxColor(${fx.id},this.value)" style="margin-top:4px">
        </div>`;
      });
    }
    async function setFxColor(id, hex) {
      const r = parseInt(hex.slice(1,3),16), g = parseInt(hex.slice(3,5),16), b = parseInt(hex.slice(5,7),16);
      await api('POST','/api/dmx/fixture/color',{fixtureId:id,r,g,b});
      setTimeout(loadFixtures, 300);
    }
    async function setMasterDimmer(v) { await api('POST','/api/dmx/master-dimmer',{value:v}); }
    async function blackout() { await api('POST','/api/dmx/blackout',{}); log('BLACKOUT'); }
    async function loadScenes() {
      const d = await api('GET','/api/dmx/scenes');
      if (!d) return;
      const grid = document.getElementById('sceneGrid');
      grid.innerHTML = '';
      d.forEach(sc => {
        grid.innerHTML += `<div class="scene-card" onclick="playScene(${sc.id})">
          <div style="font-weight:bold">${sc.name}</div>
          <div style="font-size:0.75em;color:#aaa">ID:${sc.id}</div>
        </div>`;
      });
    }
    async function playScene(id) { await api('POST','/api/dmx/scene/play',{sceneId:id}); log('Scena '+id+' avviata'); }
    async function stopScene() { await api('POST','/api/dmx/scene/stop',{}); log('Scena fermata'); }
    let arEnabled = false;
    async function toggleAudioReactive() {
      arEnabled = !arEnabled;
      await api('POST','/api/dmx/audio-reactive/enable',{enabled:arEnabled});
      document.getElementById('arLed').className = 'led ' + (arEnabled ? 'green' : 'red');
      if (arEnabled) startSpectrumPoll();
    }
    let specInterval = null;
    function startSpectrumPoll() {
      if (specInterval) return;
      specInterval = setInterval(async () => {
        const d = await api('GET','/api/dmx/audio-reactive/spectrum');
        if (!d || !d.bands) return;
        const bars = document.getElementById('spectrumBars');
        bars.innerHTML = '';
        d.bands.forEach(v => {
          bars.innerHTML += `<div class="band-bar"><div class="band-fill" style="height:${Math.round(v*100)}%"></div></div>`;
        });
      }, 100);
    }
    // PRESET
    async function loadPresets() {
      const d = await api('GET','/api/dsp/presets');
      if (!d) return;
      const list = document.getElementById('presetList');
      list.innerHTML = '';
      (d.presets||[]).forEach(p => {
        list.innerHTML += `<div style="display:flex;align-items:center;gap:8px;margin:4px 0">
          <span style="flex:1">${p.name}</span>
          <button onclick="applyPreset('${p.name}')">Applica</button>
          ${!p.isFactory ? '<button class="secondary" onclick="deletePreset(\''+p.name+'\')">Elimina</button>' : ''}
        </div>`;
      });
    }
    async function applyPreset(name) {
      await api('POST','/api/dsp/preset/apply',{name});
      log('Preset applicato: '+name);
    }
    async function savePreset() {
      const name = document.getElementById('newPresetName').value.trim();
      const desc = document.getElementById('newPresetDesc').value.trim();
      if (!name) return;
      await api('POST','/api/dsp/preset/save',{name,description:desc});
      log('Preset salvato: '+name);
      loadPresets();
    }
    async function deletePreset(name) {
      if (!confirm('Eliminare preset '+name+'?')) return;
      await api('DELETE','/api/dsp/preset',{name});
      loadPresets();
    }
    // SISTEMA
    async function loadSysInfo() {
      const d = await api('GET','/api/system/info');
      if (!d) return;
      document.getElementById('sysInfo').innerHTML =
        `Firmware: ${d.firmware_version} | Uptime: ${Math.round(d.uptime_ms/1000)}s | Heap: ${d.free_heap} byte`;
    }
    async function exportConfig() { await api('GET','/api/export/config'); log('Esportazione config avviata'); }
    async function importConfig() { await api('POST','/api/import/config',{}); log('Importazione config avviata'); }
    async function setMode2(m) {
      await api('POST','/api/system/mode',{mode:m});
      log('Modalità → '+m);
    }
    // Init
    loadSpeakers();
    loadDspStatus();
  </script>
</body>
</html>
)rawliteral";

// ======= Variabili di stato globali =======
static int s_masterVolume = 80;
static std::vector<int> s_speakerVolumes(128, 80);

static const char* roleToStrWeb(SpeakerRole role) {
    switch (role) {
        case SpeakerRole::SX:   return "SX";
        case SpeakerRole::DX:   return "DX";
        case SpeakerRole::SUB:  return "SUB";
        default:                return "MONO";
    }
}

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

// Macro helper per endpoint POST con body JSON
#define JSON_BODY_HANDLER(handler) \
    nullptr, \
    [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) { \
        StaticJsonDocument<1024> doc; \
        if (deserializeJson(doc, data, len)) { \
            req->send(400, "application/json", "{\"error\":\"bad json\"}"); \
            return; \
        } \
        handler \
    }

// ======= Setup WebServer =======
void web_server_init() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    Serial.printf("[WIFI] AP avviato: SSID=%s IP=%s\n", WIFI_SSID, WiFi.softAPIP().toString().c_str());

    // ——— Pagina principale ———
    s_server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", INDEX_HTML);
    });

    // ——— Endpoint originali (backward compat) ———
    s_server.on("/api/devices", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", buildDevicesJson());
    });

    s_server.on("/api/audio-mode", HTTP_POST, JSON_BODY_HANDLER({
        const char* mode = doc["mode"] | "mixer";
        setAudioMode(strcmp(mode, "test") == 0 ? AudioMode::TestTone : AudioMode::MixerPassThrough);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/master-volume", HTTP_POST, JSON_BODY_HANDLER({
        s_masterVolume = constrain((int)(doc["value"] | 80), 0, 100);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/speaker-volume", HTTP_POST, JSON_BODY_HANDLER({
        int id = doc["id"] | 0;
        if (id >= 0 && id < 128) s_speakerVolumes[id] = constrain((int)(doc["value"] | 80), 0, 100);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/discovery", HTTP_POST, [](AsyncWebServerRequest* req) {
        auto found = rs485_scan_devices();
        for (const auto& dev : found) {
            bool exists = false;
            for (const auto& a : g_assignments) if (a.deviceId == dev.id) { exists = true; break; }
            if (!exists) {
                SpeakerAssignment a;
                a.deviceId = dev.id; a.type = dev.type;
                a.position = "P" + std::to_string(dev.id);
                a.role = SpeakerRole::MONO;
                a.name = "Cassa " + std::to_string(dev.id);
                g_assignments.push_back(a);
            }
        }
        StaticJsonDocument<256> d;
        JsonArray arr = d.createNestedArray("devices");
        for (const auto& dev : found) arr.add(dev.id);
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    // ——— DSP avanzato ———

    s_server.on("/api/dsp/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        DspStatus* st = dsp_get_status();
        StaticJsonDocument<256> d;
        d["connected"]       = st->connected;
        d["download_ok"]     = st->downloadOk;
        d["device_group"]    = st->deviceGroup;
        d["detected_id"]     = st->detectedDspId;
        d["heartbeat"]       = st->heartbeatCounter;
        d["expert_mode"]     = dsp_is_expert_mode();
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dsp/levels", HTTP_GET, [](AsyncWebServerRequest* req) {
        DspLevelData* lv = dsp_get_level_data();
        StaticJsonDocument<512> d;
        JsonArray inArr  = d.createNestedArray("inputLevels");
        JsonArray outArr = d.createNestedArray("outputLevels");
        for (int i = 0; i < 4; i++) { inArr.add(lv->inputLevels[i]);  outArr.add(lv->outputLevels[i]); }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dsp/presets", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto names = dsp_get_preset_names();
        StaticJsonDocument<2048> d;
        JsonArray arr = d.createNestedArray("presets");
        for (const auto& n : names) {
            const DspPreset* p = dsp_get_preset(n);
            JsonObject obj = arr.createNestedObject();
            obj["name"]       = n.c_str();
            obj["description"]= p ? p->description : "";
            obj["isFactory"]  = p ? p->isFactory : false;
        }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dsp/preset/apply", HTTP_POST, JSON_BODY_HANDLER({
        const char* name = doc["name"] | "";
        uint8_t id = doc["deviceId"] | 0xFF;
        dsp_apply_preset(std::string(name), id);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/preset/save", HTTP_POST, JSON_BODY_HANDLER({
        const char* name = doc["name"] | "CUSTOM";
        const char* desc = doc["description"] | "";
        bool ok = dsp_save_preset(std::string(name), std::string(desc));
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"failed\"}");
    }));

    s_server.on("/api/dsp/preset", HTTP_DELETE, JSON_BODY_HANDLER({
        const char* name = doc["name"] | "";
        bool ok = dsp_delete_preset(std::string(name));
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"not found\"}");
    }));

    s_server.on("/api/dsp/gain", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_gain(doc["id"] | 1, doc["value_db"] | 0.0f);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/hpf", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_hpf(doc["id"] | 1, doc["freq"] | 80.0f, doc["slope"] | 1);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/lpf", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_lpf(doc["id"] | 1, doc["freq"] | 20000.0f, doc["slope"] | 1);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/delay", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_delay(doc["id"] | 1, doc["delay_ms"] | 0.0f);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/mute", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_mute(doc["id"] | 1, doc["muted"] | false);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/phase", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_speaker_phase(doc["id"] | 1, doc["inverted"] | false);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/eq", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_eq_band(doc["id"] | 1, doc["band"] | 0, doc["freq"] | 1000,
                        doc["gain"] | 0.0f, doc["q"] | 1.4f, doc["type"] | 0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/compressor", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_compressor(doc["id"]|1, doc["threshold"]|128, doc["ratio"]|4,
                           doc["attack"]|10, doc["release"]|50, doc["makeup"]|0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/noise-gate", HTTP_POST, JSON_BODY_HANDLER({
        dsp_set_noise_gate(doc["id"]|1, doc["threshold"]|50, doc["attack"]|10, doc["decay"]|100);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dsp/group-control", HTTP_POST, JSON_BODY_HANDLER({
        const char* grpStr = doc["groupType"] | "ALL";
        DspGroupType grp = DspGroupType::ALL;
        if (strcmp(grpStr, "SX")  == 0) grp = DspGroupType::SX;
        if (strcmp(grpStr, "DX")  == 0) grp = DspGroupType::DX;
        if (strcmp(grpStr, "SUB") == 0) grp = DspGroupType::SUB;
        const char* param = doc["param"] | "gain";
        float value = doc["value"] | 0.0f;
        if (strcmp(param, "gain") == 0) dsp_set_group_gain(grp, value);
        else if (strcmp(param, "mute") == 0) dsp_set_group_mute(grp, value > 0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    // ——— AutoTune ———

    s_server.on("/api/dsp/autotune/start", HTTP_POST, JSON_BODY_HANDLER({
        const char* mode = doc["mode"] | "single";
        uint8_t targetId = doc["targetId"] | 1;
        bool ok = autotune_start(
            strcmp(mode, "all") == 0 ? AutotuneMode::All : AutotuneMode::Single,
            targetId);
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"busy\"}");
    }));

    s_server.on("/api/dsp/autotune/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        const AutotuneStatus* st = autotune_get_status();
        StaticJsonDocument<512> d;
        const char* stateStr = "Idle";
        switch (st->state) {
            case AutotuneState::Muting:     stateStr = "Muting";    break;
            case AutotuneState::Generating: stateStr = "Generating";break;
            case AutotuneState::Capturing:  stateStr = "Capturing"; break;
            case AutotuneState::Analyzing:  stateStr = "Analyzing"; break;
            case AutotuneState::ApplyingEQ: stateStr = "ApplyingEQ";break;
            case AutotuneState::Complete:   stateStr = "Complete";  break;
            case AutotuneState::Error:      stateStr = "Error";     break;
            default: break;
        }
        d["state"]    = stateStr;
        d["progress"] = st->progress;
        d["message"]  = st->statusMessage;
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dsp/autotune/apply", HTTP_POST, [](AsyncWebServerRequest* req) {
        bool ok = autotune_apply_results();
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"no results\"}");
    });

    s_server.on("/api/dsp/autotune/cancel", HTTP_POST, [](AsyncWebServerRequest* req) {
        autotune_cancel();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // ——— DMX Fixture ———

    s_server.on("/api/dmx/fixtures", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto fixtures = dmx_get_all_fixtures();
        StaticJsonDocument<8192> d;
        JsonArray arr = d.to<JsonArray>();
        for (auto* fx : fixtures) {
            JsonObject obj = arr.createNestedObject();
            obj["id"]           = fx->id;
            obj["name"]         = fx->name;
            obj["type"]         = fx->type;
            obj["startAddress"] = fx->startAddress;
            obj["numChannels"]  = fx->numChannels;
            obj["groupId"]      = fx->groupId;
            obj["groupName"]    = fx->groupName;
            obj["online"]       = fx->online;
            JsonArray chArr = obj.createNestedArray("channels");
            for (int i = 0; i < fx->numChannels; i++) chArr.add(fx->channels[i]);
        }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dmx/fixture", HTTP_POST, JSON_BODY_HANDLER({
        DmxFixture fx = {};
        fx.id           = doc["id"]           | 1;
        fx.startAddress = doc["startAddress"] | 1;
        fx.numChannels  = doc["numChannels"]  | 3;
        fx.groupId      = doc["groupId"]      | 0;
        strncpy(fx.name,      doc["name"]      | "Fixture", sizeof(fx.name)-1);
        strncpy(fx.type,      doc["type"]      | "PAR_RGB", sizeof(fx.type)-1);
        strncpy(fx.groupName, doc["groupName"] | "",        sizeof(fx.groupName)-1);
        fx.online = true;
        dmx_add_fixture(fx);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/fixture", HTTP_DELETE, JSON_BODY_HANDLER({
        dmx_remove_fixture(doc["id"] | 0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/fixture/channel", HTTP_POST, JSON_BODY_HANDLER({
        dmx_set_fixture_channel(doc["fixtureId"]|1, doc["channel"]|0, doc["value"]|0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/fixture/color", HTTP_POST, JSON_BODY_HANDLER({
        dmx_set_fixture_color(doc["fixtureId"]|1, doc["r"]|0, doc["g"]|0, doc["b"]|0, doc["w"]|0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/fixture/dimmer", HTTP_POST, JSON_BODY_HANDLER({
        dmx_set_fixture_dimmer(doc["fixtureId"]|1, doc["value"]|255);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/blackout", HTTP_POST, [](AsyncWebServerRequest* req) {
        dmx_blackout();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    s_server.on("/api/dmx/master-dimmer", HTTP_POST, JSON_BODY_HANDLER({
        dmx_set_master_dimmer(doc["value"] | 255);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    // ——— DMX Gruppi ———

    s_server.on("/api/dmx/groups", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto groups = dmx_get_all_groups();
        StaticJsonDocument<4096> d;
        JsonArray arr = d.to<JsonArray>();
        for (auto* g : groups) {
            JsonObject obj = arr.createNestedObject();
            obj["id"]   = g->id;
            obj["name"] = g->name;
            JsonArray fxArr = obj.createNestedArray("fixtureIds");
            for (int i = 0; i < g->numFixtures; i++) fxArr.add(g->fixtureIds[i]);
        }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dmx/group/control", HTTP_POST, JSON_BODY_HANDLER({
        uint8_t gid = doc["groupId"] | 0;
        dmx_set_group_color(gid, doc["r"]|0, doc["g"]|0, doc["b"]|0, doc["w"]|0);
        if (doc.containsKey("dimmer")) dmx_set_group_dimmer(gid, doc["dimmer"] | 255);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    // ——— DMX Scene ———

    s_server.on("/api/dmx/scenes", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto scenes = dmx_get_all_scenes();
        StaticJsonDocument<4096> d;
        JsonArray arr = d.to<JsonArray>();
        for (auto* sc : scenes) {
            JsonObject obj = arr.createNestedObject();
            obj["id"]          = sc->id;
            obj["name"]        = sc->name;
            obj["fadeInTime"]  = sc->fadeInTime;
            obj["fadeOutTime"] = sc->fadeOutTime;
            obj["holdTime"]    = sc->holdTime;
            obj["loop"]        = sc->loop;
            obj["speed"]       = sc->speed;
            obj["isFactory"]   = sc->isFactory;
        }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dmx/scene/play", HTTP_POST, JSON_BODY_HANDLER({
        dmx_play_scene(doc["sceneId"] | 1);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/scene/stop", HTTP_POST, [](AsyncWebServerRequest* req) {
        dmx_stop_scene();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    s_server.on("/api/dmx/scene/save", HTTP_POST, JSON_BODY_HANDLER({
        DmxScene sc = {};
        sc.id          = doc["id"]         | 100;
        sc.fadeInTime  = doc["fadeInTime"] | 0.0f;
        sc.fadeOutTime = doc["fadeOutTime"]| 0.0f;
        sc.holdTime    = doc["holdTime"]   | 0.0f;
        sc.loop        = doc["loop"]       | false;
        sc.speed       = doc["speed"]      | 1.0f;
        sc.isFactory   = false;
        strncpy(sc.name, doc["name"] | "Custom", sizeof(sc.name)-1);
        dmx_save_scene(sc);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/scene", HTTP_DELETE, JSON_BODY_HANDLER({
        dmx_delete_scene(doc["sceneId"] | 0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    // ——— Audio-Reactive ———

    s_server.on("/api/dmx/audio-reactive/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        StaticJsonDocument<512> d;
        d["enabled"]    = audio_reactive_is_enabled();
        d["scenarioId"] = audio_reactive_get_scenario();
        const float* levels = audio_reactive_get_band_levels();
        JsonArray arr = d.createNestedArray("bandLevels");
        for (int i = 0; i < AUDIO_NUM_BANDS; i++) arr.add(levels[i]);
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dmx/audio-reactive/enable", HTTP_POST, JSON_BODY_HANDLER({
        audio_reactive_enable(doc["enabled"] | false, doc["scenarioId"] | 0);
        req->send(200, "application/json", "{\"ok\":true}");
    }));

    s_server.on("/api/dmx/audio-reactive/scenarios", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto scenarios = audio_reactive_get_all_scenarios();
        StaticJsonDocument<4096> d;
        JsonArray arr = d.to<JsonArray>();
        for (auto* sc : scenarios) {
            JsonObject obj = arr.createNestedObject();
            obj["id"]        = sc->id;
            obj["name"]      = sc->name;
            obj["isFactory"] = sc->isFactory;
        }
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/dmx/audio-reactive/spectrum", HTTP_GET, [](AsyncWebServerRequest* req) {
        const float* levels = audio_reactive_get_band_levels();
        StaticJsonDocument<512> d;
        JsonArray arr = d.createNestedArray("bands");
        for (int i = 0; i < AUDIO_NUM_BANDS; i++) arr.add(levels[i]);
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    // ——— USB Import/Export ———

    s_server.on("/api/export/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        bool ok = usb_storage_export_config();
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"export failed\"}");
    });

    s_server.on("/api/import/config", HTTP_POST, [](AsyncWebServerRequest* req) {
        bool ok = usb_storage_import_config();
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"import failed\"}");
    });

    s_server.on("/api/export/presets", HTTP_GET, [](AsyncWebServerRequest* req) {
        bool ok = usb_storage_export_audio_reactive();
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"failed\"}");
    });

    s_server.on("/api/export/scenes", HTTP_GET, [](AsyncWebServerRequest* req) {
        bool ok = usb_storage_export_fixtures();
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"failed\"}");
    });

    // ——— Sistema ———

    s_server.on("/api/system/info", HTTP_GET, [](AsyncWebServerRequest* req) {
        StaticJsonDocument<256> d;
        d["firmware_version"] = "2.0.0";
        d["uptime_ms"]        = millis();
        d["free_heap"]        = ESP.getFreeHeap();
        d["chip_model"]       = ESP.getChipModel();
        d["dsp_connected"]    = dsp_get_status()->connected;
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/system/mode", HTTP_POST, JSON_BODY_HANDLER({
        const char* mode = doc["mode"] | "base";
        const char* pass = doc["password"] | "";
        bool ok = dsp_set_expert_mode(strcmp(mode, "expert") == 0, pass);
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"error\":\"wrong password\"}");
    }));

    s_server.on("/api/system/usb-status", HTTP_GET, [](AsyncWebServerRequest* req) {
        StaticJsonDocument<64> d;
        d["connected"] = usb_storage_is_ready();
        String out; serializeJson(d, out);
        req->send(200, "application/json", out);
    });

    // ——— 404 catch-all ———
    s_server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "text/plain", "Not found");
    });

    // ——— Audio levels ———
    s_server.on("/api/audio/levels", HTTP_GET, [](AsyncWebServerRequest* req) {
        float left, right;
        audio_get_channel_levels(left, right);
        float peak = audio_get_peak_db();
        StaticJsonDocument<256> doc;
        doc["master_db"] = peak;
        doc["left_db"]   = (left  > 1e-6f) ? 20.0f * log10f(left)  : -60.0f;
        doc["right_db"]  = (right > 1e-6f) ? 20.0f * log10f(right) : -60.0f;
        doc["peak"]      = (peak > -3.0f);
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ——— Audio spectrum FFT ———
    s_server.on("/api/audio/spectrum", HTTP_GET, [](AsyncWebServerRequest* req) {
        const FftResult& fft = audio_get_fft_result();
        StaticJsonDocument<1024> doc;
        JsonArray arr = doc.createNestedArray("bands");
        for (int i = 0; i < 32; i++) arr.add(fft.bands[i]);
        doc["timestamp"] = fft.timestamp;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ——— AutoTune remoto (smartphone) ———
    s_server.on("/api/autotune/start-remote", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<256> doc;
            deserializeJson(doc, data, len);
            uint8_t targetId = doc["targetId"] | 0;
            bool ok = autotune_start_remote(targetId);
            StaticJsonDocument<128> resp;
            resp["ok"]      = ok;
            resp["message"] = ok ? "AutoTune remoto avviato" : "Procedura già in corso";
            String out; serializeJson(resp, out);
            req->send(200, "application/json", out);
        });

    s_server.on("/api/autotune/upload-fft", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<2048> doc;
            auto err = deserializeJson(doc, data, len);
            if (err) { req->send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
            JsonArray arr = doc["bands"].as<JsonArray>();
            float bands[64];
            uint8_t n = 0;
            for (float v : arr) { if (n < 64) bands[n++] = v; }
            autotune_upload_fft(bands, n);
            req->send(200, "application/json", "{\"ok\":true}");
        });

    s_server.on("/api/autotune/sweep-status", HTTP_GET, [](AsyncWebServerRequest* req) {
        const AutotuneStatus* st = autotune_get_status();
        StaticJsonDocument<256> doc;
        doc["state"]    = (int)st->state;
        doc["progress"] = st->progress;
        doc["message"]  = st->statusMessage;
        doc["isRemote"] = autotune_is_remote_mode();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ——— AutoTune con microfono USB di misura (Dayton iMM-6C) ———
    s_server.on("/api/autotune/start-usb-mic", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<256> doc;
            deserializeJson(doc, data, len);
            uint8_t targetId = doc["target_id"] | 0;
            bool ok = autotune_start_usb_mic(targetId);
            StaticJsonDocument<128> resp;
            resp["ok"]      = ok;
            resp["message"] = ok ? "AutoTune microfono USB avviato" : "Procedura già in corso";
            String out; serializeJson(resp, out);
            req->send(200, "application/json", out);
        });

    // ——— Calibrazione microfono di misura ———
    s_server.on("/api/autotune/calibration", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            DynamicJsonDocument doc(4096);
            auto err = deserializeJson(doc, data, len);
            if (err) { req->send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }

            MicCalibration cal = {};
            const char* micName = doc["mic_name"] | "Sconosciuto";
            strncpy(cal.micName, micName, sizeof(cal.micName) - 1);
            JsonArray points = doc["points"].as<JsonArray>();
            for (JsonObject p : points) {
                if (cal.numPoints >= 128) break;
                cal.freqHz[cal.numPoints]       = p["freq"] | 0.0f;
                cal.correctionDb[cal.numPoints]  = p["correction"] | 0.0f;
                cal.numPoints++;
            }
            cal.valid = (cal.numPoints >= 2);
            autotune_load_calibration(&cal);
            StaticJsonDocument<128> resp;
            resp["ok"]        = cal.valid;
            resp["numPoints"] = cal.numPoints;
            String out; serializeJson(resp, out);
            req->send(200, "application/json", out);
        });

    s_server.on("/api/autotune/calibration", HTTP_GET, [](AsyncWebServerRequest* req) {
        const MicCalibration* cal = autotune_get_calibration();
        if (!cal->valid) {
            req->send(200, "application/json", "{\"valid\":false}");
            return;
        }
        DynamicJsonDocument doc(4096);
        doc["valid"]    = true;
        doc["mic_name"] = cal->micName;
        JsonArray points = doc.createNestedArray("points");
        for (uint8_t i = 0; i < cal->numPoints; i++) {
            JsonObject p = points.createNestedObject();
            p["freq"]       = cal->freqHz[i];
            p["correction"] = cal->correctionDb[i];
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    s_server.on("/api/autotune/calibration", HTTP_DELETE, [](AsyncWebServerRequest* req) {
        MicCalibration empty = {};
        autotune_load_calibration(&empty);
        storage_clear_mic_calibration();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // ——— Venue map ———
    static String s_venueMapJson = "{}";
    s_server.on("/api/venue/map", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", s_venueMapJson);
    });

    s_server.on("/api/venue/map", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            s_venueMapJson = String((char*)data, len);
            req->send(200, "application/json", "{\"ok\":true}");
        });

    s_server.on("/api/venue/calculate-delays", HTTP_POST, [](AsyncWebServerRequest* req) {},
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            StaticJsonDocument<4096> doc;
            auto err = deserializeJson(doc, data, len);
            if (err) { req->send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
            // Calcola e applica delay basati sulla distanza (Formula: delay_ms = dist_m / 343 * 1000)
            StaticJsonDocument<1024> resp;
            JsonArray delays = resp.createNestedArray("delays");
            JsonArray speakers = doc["speakers"].as<JsonArray>();
            JsonObject listener = doc["listenerPosition"].as<JsonObject>();
            float lx = listener["x"] | 0.0f;
            float ly = listener["y"] | 0.0f;
            for (JsonObject spk : speakers) {
                float sx = spk["x"] | 0.0f;
                float sy = spk["y"] | 0.0f;
                int   id = spk["id"] | 0;
                float dx = sx - lx, dy = sy - ly;
                float dist    = sqrtf(dx * dx + dy * dy);
                float delayMs = dist / 343.0f * 1000.0f;
                if (id > 0) dsp_set_speaker_delay((uint8_t)id, delayMs);
                JsonObject d = delays.createNestedObject();
                d["id"]       = id;
                d["dist_m"]   = dist;
                d["delay_ms"] = delayMs;
            }
            String out; serializeJson(resp, out);
            req->send(200, "application/json", out);
        });

    // ——— Gruppi personalizzabili ———
    s_server.on("/api/groups", HTTP_GET, [](AsyncWebServerRequest* req) {
        StaticJsonDocument<2048> doc;
        JsonArray arr = doc.createNestedArray("groups");
        for (auto* g : dmx_get_all_groups()) {
            JsonObject obj = arr.createNestedObject();
            obj["id"]   = g->id;
            obj["name"] = g->name;
            obj["type"] = "dmx";
            JsonArray fids = obj.createNestedArray("fixtureIds");
            for (int i = 0; i < g->numFixtures; i++) fids.add(g->fixtureIds[i]);
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    s_server.begin();
    Serial.println("[WEB] Server HTTP avviato su porta 80 con tutti gli endpoint");
}

void web_server_handle() {
    // ESPAsyncWebServer è completamente asincrono: nulla da fare nel loop
}
