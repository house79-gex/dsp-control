# Context – Decisioni di Design

Questo documento registra le decisioni architetturali, i problemi risolti
e le motivazioni tecniche per future reference.

---

## Decisione: DSP Processing sempre a 48kHz

**Problema**: Il DSP CQ260D (hardware) lavora a 48kHz fisso per design interno.
Il campo `Delay[4]` in SYSREG è espresso in campioni @ 48kHz (da protocollo RS-485
reverse-engineered, `docs/PROTOCOL_RS485_CQ260D.md` linea 218).

**Errore precedente**: `ms_to_samples()` usava `delayMs * 48.0f` come shortcut
non documentato. Questo funzionava per caso quando l'input era già 44.1kHz
ma produceva un ritardo sbagliato (errore ~8.5% = 441/480).

**Decisione**: Creare `audio_ms_to_samples_dsp()` con commento esplicito
`AUDIO_PROCESSING_SAMPLE_RATE = 48000`. Tutte le conversioni per il DSP
devono passare per questa funzione. Il sample rate dell'input è irrilevante
per i calcoli DSP.

---

## Decisione: Separazione AUDIO_SAMPLE_RATE vs AUDIO_PROCESSING_SAMPLE_RATE

**Problema**: Una singola costante `AUDIO_SAMPLE_RATE = 44100` era usata sia
per l'input (corretto) sia per i calcoli DSP (sbagliato, dovrebbe essere 48000).

**Decisione**:
- `AUDIO_SAMPLE_RATE_DEFAULT = 44100`: SR default sorgente (SC LIVE 4)
- `AUDIO_PROCESSING_SAMPLE_RATE = 48000`: SR fisso per DSP hardware
- `AUDIO_SAMPLE_RATE = AUDIO_SAMPLE_RATE_DEFAULT`: alias per retrocompatibilità

---

## Decisione: Sample Rate Converter con interpolazione lineare

**Alternative considerate**:
1. Polyphase FIR (alta qualità, alta CPU, ~10ms latenza)
2. Interpolazione lineare (bassa qualità audio, bassa CPU, <0.1ms latenza)
3. Libreria esp-dsp SRC (non disponibile in esp-dsp v1.3)

**Decisione**: Interpolazione lineare per v2.1.
- Latenza critica: il sistema live non tollera latenze aggiuntive
- Qualità: l'ES8388 ha già un SRC hardware interno per SR vicini
- CPU: ESP32-S3 @ 240MHz è abbastanza per linear SRC stereo @ 48kHz

**Future consideration**: Sostituire con polyphase SRC per applicazioni
che richiedono qualità audiofila (v3.x).

---

## Decisione: Display pins dal datasheet (sostituzione TODO)

**Problema**: I pin display in `config.h` erano placeholder SPI con TODO.
Il display usa interfaccia RGB 40-pin, non SPI.

**Decisione**: Aggiornare completamente al datasheet UEDX80480050E-WB-A.
Conflitti di pin con RS485/DMX/RELAY notati nei commenti – da risolvere
a livello hardware nella revisione PCB v2.

**Pin conflicts identificati** (da risolvere nel PCB):
- GPIO21: LCD_R3 vs RS485_DE
- GPIO45: LCD_R0 vs RELAY_PIN
- GPIO48: LCD_R1 vs LED_STATUS
- GPIO2:  LCD_BL_EN vs DMX_DE
- GPIO1:  LCD_B4 vs DMX_TX

---

## Decisione: Audio Protection Pipeline

**Motivazione**: Output DAC diretto agli amplificatori senza limitazione
potrebbe causare danni agli speaker in caso di:
- Burst DC (offset ES8388 al boot)
- Infrasuoni (movimento membrane)
- Clipping digitale (overflow accumuli EQ/SRC)

**Ordine pipeline scelto**: DC Block → HPF 30Hz → Limiter → Soft Clip
- DC Block prima: rimuove offset prima che il limiter lo "veda"
- HPF prima del limiter: non sprecare headroom per infrasuoni
- Soft clip dopo il limiter: safety net finale

---

## Lezioni Apprese

1. **Costanti "ovvie" vanno documentate**: `48.0f` in `ms_to_samples()` non
   era ovviamente il SR del DSP hardware. Ora è `AUDIO_PROCESSING_SAMPLE_RATE`.

2. **Pin display richiedono datasheet**: I pin RGB sono completamente diversi
   dai pin SPI originariamente ipotizzati.

3. **SR input ≠ SR processing**: Sempre distinguere tra SR della sorgente
   (variabile) e SR del processore hardware (fisso). Non mescolare.
