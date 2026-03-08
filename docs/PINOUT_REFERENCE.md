# Pinout Reference – Sistema Audio PA Dual-ESP32

Tabelle di riferimento complete per entrambi gli ESP32.

---

## ESP32 #1 (Master) – UEDX80480050E-WB-A

| GPIO | Segnale | Tipo | Periferica | Note |
|------|---------|------|------------|------|
| GPIO1  | LCD_B4    | OUT | Display RGB | Blu bit 4 |
| GPIO2  | LCD_BL_EN | OUT | Display RGB | Backlight PWM |
| GPIO3  | LCD_B1    | OUT | Display RGB | Blu bit 1 |
| GPIO4  | LCD_G5    | OUT | Display RGB | Verde bit 5 |
| GPIO5  | LCD_G0    | OUT | Display RGB | Verde bit 0 |
| GPIO6  | LCD_G1    | OUT | Display RGB | Verde bit 1 |
| GPIO7  | LCD_G2    | OUT | Display RGB | Verde bit 2 |
| GPIO8  | LCD_B0    | OUT | Display RGB | Blu bit 0 |
| GPIO9  | LCD_B3    | OUT | Display RGB | Blu bit 3 |
| GPIO10 | RS485_DE_RE | OUT | RS-485 | HIGH=TX, LOW=RX |
| GPIO11 | I2S_DIN   | IN  | I2S Master | Da ES8388 DOUT |
| GPIO12 | I2S_BCLK  | OUT | I2S Master | Clock → ES8388 + ESP32 #2 |
| GPIO13 | I2S_WS    | OUT | I2S Master | Word Select → ES8388 + ESP32 #2 |
| GPIO14 | LCD_R4    | OUT | Display RGB | Rosso bit 4 |
| GPIO15 | LCD_G3    | OUT | Display RGB | Verde bit 3 |
| GPIO16 | LCD_G4    | OUT | Display RGB | Verde bit 4 |
| GPIO17 | IPC_UART_TX | OUT | UART IPC | → ESP32 #2 GPIO2 (RX) |
| GPIO18 | IPC_UART_RX | IN  | UART IPC | ← ESP32 #2 GPIO1 (TX) |
| GPIO19 | TOUCH_SDA | IN/OUT | I2C GT911 | Touch data |
| GPIO20 | TOUCH_SCL | OUT | I2C GT911 | Touch clock |
| GPIO21 | LCD_R3    | OUT | Display RGB | Rosso bit 3 |
| GPIO38 | I2S_DOUT  | OUT | I2S Master | → ES8388 DIN (DAC) |
| GPIO39 | LCD_HSYNC | OUT | Display RGB | H-Sync |
| GPIO40 | LCD_DE    | OUT | Display RGB | Data Enable |
| GPIO41 | LCD_VSYNC | OUT | Display RGB | V-Sync |
| GPIO42 | LCD_PCLK  | OUT | Display RGB | Pixel Clock |
| GPIO43 | RS485_TX  | OUT | UART2 RS-485 | — |
| GPIO44 | RS485_RX  | IN  | UART2 RS-485 | — |
| GPIO45 | LCD_R0    | OUT | Display RGB | Rosso bit 0 |
| GPIO46 | LCD_B2    | OUT | Display RGB | Blu bit 2 |
| GPIO47 | LCD_R2    | OUT | Display RGB | Rosso bit 2 |
| GPIO48 | LCD_R1    | OUT | Display RGB | Rosso bit 1 |
| I2C (GPIO19/20) | TCA9535 | — | Expander 0x20 | Relay + LED + TOUCH_RST |

**GPIO liberi**: nessuno (tutti occupati in architettura v2.0)

---

## ESP32 #2 (Slave) – WROOM1-N16R8

| GPIO | Segnale | Tipo | Periferica | Note |
|------|---------|------|------------|------|
| GPIO1  | IPC_UART_TX | OUT | UART IPC | → Master GPIO18 (RX) |
| GPIO2  | IPC_UART_RX | IN  | UART IPC | ← Master GPIO17 (TX) |
| GPIO3  | — | — | Libero | Disponibile |
| GPIO4  | — | — | Libero | Disponibile |
| GPIO5  | I2S_BCLK_IN | IN  | I2S Slave | ← Master GPIO12 (BCLK) |
| GPIO6  | I2S_WS_IN   | IN  | I2S Slave | ← Master GPIO13 (WS) |
| GPIO7  | I2S_DIN     | IN  | I2S Slave | ← ES8388 DOUT (fan-out) |
| GPIO8  | DMX_TX      | OUT | UART2 DMX | Linea DMX512 |
| GPIO9  | DMX_DE      | OUT | DMX DE    | Sempre HIGH (trasmissione) |
| GPIO10 | RELAY_GPIO  | OUT | Relay     | Relay generale |
| GPIO11 | STROBE_GPIO | OUT | Strobo    | Uscita strobo |
| GPIO12..48 | — | — | Liberi | >10 GPIO disponibili per espansioni |
| IPEX   | WiFi Antenna | — | ESP-NOW TX | Antenna 2.4GHz 3dBi esterna |

**GPIO liberi**: GPIO3, GPIO4, GPIO12-GPIO48 (tranne occupati) – ampio margine

---

## Connessioni Inter-ESP32

| Segnale | ESP32 #1 (Master) | ESP32 #2 (Slave) | Protocollo |
|---------|-------------------|------------------|------------|
| I2S BCLK | GPIO12 (OUT) | GPIO5 (IN) | I2S clock |
| I2S WS   | GPIO13 (OUT) | GPIO6 (IN) | I2S word select |
| I2S DIN  | GPIO11 (IN, condiviso) | GPIO7 (IN, condiviso) | I2S data |
| IPC TX→RX | GPIO17 (TX) | GPIO2 (RX) | UART 115200 |
| IPC RX←TX | GPIO18 (RX) | GPIO1 (TX) | UART 115200 |
| GND | GND | GND | Massa comune (obbligatorio) |

---

## Connessioni Esterne

### ESP32 #1 (Master) → DSP CQ260D
| Master GPIO | Segnale | CQ260D Pin |
|-------------|---------|------------|
| GPIO43 | RS485_TX (via MAX485) | RS485 A/B |
| GPIO44 | RS485_RX (via MAX485) | RS485 A/B |
| GPIO10 | RS485_DE_RE | MAX485 DE+/RE- |

### ESP32 #2 (Slave) → Fixture DMX512
| Slave GPIO | Segnale | Driver |
|------------|---------|--------|
| GPIO8 | DMX_TX | MAX485 DI |
| GPIO9 | DMX_DE | MAX485 DE+/RE- (collegati) |
| — | XLR 3-pin | MAX485 A→pin3, B→pin2 |

---

## Compatibilità Livelli Logici

| Connessione | Tensione | Compatibile |
|------------|----------|-------------|
| ESP32 #1 ↔ ESP32 #2 (I2S, UART) | 3.3V ↔ 3.3V | ✅ Diretta |
| ESP32 #1 ↔ ES8388 (I2S) | 3.3V ↔ 3.3V | ✅ Diretta |
| ESP32 #2 ↔ MAX485 DMX | 3.3V ↔ 5V | ⚠️ MAX485 3.3V-compat? Usare SN65HVD485 (3.3V) |
| ESP32 #1 ↔ TCA9535 (I2C) | 3.3V ↔ 3.3V | ✅ Diretta |
