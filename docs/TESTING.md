# Testing – Sistema Audio PA Dual-ESP32

Procedure di test per validare l'integrazione dual-ESP32.

---

## 1. Test Unitari (PlatformIO native)

```bash
# Esegui test unitari (native env, nessun hardware richiesto)
cd firmware/esp32
pio test -e native
```

I test sono in `tests/firmware/`. Coprono:
- Serializzazione/deserializzazione JSON (storage)
- Calcolo delay audio (`audio_ms_to_samples_dsp`)
- Protocollo RS-485 checksum CRC

---

## 2. Test IPC (UART Inter-Processor)

### 2.1 Test Ping/Pong

**Prerequisiti**: entrambi gli ESP32 flashati e collegati via UART IPC

```
[Master] Monitora seriale a 115200 baud:
  Atteso: "[STORAGE] ESP32 #2 disponibile via IPC – storage delegato"

[Slave] Monitora seriale a 115200 baud:
  Atteso: "[IPC_SLAVE] UART1 inizializzato @ 115200 baud"
```

**Risultato atteso**: Master riceve RESP_PONG entro 200ms

### 2.2 Test Storage Save/Load

1. Dal Master, salva un preset via REST API o UI
2. Monitor Slave: `[STORAGE_SLAVE] Salvato key='preset_0' (N bytes)`
3. Riavvia Master (reset hardware)
4. Monitor Master: `[STORAGE] IPC disponibile, caricamento preset da ESP32 #2`
5. Verificare che i dati siano identici

### 2.3 Test Scene Change DMX

1. Monitor Master: chiamare `ipc_scene_change(1)` via API
2. Monitor Slave: `[DMX_SLAVE] Scena 1 applicata`
3. Verificare canali DMX con tester o fixture

### 2.4 Test Latenza IPC

```
Latenza attesa CMD_PING → RESP_PONG: < 2ms
Latenza attesa CMD_STORAGE_LOAD → RESP_STORAGE_DATA: < 50ms
Timeout configurato: 100ms (3 retry)
```

---

## 3. Test I2S Sincronizzazione

### 3.1 Verifica via Log Seriale

```
[Master] Avvia PRIMA il Master (genera BCLK/WS)
[Slave] Avvia DOPO lo Slave (aggancia il clock)

Log atteso Slave:
  [AUDIO_SLAVE] I2S Slave RX configurato (BCLK=GPIO5, WS=GPIO6, DIN=GPIO7)
  [AUDIO_SLAVE] FFT bande: Bass=0.12, LowMid=0.05, Mid=0.08...
```

### 3.2 Verifica con Oscilloscopio (opzionale)

| Sonda | Punto | Segnale atteso |
|-------|-------|----------------|
| CH1 | GPIO12/GPIO5 | BCLK: 3MHz (48kHz × 64) |
| CH2 | GPIO13/GPIO6 | WS: 48kHz (alternato L/R) |
| CH3 | GPIO11/GPIO7 | DIN: dato I2S (1.5V pk-pk tipico) |

**Latenza I2S**: < 1ms (garantita da hardware DMA)

---

## 4. Test DMX512

### 4.1 Verifica Frame Rate

```
Log Slave ogni secondo:
  DMX FPS: atteso 40 (±2)
```

### 4.2 Verifica Timing DMX (oscilloscopio)

| Parametro | Standard DMX512 | Atteso |
|-----------|-----------------|--------|
| Break | 88µs min | 88µs |
| Mark After Break | 8µs min | 8µs |
| Baudrate | 250kbps | 250kbps |
| Stop bits | 2 | 2 (SERIAL_8N2) |

---

## 5. Test ESP-NOW Wireless

### 5.1 Verifica Trasmissione

```
Log Slave:
  [WIRELESS] ESP-NOW inizializzato, TX power 20 dBm, MAC: XX:XX:XX:XX:XX:XX
```

### 5.2 Test Range

1. Flash un ESP32 receiver (casse remote) con firmware ricevitore
2. Verificare ricezione a distanze progressive: 10m, 50m, 100m, 200m
3. Atteso con antenna 3dBi IPEX in linea di vista: > 150m

---

## 6. Test Integrazione Completa

### Sequenza di avvio corretta

```
1. Avvia ESP32 #2 (Slave) PRIMA
2. Attendi log "[SLAVE] Setup completato – loop IPC attivo"
3. Avvia ESP32 #1 (Master)
4. Attendi log "[STORAGE] ESP32 #2 disponibile via IPC"
5. Display Master: schermata principale LVGL
6. Verifica LED DMX reattivi all'audio (se fixture collegato)
```

### Checklist test completo

- [ ] IPC Ping/Pong < 2ms
- [ ] Storage save/load round-trip corretto
- [ ] Scene DMX cambiate via IPC
- [ ] I2S Slave FFT > 0 su segnale audio
- [ ] DMX FPS = 40 ±2
- [ ] ESP-NOW TX attivo (contatore incrementa)
- [ ] Display Master funzionante (LVGL touch)
- [ ] REST API Master risponde (`curl http://192.168.4.1/api/status`)
- [ ] Riavvio Master: preset ripristinati da Slave NVS

---

## 7. Fallback e Resilienza

### Slave non disponibile

Se ESP32 #2 non risponde al ping iniziale:
- Master usa NVS locale automaticamente
- Log: `[STORAGE] ESP32 #2 non risponde – fallback su NVS locale`
- Sistema funziona normalmente (senza DMX, senza wireless)

### I2S Slave senza clock

Se Master non è avviato o BCLK non arriva:
- `i2s_read()` ritorna entro 10ms (timeout `pdMS_TO_TICKS(10)`)
- `audio_slave_process_fft()` ritorna senza bloccare
- DMX continua a funzionare normalmente

### IPC timeout

Se un comando IPC non riceve ACK entro 100ms:
- 3 retry automatici (con delay 10ms tra tentativi)
- Se tutti falliscono: `s_ipcAvailable = false`, fallback NVS locale
