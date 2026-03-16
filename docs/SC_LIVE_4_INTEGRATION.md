# Integrazione Denon DJ SC LIVE 4

## Panoramica

Guida completa per collegare e configurare il sistema DSP-Control con la console DJ Denon DJ SC LIVE 4.

## Specifiche SC LIVE 4

| Parametro | Valore |
|-----------|--------|
| **Output Main XLR** | +18 dBu max (bilanciato) |
| **Sample Rate** | 44.1 kHz (nativo) |
| **Bit Depth** | 24-bit |
| **Dynamic Range** | 110 dB (A-weighted) |
| **THD** | 0.0048% @ 1 kHz |
| **Frequency Response** | 20 Hz – 20 kHz (±0.5 dB) |

## Schema Collegamento (versione attuale con RCA sbilanciati)

```
SC LIVE 4 RCA Out L/R (linea sbilanciata)
           │
           ▼
Attenuatore passivo stereo (R1/R2 per canale)
           │
           ▼
DSP-Control Master ES8388 ADC (Line In, gain digitale 0 dB)
           │
           ├─► Elaborazione DSP (EQ, Crossover, Limiter)
           ├─► Wireless TX CH1/CH2 (ESP-NOW)
           └─► DMX Master Generator (Audio-Reactive)
           │
           ▼
Casse Remote (Modulo RX) + Luci DMX
```

## Configurazione Step-by-Step

### 1. Collegamento Hardware

1. **Cavo RCA sbilanciato** (RCA Out L) → **RCA In L** DSP-Control
2. **Cavo RCA sbilanciato** (RCA Out R) → **RCA In R** DSP-Control
3. Ogni RCA In passa per un **attenuatore passivo** verso l’ingresso linea ES8388
4. Alimentazione DSP-Control: 12V 3A (separata da SC LIVE 4)

### 2. Configurazione SC LIVE 4

**Opzione A: Livello moderato sugli RCA (RACCOMANDATO)**
- Menu SC LIVE 4 → Settings → Audio → Output Level → valore intermedio (evitare “Max” costante)
- Risultato: livello di linea **compatibile** con attenuatore + ES8388, con buon headroom.

**Opzione B: Output RCA “caldo” + attenuazione maggiore**
- Se utilizzi spesso la SC LIVE 4 vicina al massimo, puoi aumentare l’attenuazione (es. R1=15kΩ, R2=4.7kΩ).
- Il gain ADC ES8388 resta vicino a 0 dB, sfruttando la dinamica del codec.

### 3. Verifica Configurazione Audio

Via app Flutter o API REST:

```
GET /api/audio/input-gain
```

```json
{
  "success": true,
  "inputGainDb": -14.0,
  "sampleRate": 44100,
  "bitDepth": 24
}
```

### 4. Test Livello Audio

1. Play traccia test @ 0 dBFS da SC LIVE 4
2. Verificare nessun clip su DSP-Control (LED/indicatore)
3. Regolare gain se necessario:

```
POST /api/audio/input-gain
{"gain": -14.0}
```

## Gain Staging Ottimale (RCA → Attenuatore → ES8388)

Esempio con attenuatore **R1 = 10kΩ** serie e **R2 = 4.7kΩ** verso massa (per canale):

| Stadio | Livello | Note |
|--------|---------|------|
| SC LIVE 4 RCA Out | livello linea consumer/pro, regolato da master | Evitare clip su uscita console |
| Attenuatore passivo | ≈ ‑9.5 dB | Protegge l’ingresso e uniforma le sorgenti |
| DSP-Control ADC Input | vicino a 0 dBFS su picchi musicali | Headroom digitale mantenuto |
| ES8388 ADC Gain | 0 dB (range fine ±6 dB via API) | Regolazione di fino via `/api/audio/input-gain` |
| Wireless TX | 0 dBFS | Nessuna perdita |
| Modulo RX DAC Output | livello linea verso driver bilanciato/XLR | Vedi documentazione modulo RX |
| CQ260D Ampli Input | +4 dBu nominale | Headroom ~12 dB se gain ampli corretto |

## Audio-Reactive con SC LIVE 4

La SC LIVE 4 ha dynamic range 110 dB → perfetto per audio-reactive preciso!

### Bande Ottimizzate per Musica DJ

| Banda | Range | Uso Tipico | Sensibilità |
|-------|-------|------------|-------------|
| Sub Bass | 20–60 Hz | Kick drum fundamentals | 2.5× |
| Bass | 60–250 Hz | Bassline | 2.0× |
| Low Mids | 250–500 Hz | Vocals low | 1.5× |
| Mids | 500–2000 Hz | Vocals, snare | 1.0× (rif.) |
| High Mids | 2000–6000 Hz | Hi-hat, synth leads | 0.8× |
| Treble | 6000–20000 Hz | Crash, FX | 0.6× |

### Scene Consigliate

- **Kick Pulse**: Sub Bass → Fixture front (strobe on beat)
- **Bassline Glow**: Bass → Fixture floor (smooth fade)
- **Hi-Hat Flicker**: High Mids → Fixture ceiling (fast chase)
- **Full Spectrum Rainbow**: Tutte le bande → Colore progressivo

## Troubleshooting

### Audio Distorto

**Causa:** Input gain troppo alto  
**Soluzione:**
1. Ridurre gain SC LIVE 4 a -14 dB o inferiore
2. Oppure ridurre gain DSP-Control: `POST /api/audio/input-gain {"gain": -20.0}`

### Audio Troppo Basso

**Causa:** Input gain troppo basso  
**Soluzione:**
1. Aumentare gain SC LIVE 4 (max +18 dBu)
2. Oppure aumentare gain DSP-Control: `POST /api/audio/input-gain {"gain": -6.0}`

### Audio-Reactive Non Risponde

**Causa:** Sensibilità troppo bassa  
**Soluzione:** Aumentare sensibilità bande in app Audio-Reactive Config

### Latenza Audio >50ms

**Causa:** Buffer wireless troppo grande  
**Soluzione:** Verificare packet loss <1%, se OK ridurre buffer in `audio_rx.cpp`

## Performance Attese

| Metrica | Valore Target | Note |
|---------|---------------|------|
| **Latenza totale** | <30ms | SC LIVE 4 → Wireless → Cassa |
| **THD+N** | <0.01% | Trasparenza audio |
| **SNR** | >90 dB | Con gain -14 dB |
| **Packet Loss** | <0.5% | In condizioni normali |
| **Audio-Reactive Latency** | <50ms | Kick → Luci |

## Riferimenti

- [Denon DJ SC LIVE 4 Manual](https://www.denondj.com)
- [DSP-Control System Documentation](SYSTEM_DOCUMENTATION.md)
