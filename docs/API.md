# DSP Control REST API Documentation

Base URL: `http://192.168.4.1` (WiFi AP mode)

## Audio Endpoints

### GET `/api/audio/levels`
Returns current VU meter levels.

**Response**:
```json
{
  "left": -12.5,
  "right": -14.2,
  "peak_left": -6.8,
  "peak_right": -7.1
}
```

### GET `/api/audio/fft`
Returns 32-band FFT spectrum (20Hz-20kHz).

**Response**:
```json
{
  "bands": [0.2, 0.5, 0.8, 0.1],
  "sample_rate": 44100
}
```

### POST `/api/audio/mode`
Set audio relay mode.

**Body**:
```json
{
  "mode": "mixer"
}
```

## DSP Endpoints

### GET `/api/dsp/devices`
List discovered DSP devices on RS-485 bus.

**Response**:
```json
{
  "devices": [
    {
      "address": 1,
      "model": "CQ260D",
      "firmware": 12
    }
  ]
}
```

### POST `/api/dsp/{addr}/volume`
Set master volume for DSP device.

**Body**:
```json
{
  "volume": 75
}
```

### POST `/api/dsp/{addr}/eq`
Set EQ band gain.

**Body**:
```json
{
  "band": 0,
  "gain": -3.5
}
```

### POST `/api/dsp/{addr}/preset/save`
Save current settings to preset slot.

**Body**:
```json
{
  "slot": 1,
  "name": "My Preset"
}
```

### POST `/api/dsp/{addr}/preset/load`
Load preset from slot.

**Body**:
```json
{
  "slot": 1
}
```

### POST `/api/dsp/{addr}/crossover`
Set crossover frequency and slope.

**Body**:
```json
{
  "band": 0,
  "freq_hz": 80,
  "slope": 3
}
```

### POST `/api/dsp/{addr}/limiter`
Set limiter parameters.

**Body**:
```json
{
  "threshold_db": -6,
  "ratio": 10,
  "attack_ms": 10,
  "release_ms": 200
}
```

### POST `/api/dsp/{addr}/compressor`
Set compressor parameters.

**Body**:
```json
{
  "threshold_db": -12,
  "ratio": 4,
  "attack_ms": 20,
  "release_ms": 500,
  "makeup_db": 3
}
```

## DMX Endpoints

### GET `/api/dmx/fixtures`
List all configured DMX fixtures.

### POST `/api/dmx/fixture/{id}/rgb`
Set RGB color for PAR fixture.

**Body**:
```json
{
  "r": 255,
  "g": 0,
  "b": 128
}
```

### POST `/api/dmx/audio-reactive`
Enable/disable audio-reactive mode.

**Body**:
```json
{
  "enabled": true,
  "sensitivity": 80
}
```

## WLED Endpoints

### GET `/api/wled/status`
Get current WLED strip status.

### POST `/api/wled/color`
Set solid color.

**Body**:
```json
{
  "r": 255,
  "g": 100,
  "b": 50,
  "brightness": 200
}
```

### POST `/api/wled/effect`
Set effect mode.

**Body**:
```json
{
  "effect_id": 1,
  "speed": 128,
  "intensity": 200
}
```

## AutoTune Endpoints

### POST `/api/autotune/start`
Start local AutoTune sweep.

**Body**:
```json
{
  "mode": "local",
  "speaker_id": 1
}
```

### GET `/api/autotune/results`
Get AutoTune results.

**Response**:
```json
{
  "delay_ms": 12.5,
  "eq_adjustments": [
    {"band": 0, "gain": -2.1},
    {"band": 3, "gain": 1.5}
  ]
}
```

## System Endpoints

### GET `/api/system/info`
Get system information.

**Response**:
```json
{
  "version": "1.0.0",
  "uptime": 3600,
  "free_heap": 180000,
  "wifi_clients": 2
}
```

### POST `/api/system/restart`
Restart ESP32 controller.
