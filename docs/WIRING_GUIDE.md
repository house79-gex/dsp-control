# Guida Cablaggio Dual-ESP32

Istruzioni dettagliate per il collegamento fisico tra ESP32 #1 (Master) ed ESP32 #2 (Slave).

---

## 1. Lista Materiali (BOM)

| Componente | Qt | Specifiche | Note |
|------------|----|------------|------|
| ESP32-S3 Display Module UEDX80480050E-WB-A | 1 | 5" 800×480 RGB, 16MB Flash, 8MB PSRAM | Board Master |
| ESP32-S3-WROOM1-N16R8 Dev Board | 1 | 16MB Flash, 8MB PSRAM, connettore IPEX | Board Slave |
| Antenna 2.4GHz 3dBi IPEX | 1 | Omnidirezionale, cavo 15cm | Range 150-300m |
| Cavo ribbon 7-pin 2.54mm | 1 | Lunghezza 30cm, connettori DuPont | I2S + UART + GND |
| Alimentatore 5V 2A | 1 | Barrel jack 5.5×2.1mm (o 2×1A separati) | Almeno 1.5A totali |
| GPIO I/O Expander TCA9535 | 1 | I2C, 16-bit, indirizzo 0x20 | Per relay/LED Master |
| Driver DMX MAX485 (o SN65HVD485) | 1 | Half-duplex RS-485 | Per DMX512 Slave |
| Filo 20AWG | 30cm | Per GND comune | Sezione adeguata |

---

## 2. Schema Cablaggio Completo

### 2.1 I2S Bus Condiviso (3 fili)

```
ES8388 BCLK ←──────────┬── ESP32 #1 GPIO12 (I2S_BCLK, Master out)
                        └── ESP32 #2 GPIO5  (I2S_BCLK_IN, Slave in)

ES8388 WS   ←──────────┬── ESP32 #1 GPIO13 (I2S_WS, Master out)
                        └── ESP32 #2 GPIO6  (I2S_WS_IN, Slave in)

ES8388 DOUT ────────────┬── ESP32 #1 GPIO11 (I2S_DIN, Master in)
                        └── ESP32 #2 GPIO7  (I2S_DIN, Slave in)
```

> **Nota cablaggio**:
> - Usare filo schermato se distanza > 10cm
> - Se distanza > 20cm, inserire buffer 74LVC1G125 su ES8388 DOUT (fan-out attivo)
> - BCLK e WS sono segnali di uscita dal Master → resistenza di terminazione non necessaria

### 2.2 UART IPC (2 fili)

```
ESP32 #1 GPIO17 (TX) ──────────→ ESP32 #2 GPIO2 (RX)
ESP32 #1 GPIO18 (RX) ←────────── ESP32 #2 GPIO1 (TX)
```

> **Importante**: TX e RX **devono essere incrociati** (TX → RX, non TX → TX)  
> Baudrate: 115200 bps, 8N1  
> Livelli logici: 3.3V (entrambi ESP32-S3 → compatibili diretti, no level shifter)

### 2.3 Alimentazione e GND

```
Alimentatore 5V 2A
    ├─ +5V ──┬─────────→ ESP32 #1 VIN (pin 5V)
    │        └─────────→ ESP32 #2 5V  (pin 5V)
    └─ GND ──┴─────────→ GND comune
                │
                ├─────────→ ESP32 #1 GND
                └─────────→ ESP32 #2 GND
```

> **GND comune è obbligatorio!** Senza di esso i livelli logici I2S e UART saranno errati.

### 2.4 Riepilogo Cablaggio (7 fili)

| # | Partenza | Arrivo | Colore consigliato | Segnale |
|---|----------|--------|--------------------|---------|
| 1 | ESP32 #1 GPIO12 | ESP32 #2 GPIO5 | Arancio | I2S BCLK |
| 2 | ESP32 #1 GPIO13 | ESP32 #2 GPIO6 | Giallo | I2S WS |
| 3 | ES8388 DOUT fan-out | ESP32 #2 GPIO7 | Verde | I2S DIN |
| 4 | ESP32 #1 GPIO17 | ESP32 #2 GPIO2 | Blu | IPC TX→RX |
| 5 | ESP32 #1 GPIO18 | ESP32 #2 GPIO1 | Viola | IPC RX←TX |
| 6 | GND comune | GND comune | Nero | Massa |
| 7 | +5V (opzionale) | ESP32 #2 5V | Rosso | Alimentazione |

---

## 3. Schema DMX512 (ESP32 #2 → Fixture)

```
ESP32 #2 GPIO8 (DMX_TX) ──→ MAX485 DI
ESP32 #2 GPIO9 (DMX_DE) ──→ MAX485 DE + /RE (collegati insieme)
                              MAX485 A ──→ XLR pin 3 (+)
                              MAX485 B ──→ XLR pin 2 (-)
                                       ──→ XLR pin 1 (GND schermo)
                              Terminazione: 120Ω tra A e B (fine linea)
```

---

## 4. Procedura Cablaggio Step-by-Step

1. **Preparare i cavi**: tagliare ribbon 7-pin da 30cm, crimpar connettori DuPont
2. **Collegare I2S** (3 fili): BCLK GPIO12→GPIO5, WS GPIO13→GPIO6, DIN fan-out→GPIO7
3. **Collegare UART IPC** (2 fili): TX GPIO17→GPIO2 (RX slave), RX GPIO18←GPIO1 (TX slave)
4. **Collegare GND comune**: filo 20AWG tra i due ESP32
5. **Montare antenna IPEX** su ESP32 #2 (avvitare delicatamente il connettore U.FL)
6. **Collegare alimentazione**: verificare tensioni con multimetro (5.0V ±0.1V)
7. **Test continuità**: verificare GND comune con multimetro (resistenza < 1Ω)

---

## 5. Checklist Pre-Accensione

- [ ] I2S BCLK GPIO12 → GPIO5 (non invertito)
- [ ] I2S WS GPIO13 → GPIO6 (non invertito)
- [ ] I2S DIN ES8388 DOUT fan-out → GPIO7 (non GPIO11!)
- [ ] UART TX/RX incrociati (TX Master→RX Slave, TX Slave→RX Master)
- [ ] GND comune verificato con multimetro (resistenza < 1Ω)
- [ ] Antenna IPEX montata saldamente (connettore U.FL inserito a fondo)
- [ ] Alimentatore 5V erogante almeno 1.5A totali
- [ ] Display FPC40 ben inserito nel connettore del Master
- [ ] TCA9535 I2C addr 0x20 correttamente configurato (A0=A1=A2=GND)

---

## 6. Test Funzionamento

### Test 1: Boot e IPC Ping
```
Monitor seriale ESP32 #1 (Master):
  [STORAGE] ESP32 #2 disponibile via IPC – storage delegato
  [IPC_MASTER] UART1 inizializzato @ 115200 baud (TX=GPIO17, RX=GPIO18)

Monitor seriale ESP32 #2 (Slave):
  [IPC_SLAVE] UART1 inizializzato @ 115200 baud (RX=GPIO2, TX=GPIO1)
```

### Test 2: I2S Sync
```
Monitor seriale ESP32 #2 (Slave):
  [AUDIO_SLAVE] I2S Slave RX configurato (BCLK=GPIO5, WS=GPIO6, DIN=GPIO7)

Se I2S non si sincronizza (es. Master non avviato), audio_slave_process_fft()
ritornerà immediatamente per timeout senza bloccare il sistema.
```

### Test 3: DMX Output
```
Monitor seriale ESP32 #2 (Slave):
  [DMX_SLAVE] Inizializzato (TX=GPIO8, DE=GPIO9, 512 canali @ 40 Hz)

Verificare con DMX tester o fixture che risponde ai comandi:
  ipc_scene_change(1);  // Sul Master: attiva scena 1 (rosso)
```

### Test 4: Storage Round-trip
```
Sul Master: salva preset via storage_save_dsp_presets(presets)
Sul Slave: log [STORAGE_SLAVE] Salvato key='preset_0' (...bytes)
Riavvio Master: verifica che i preset vengano ricaricati via IPC
```
