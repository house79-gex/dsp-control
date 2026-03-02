# Guida Setup Sistema Wireless 2 Canali

Procedura step-by-step per configurare e mettere in funzione il sistema wireless.

---

## Prerequisiti

- PlatformIO installato (VSCode + estensione, oppure CLI)
- 2× ESP32-S3 DevKitC-1 (moduli RX)
- Master ESP32-S3 già flashato con firmware principale
- Cavi USB-C per programmazione

---

## Step 1 – Flash Modulo RX #1 (CH1)

```bash
cd modulo_rx
# Assicurarsi che platformio.ini contenga: -DMY_CHANNEL_ID=1
pio run -e esp32s3 -t upload
```

## Step 2 – Leggere MAC Address Modulo RX #1

Aprire il monitor seriale dopo il flash:

```bash
pio device monitor
```

Annotare il MAC stampato all'avvio:
```
[RX] MAC: 24:0A:C4:12:34:56
```

## Step 3 – Flash Modulo RX #2 (CH2)

Modificare `platformio.ini`, cambiare `-DMY_CHANNEL_ID=1` in `-DMY_CHANNEL_ID=2`:

```bash
pio run -e esp32s3 -t upload
```

## Step 4 – Leggere MAC Address Modulo RX #2

```bash
pio device monitor
```
```
[RX] MAC: 24:0A:C4:78:9A:BC
```

## Step 5 – Configurare MAC nel Master

Con il master acceso e connesso all'AP WiFi (`SISTEMA_AUDIO_01`):

```bash
# Configura CH1
curl -X POST http://192.168.4.1/api/wireless/peer1 \
  -H "Content-Type: application/json" \
  -d '{"mac": [36, 10, 196, 18, 52, 86]}'

# Configura CH2
curl -X POST http://192.168.4.1/api/wireless/peer2 \
  -H "Content-Type: application/json" \
  -d '{"mac": [36, 10, 196, 120, 154, 188]}'
```

> **Nota:** I valori MAC sono in decimale nell'array JSON.  
> `0x24=36`, `0x0A=10`, `0xC4=196`, `0x12=18`, `0x34=52`, `0x56=86`

## Step 6 – Abilitare il Wireless

```bash
# Abilita wireless
curl -X POST http://192.168.4.1/api/wireless/enable \
  -H "Content-Type: application/json" \
  -d '{"enabled": true}'
```

## Step 7 – Verificare Stato

```bash
curl http://192.168.4.1/api/wireless/status
```

Risposta attesa (entrambi online):
```json
{
  "enabled": true,
  "mode": 0,
  "ch1": { "online": true, "loss_rate": 0.0, "packets_sent": 1234 },
  "ch2": { "online": true, "loss_rate": 0.0, "packets_sent": 1234 }
}
```

## Step 8 – Impostare Modalità Audio

```bash
# STEREO_SPLIT: CH1=L, CH2=R (default)
curl -X POST http://192.168.4.1/api/wireless/mode \
  -H "Content-Type: application/json" \
  -d '{"mode_name": "STEREO_SPLIT"}'

# oppure DUAL_MONO o MAIN_SUB
```

## Step 9 – Test Audio

1. Riproduci un sine sweep 20Hz–20kHz dal master
2. Verifica ascolto su entrambi i moduli RX
3. Latenza attesa: <15ms
4. Controllare sul Serial Monitor del RX:
   ```
   [STATS CH1] Ricevuti=5000 Persi=2 Loss=0.0% AudioUnderrun=0
   ```

## Step 10 – Test DMX

1. Attivare una scena DMX dal master: `POST /api/dmx/scene/play {"sceneId": 1}`
2. Verificare che i fixture connessi ai moduli RX rispondano
3. Controllare sincronizzazione tra i due canali

## Step 11 – Test RS-485

1. Inviare comando DSP: `POST /api/dsp/mute {"channelId": 1, "mute": true}`
2. Verificare risposta del CQ260D via RS-485 sul modulo RX
3. Smutare: `POST /api/dsp/mute {"channelId": 1, "mute": false}`

---

## Disabilitare il Wireless (Fallback Cablato)

In qualsiasi momento è possibile disabilitare il wireless per tornare alla modalità cablata:

```bash
curl -X POST http://192.168.4.1/api/wireless/enable \
  -H "Content-Type: application/json" \
  -d '{"enabled": false}'
```

La configurazione (MAC peer, modalità, stato enable/disable) viene salvata in NVS  
e ripristinata automaticamente al prossimo riavvio del master.

---

## Troubleshooting Rapido

| Sintomo | Soluzione |
|---------|-----------|
| `"online": false` dopo 3s | Verificare MAC e distanza |
| `loss_rate > 0.05` | Avvicinare i moduli o rimuovere ostacoli |
| Audio glitch | Aumentare `AUDIO_RING_FRAMES` in `modulo_rx/src/config.h` |
| DMX non risponde | Verificare pin `DMX_TX_PIN` e baud rate 250000 |
| RS-485 non funziona | Verificare `RS485_DE_PIN` e cablaggio MAX485 |
