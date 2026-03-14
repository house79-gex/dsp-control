# Display Setup – UEDX80480050E-WB-A 5"

## Specifiche Hardware

| Parametro | Valore |
|-----------|--------|
| Diagonale | 5.0" |
| Risoluzione | 800 × 480 px |
| Interfaccia | RGB 40-pin (24-bit) |
| Touch | GT911 capacitivo, 5 punti |
| Touch interfaccia | I2C (400kHz) |
| Backlight | LED, controllo PWM |
| Alimentazione | 3.3V |
| Pixel clock | 20MHz (tipico) |

Datasheet: **`docs/UEDX80480050E-WB-A-V3.3-SPEC.pdf`** (V3.3, 2025-01-08).  
Sintesi pin J2 + alimentazione: **[`DATASHEETS_REFERENCE.md`](DATASHEETS_REFERENCE.md)**.

| Alimentazione (SPEC) | Valore |
|----------------------|--------|
| Tensione | DC **4.0–5.5 V** (tip. **5 V**) |
| Corrente | ~**300 mA** max backlight; ~150 mA backlight off |
| Consigliato | **5 V ≥ 1 A** |

---

## Pinout GPIO (allineato SPEC J2 + `config.h`)

### Canale Rosso (R0-R4)
| Segnale | GPIO |
|---------|------|
| LCD_R0  | 45   |
| LCD_R1  | 48   |
| LCD_R2  | 47   |
| LCD_R3  | 21   |
| LCD_R4  | 14   |

### Canale Verde (G0-G5)
| Segnale | GPIO |
|---------|------|
| LCD_G0  | 5    |
| LCD_G1  | 6    |
| LCD_G2  | 7    |
| LCD_G3  | 15   |
| LCD_G4  | 16   |
| LCD_G5  | 4    |

### Canale Blu (B0-B4)
| Segnale | GPIO |
|---------|------|
| LCD_B0  | 8    |
| LCD_B1  | 3    |
| LCD_B2  | 46   |
| LCD_B3  | 9    |
| LCD_B4  | 1    |

### Segnali di Controllo
| Segnale | GPIO | Descrizione |
|---------|------|------------|
| LCD_PCLK  | 42 | Pixel clock (fronte di discesa) |
| LCD_HSYNC | 39 | Horizontal sync (active LOW) |
| LCD_VSYNC | 41 | Vertical sync (active LOW) |
| LCD_DE    | 40 | Data enable |
| LCD_BL_EN |  2 | Backlight enable (PWM) |

### Touch GT911 (I2C) – da SPEC TP
| Segnale | GPIO | Descrizione |
|---------|------|------------|
| TOUCH_SCL | 20 | I2C clock (TP pin 1) |
| TOUCH_SDA | 19 | I2C data (TP pin 2) |
| TOUCH_INT | 18 | SPEC: non usato su TP pin 3 |
| **TOUCH_RST** | **38** | **TP pin 6 – reset GT911** (stesso GPIO usato come **I2S_DOUT** nel firmware → **conflitto hardware** sul PCB UEDX; usare TCA9535 per RST o spostare I2S_DOUT) |

**Indirizzo I2C**: `0x5D` (se INT=LOW durante reset) oppure `0x14` (se INT=HIGH)

### I2S verso codec (SPEC: RTP)
| GPIO | Nome SPEC | Uso firmware |
|------|-----------|--------------|
| 11 | RTP-DIN | I2S DIN ← ADC codec |
| 12 | RTP-CLK | BCLK |
| 13 | RTP-DOUT | WS / verso codec (allineato `I2S_WS`) |
| 38 | CTP-rst **e** I2S_DOUT in firmware | **Verificare PCB** |

---

## Configurazione Timing RGB

```cpp
// Horizontal timing
hsync_polarity    = 0;   // Active LOW
hsync_front_porch = 8;   // pixel
hsync_pulse_width = 4;   // pixel
hsync_back_porch  = 8;   // pixel

// Vertical timing
vsync_polarity    = 0;   // Active LOW
vsync_front_porch = 8;   // linee
vsync_pulse_width = 4;   // linee
vsync_back_porch  = 8;   // linee

// Clock
freq_write      = 20000000;  // 20MHz
pclk_active_neg = true;      // Clock sul fronte di discesa
```

---

## Calibrazione Touch GT911

1. Assicurarsi che TOUCH_RST sia HIGH prima dell'init
2. Abbassare TOUCH_INT a LOW prima del reset per selezionare addr 0x5D
3. Attendere ≥10ms dal reset prima di comunicare via I2C
4. Il registro di configurazione GT911 (0x8047) deve corrispondere alla risoluzione del pannello (800×480)

```cpp
// LovyanGFX gestisce automaticamente l'init GT911
// Per calibrazione manuale nel codice utente:
auto cfg = _touch_instance.config();
cfg.x_min = 0; cfg.x_max = 799;
cfg.y_min = 0; cfg.y_max = 479;
```

---

## Troubleshooting

| Sintomo | Possibile Causa | Soluzione |
|---------|----------------|-----------|
| Display nero | Backlight spento | Verificare GPIO2 HIGH |
| Display nero | Clock troppo alto | Ridurre freq_write a 15MHz |
| Colori sbagliati | Byte order R/B invertiti | Swap canali R e B nel config |
| Touch non risponde | Indirizzo I2C sbagliato | Provare 0x14 invece di 0x5D |
| Touch spostato | Calibrazione ersa | Verificare x_min/x_max/y_min/y_max |
| Flickering | PSRAM contesa | Allocare FB in PSRAM con ps_malloc |
