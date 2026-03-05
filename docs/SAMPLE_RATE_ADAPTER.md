# Sample Rate Adapter – Design e Test

## Problema

Il DSP CQ260D processa l'audio a **48kHz fisso** (da datasheet hardware).
La sorgente Denon SC LIVE 4 produce audio a **44.1kHz**.

Il campo `Delay[4]` nel SYSREG del DSP è espresso in **campioni @ 48kHz**:
- 1 campione @ 48kHz = 1/48000 s ≈ 20.83 µs
- 10ms = 480 campioni @ 48kHz
- 10ms ≠ 441 campioni @ 44.1kHz (SBAGLIATO per il DSP)

---

## Architettura SRC

```
Input: 44100 Hz                Output: 48000 Hz
  │                              │
  │  ratio = 48000/44100         │
  │        = 1.08843...          │
  │                              │
  ▼                              ▼
[n]  [n+1]  [n+2]  ...  →  [m]  [m+1]  [m+2]  ...
  \   /                    ↑
   \ /          interpolazione lineare
    X           pos_m = m / ratio
   / \
  /   \
prev  curr
```

---

## Algoritmo: Interpolazione Lineare

Per ogni frame di output `m`, calcoliamo la posizione frazionaria nell'input:

```
inPos  = phase / ratio        // posizione reale nell'input
inFrame = floor(inPos)         // frame input intero
frac    = inPos - inFrame      // parte frazionaria [0, 1)

output[m] = input[inFrame] + frac * (input[inFrame+1] - input[inFrame])

phase += 1.0  // avanza di 1 frame output
```

Dopo ogni blocco: `phase -= inputFrames * ratio` (mantieni la fase residua).

---

## Tabelle Conversione Delay

### DSP CQ260D @ 48kHz (CORRETTE)

| Delay (ms) | Campioni @ 48kHz | Note |
|-----------|-----------------|------|
| 0.021 ms  | 1               | 1 campione minimo |
| 1 ms      | 48              | |
| 5 ms      | 240             | ritardo speaker vicino |
| 10 ms     | 480             | ritardo ~3.4m distanza |
| 20 ms     | 960             | ritardo ~6.8m |
| 50 ms     | 2400            | ritardo ~17m |
| 100 ms    | 4800            | ritardo ~34m |
| 1000 ms   | 48000           | |
| 1365.3 ms | 65535           | **MASSIMO** (UINT16_MAX) |

### Confronto Errato vs Corretto

| Delay (ms) | Campioni @ 44.1kHz (SBAGLIATO) | Campioni @ 48kHz (CORRETTO) | Errore |
|-----------|-------------------------------|----------------------------|--------|
| 10 ms     | 441                           | 480                         | -8.5%  |
| 20 ms     | 882                           | 960                         | -8.5%  |
| 50 ms     | 2205                          | 2400                        | -8.5%  |

L'errore fisso del ~8.5% corrispondeva a circa **29cm di disallineamento**
nella venue map per ogni metro di distanza nominale.

---

## Test Results

### Test Conversione Delay

```
test_delay_conversion_10ms    [PASS]  10ms → 480 campioni
test_delay_conversion_20ms    [PASS]  20ms → 960 campioni
test_delay_conversion_1ms     [PASS]   1ms →  48 campioni
test_delay_conversion_zero    [PASS]   0ms →   0 campioni
test_delay_conversion_negative[PASS]  -5ms →   0 (saturazione)
test_delay_conversion_max     [PASS] 2000ms → 65535 (saturazione)
test_delay_dsp_always_48khz   [PASS]  10ms → 480 (non 441!)
```

### Test SRC 44.1kHz → 48kHz

```
test_src_init_44100_to_48000         [PASS]  ratio=1.0884
test_src_output_frame_count_100ms    [PASS]  4410→4796 frames (±50)
test_src_preserves_frequency         [PASS]  1kHz→1kHz (±50Hz)
test_src_48000_to_48000_passthrough  [PASS]  256→256 frames
test_src_reset                       [PASS]  stato azzerato
```

---

## Considerazioni Future

### SRC Polyphase FIR (v2.3)

Per applicazioni audiofila:
- Ordine filtro: 64-128 tap
- Oversampling: 4x
- Qualità: -140dB alias rejection
- CPU: ~5-8% @ 240MHz (stimato)
- Libreria candidata: `esp-dsp` con coefficienti custom

### MCLK-based SR Detection

Alternativa più accurata all'I2S timing:
- Misurare frequenza MCLK in ingresso (tipico 256×SR)
- 256 × 44100 = 11.2896 MHz
- 256 × 48000 = 12.288 MHz
- Usa PCNT (pulse counter) ESP32-S3 per misura precisa
