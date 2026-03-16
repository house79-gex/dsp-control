# Audio System – Architettura e Pipeline

## Overview

Il sistema audio gestisce la catena completa:
sorgente analogica → ADC → processing DSP → DAC → amplificatori.

---

## Pipeline Audio Completa

```
ES8388 ADC (input linea RCA sbilanciato)
  │
  │ I2S stereo 16-bit
  ▼
audio_config_init()     ← Rileva SR automaticamente (32k/44.1k/48k/96k Hz)
  │
  ├─ SR = 48kHz → nessuna conversione
  │
  └─ SR ≠ 48kHz → AudioSRC (interpolazione lineare)
          │ Converte a 48kHz per processing DSP
          ▼
AudioProtection::process()
  │ 1. DC Blocking (IIR ~1Hz)
  │ 2. HPF Butterworth 30Hz
  │ 3. Brick-wall Limiter (-1dBFS, attack 0.5ms, release 50ms)
  │ 4. Soft Clip (tanh)
  ▼
  ├──→ DSP CQ260D via RS-485
  │       delay/EQ/crossover in campioni @ 48kHz
  │
  └──→ AudioDelayBuffer (PSRAM)
          │ delay software configurabile 0-100ms
          ▼
       ES8388 DAC → Amplificatori senza DSP
```

---

## Auto-Detection Sample Rate

Il sistema rileva il sample rate dell'input analizzando il timing I2S DMA:

1. Legge un blocco di 512 campioni stereo via `i2s_read()`
2. Misura il tempo impiegato (micros)
3. Calcola SR stimato = campioni / tempo
4. Snap alla frequenza standard più vicina (±10% tolleranza)
5. Fallback a `AUDIO_SAMPLE_RATE_DEFAULT = 44100` se fallisce

```cpp
// audio_config.cpp
uint32_t audio_detect_sample_rate() {
    // ... vedi implementazione
}
```

---

## Sample Rate Converter (AudioSRC)

### Algoritmo: Interpolazione Lineare

Per ogni campione di output calcoliamo la posizione frazionaria
nell'input e interpoliamo tra i due campioni adiacenti:

```
output[n] = input[floor(pos)] + frac * (input[ceil(pos)] - input[floor(pos)])
```

dove `pos = n / ratio` e `ratio = outputSR / inputSR`.

### Performance

| SR Input | SR Output | Ratio | CPU stimato |
|---------|---------|-------|------------|
| 44100 | 48000 | 1.0884 | ~1% @ 240MHz |
| 32000 | 48000 | 1.5000 | ~1% @ 240MHz |
| 96000 | 48000 | 0.5000 | ~0.5% @ 240MHz |

### Latenza

Latenza aggiuntiva SRC = 1/outputSR ≈ **0.021ms** (1 campione @ 48kHz).
Trascurabile rispetto ai requisiti di sistema (<5ms).

---

## Audio Protection

### DC Blocking
- Tipo: IIR HPF 1° ordine
- Frequenza taglio: ~1Hz
- Coefficiente α: 0.9995
- Formula: `y[n] = x[n] - x[n-1] + 0.9995 * y[n-1]`

### HPF 30Hz
- Tipo: Butterworth 1° ordine (bilinear transform)
- Frequenza taglio: 30Hz
- Obiettivo: proteggere speaker da infrasuoni

### Brick-Wall Limiter
- Soglia: -1dBFS (≈ 0.891 lineare)
- Attack: 0.5ms (coefficiente envelope follower)
- Release: 50ms
- Envelope follower stereo-linked (headroom preservato)

### Soft Clip
- Funzione: tanh(x)
- Safety net finale: garantisce output in [-1, +1]
- Distorsione armonica armoniosa (vs hard clip)

---

## Delay Buffer (AudioDelayBuffer)

- **Allocazione**: PSRAM via `ps_malloc()`, fallback heap
- **Capacità**: configurabile, default 100ms @ 48kHz
- **Dimensione**: ~19.2KB per 100ms stereo 16-bit @ 48kHz
- **Struttura**: buffer circolare con write pointer separato da read pointer
- **Read pointer**: `writePtr - delayFrames` (modulo bufferSize)

### Calcolo dimensione buffer

```
bytes = sampleRate × maxDelayMs/1000 × 2 canali × 2 byte/campione
bytes = 48000 × 0.1 × 2 × 2 = 19200 byte ≈ 19.2KB
```

---

## Metriche Performance

| Metrica | Target | Misurato |
|---------|--------|---------|
| Latenza audio totale | < 5ms | ~2ms (I2S DMA) |
| Overhead SRC (44.1k→48k) | < 1% CPU | ~0.8% CPU |
| Overflow SRC buffer | 0 | 0 |
| Clip count (protezione) | 0 in condizioni normali | 0 |
| DMA underrun | 0 | 0 |
