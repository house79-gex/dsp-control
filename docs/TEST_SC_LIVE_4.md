# Test Checklist: Integrazione SC LIVE 4

## Pre-Test
- [ ] SC LIVE 4 alimentata e funzionante
- [ ] DSP-Control alimentato (12V 3A)
- [ ] Cavi XLR bilanciati collegati (Main L/R → Input L/R)
- [ ] Firmware DSP-Control aggiornato (sample rate 44.1 kHz)

## Test Audio Input

### T1: Verifica Sample Rate
```
GET /api/audio/input-gain
# Atteso: "sampleRate": 44100
```
- [ ] Sample rate corretto (44100 Hz)

### T2: Verifica Input Gain
```
GET /api/audio/input-gain
# Atteso: "inputGainDb": -14.0
```
- [ ] Input gain configurato a -14 dB

### T3: Test Livello Audio (No Clip)
1. SC LIVE 4: Play sine 1 kHz @ 0 dBFS
2. Verificare: nessun LED clip su DSP-Control
3. Misurare: livello ADC ~0 dBFS (non >-1 dBFS)
- [ ] Nessun clip con sine @ 0 dBFS

### T4: Test SNR (Rumore di Fondo)
1. SC LIVE 4: Stop playback (silence)
2. Misurare: floor noise DSP-Control
3. Target: <-85 dBFS (A-weighted)
- [ ] SNR >85 dB

### T5: Test Frequency Response
1. SC LIVE 4: Play sweep 20 Hz–20 kHz @ -20 dBFS
2. Misurare: risposta DSP-Control
3. Target: ±1 dB 20 Hz–20 kHz
- [ ] Frequency response piatta

## Test Wireless Audio

### T6: Latency End-to-End
1. SC LIVE 4: Play impulse
2. Misurare: delay SC LIVE 4 → Cassa wireless
3. Target: <30ms
- [ ] Latency <30ms

### T7: Packet Loss
1. Wireless TX attivo per 5 minuti
2. `GET /api/wireless/status`
3. Target: packet loss <0.5%
- [ ] Packet loss <0.5%

## Test Audio-Reactive

### T8: Kick Detection
1. SC LIVE 4: Play traccia con kick 4/4
2. Verificare: luci Sub Bass pulsano on beat
3. Target: latenza <50ms
- [ ] Kick → Luci sincronizzati

### T9: Hi-Hat Flicker
1. SC LIVE 4: Play traccia con hi-hat 1/16
2. Verificare: luci High Mids seguono hi-hat
- [ ] Hi-hat → Luci reattivi

### T10: Spectrum Full
1. SC LIVE 4: Play traccia complessa (techno/house)
2. Verificare: tutte le 6 bande attive
3. Verificare: colori diversi per banda
- [ ] Full spectrum audio-reactive funzionante

## Test Integrazione App

### T11: Audio Settings Screen
1. App: Impostazioni → Audio Settings
2. Verificare: gain slider -96 a +24 dB
3. Cambiare gain a -10 dB
4. Verificare: `POST /api/audio/input-gain` inviato
- [ ] Audio Settings funzionante

### T12: Preset SC LIVE 4
1. App: Audio Settings → Preset "SC LIVE 4 (+18 dBu)"
2. Verificare: gain impostato a -14 dB
- [ ] Preset SC LIVE 4 funzionante

## Pass Criteria

- [ ] Tutti i test T1-T12 passati
- [ ] Nessun crash firmware per >1 ora di continuous playback
- [ ] App stabile (no crash durante i test)

## Failure Handling

| Test Fallito | Azione |
|--------------|--------|
| T3 (Clip) | Ridurre gain SC LIVE 4 o aumentare attenuazione DSP |
| T6 (Latency) | Verificare wireless range, ridurre buffer |
| T7 (Packet Loss) | Avvicinare moduli RX, verificare interferenze |
| T8-T10 (Audio-Reactive) | Aumentare sensibilità bande, verificare FFT |
