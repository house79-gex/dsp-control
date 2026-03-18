# Cartella `docs/hardware/` – schematici e datasheet

I PDF sotto elencati **non sono inclusi nel repository** (copyright/ dimensione). Vanno ottenuti dai fornitori o dalla documentazione ufficiale e, se necessario, conservati in locale (es. in `docs/` o in questa cartella).

| File (esterno) | Contenuto |
|-----------------|-----------|
| **UEDX80480050E-WB-A-V3.3-SPEC.pdf** | Display 5" + ESP32-S3 (UEDX) |
| **M144_sch_moduleaudio_v10.pdf** | Modulo M5 audio ES8388 |
| **ES8388.pdf** | Datasheet codec |
| **Relè Module interface.pdf** | Pin relè |
| **Encoder rotativo.pdf** | EC11 |
| **pololu-power-oring-ideal-diode-pair.pdf** | Pololu ORing #5398 (legacy: non usato se alimenti con PSU IRM dedicati) |
| **MAX485.pdf** | RS-485 |
| **Led Ring WS2811.pdf** | Anello LED |

## Alimentazione (revisione attuale)

Nella revisione attuale l’alimentazione usa PSU AC/DC dedicati:

- `Mean Well IRM-30-5ST` (Master, 5V 6A)
- `Mean Well IRM-05-5` (RX, 5V)
- `Mean Well IRM-20-15` (RX, 15V per DRV134)

Aggiungere i datasheet IRM in `docs/` quando li hai disponibili.

Sintesi cablaggio e pin ricavati da questi PDF: **`docs/DATASHEETS_REFERENCE.md`** e **`docs/CABLING_COMPLETE.md`**.
