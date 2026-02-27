#include "dsp_protocol.h"
#include "rs485.h"
#include <Arduino.h>
#include <string.h>

// ======= Variabili di stato del modulo =======

// Buffer ricezione frame DSP
static uint8_t  s_rxBuffer[DSP_MAX_FRAME_SIZE];
static int      s_rxState     = 0;   // State machine ricezione: 0,1,2,3
static int      s_rxMsgLen    = 0;   // Lunghezza messaggio atteso
static int      s_rxCnt       = 0;   // Byte accumulati

// SYSREG – registro parametri DSP
static SysReg   s_sysReg;

// Stato connessione
static DspStatus s_status = {
    false, false, 0, 0, 100, 0
};

// Dati VU meter
static DspLevelData s_levelData;

// ======= Implementazione interna =======

uint8_t dsp_calc_checksum(const uint8_t* data, uint8_t length) {
    // Checksum: somma di tutti i byte & 0xFF
    int sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return (uint8_t)(sum & 0xFF);
}

void dsp_protocol_init() {
    memset(&s_sysReg,    0, sizeof(SysReg));
    memset(&s_levelData, 0, sizeof(DspLevelData));
    s_rxState = 0;
    s_rxCnt   = 0;
    Serial.println("[DSP_PROTO] Modulo protocollo CQ260D inizializzato");
}

void dsp_command_send(const uint8_t* buf, size_t len, DspId id) {
    if (len == 0 || len > (DSP_MAX_FRAME_SIZE - 7)) {
        Serial.println("[DSP_PROTO] Errore: lunghezza payload non valida");
        return;
    }

    // Costruisce il frame completo:
    // [0xFF][0x55][LEN][GRP][ID_L][ID_H][DATA...][CHK]
    uint8_t frame[DSP_MAX_FRAME_SIZE];
    frame[0] = 0xFF;                         // Sync 1
    frame[1] = 0x55;                         // Sync 2
    frame[2] = (uint8_t)(buf[0] + 1 + 2);   // LEN = payload[0] + 3
    frame[3] = id.grp;                       // Gruppo DSP
    frame[4] = (uint8_t)(id.dspID & 0xFF);  // ID byte basso (LE)
    frame[5] = (uint8_t)(id.dspID >> 8);    // ID byte alto

    // Copia payload del comando
    for (size_t i = 0; i < len; i++) {
        frame[6 + i] = buf[i];
    }

    // Calcola checksum su tutti i byte (inclusi header)
    uint8_t totalLen = (uint8_t)(6 + len);
    frame[totalLen] = dsp_calc_checksum(frame, totalLen);

    // Invia via RS-485
    rs485_send_raw(frame, totalLen + 1);

    // Delay tra invii come da specifiche protocollo
    delay(DSP_SEND_DELAY_MS);
}

void dsp_uctrl_cmd_processor() {
    // Struttura TuningCmdBuffer (già in s_rxBuffer):
    //   [0] = lunghezza
    //   [1] = sub-field
    //   [2] = dato basso (LE)
    //   [3] = dato alto
    //   [4] = Command ID (b3)
    //   [5] = Sub-command (b4)

    if (s_rxCnt < 5) {
        return; // Frame troppo corto
    }

    uint16_t dataWord = (uint16_t)((s_rxBuffer[3] << 8) | s_rxBuffer[2]);
    uint8_t  cmdId    = s_rxBuffer[4];
    uint8_t  subCmd   = (s_rxCnt > 5) ? s_rxBuffer[5] : 0;

    switch (cmdId) {
        case DSP_CMD_CONNECTED:
            // DSP ha risposto alla richiesta di connessione
            s_status.connected = true;
            s_status.lastActivity = millis();
            Serial.println("[DSP_PROTO] Connessione stabilita con DSP CQ260D");
            break;

        case DSP_CMD_DOWNLOAD_OK:
            // Download parametri completato
            s_status.downloadOk = true;
            Serial.println("[DSP_PROTO] Download parametri completato");
            break;

        case DSP_CMD_UPLOAD_DATA: {
            // Upload blocco dati (50 byte per volta) → popola SYSREG
            uint16_t offset = dataWord * DSP_UPLOAD_BLOCK_SIZE;
            uint8_t* sysRegPtr = (uint8_t*)&s_sysReg;
            size_t   sysRegSize = sizeof(SysReg);

            if (offset < sysRegSize) {
                size_t copyLen = s_rxBuffer[0] - 4; // Sottrai header interno
                if (offset + copyLen > sysRegSize) {
                    copyLen = sysRegSize - offset;
                }
                if (copyLen > 0 && s_rxCnt >= (int)(6 + copyLen)) {
                    memcpy(sysRegPtr + offset, &s_rxBuffer[6], copyLen);
                    Serial.printf("[DSP_PROTO] Upload blocco %d (%d byte)\n", dataWord, (int)copyLen);
                }
            }
            break;
        }

        case DSP_CMD_DEVICE_INFO:
            // Informazioni dispositivo
            s_status.detectedDspId  = dataWord;
            s_status.deviceGroup    = subCmd;
            Serial.printf("[DSP_PROTO] Device Info: ID=0x%04X gruppi=%d\n", dataWord, subCmd);
            break;

        case DSP_CMD_LEVEL_DISPLAY:
            // Dati VU meter (8 valori int16 = ingressi + uscite)
            for (int i = 0; i < 4 && (6 + i * 2 + 1) < s_rxCnt; i++) {
                s_levelData.inputLevels[i]  = (int16_t)((s_rxBuffer[6 + i * 2 + 1] << 8) |
                                                          s_rxBuffer[6 + i * 2]);
            }
            for (int i = 0; i < 4 && (14 + i * 2 + 1) < s_rxCnt; i++) {
                s_levelData.outputLevels[i] = (int16_t)((s_rxBuffer[14 + i * 2 + 1] << 8) |
                                                          s_rxBuffer[14 + i * 2]);
            }
            break;

        default:
            Serial.printf("[DSP_PROTO] Comando sconosciuto: 0x%02X\n", cmdId);
            break;
    }
}

void dsp_rx_process_byte(uint8_t rx) {
    // State machine ricezione frame DSP
    switch (s_rxState) {
        case 0:
            // Attesa sync byte 1 (0xFF)
            if (rx == 0xFF) {
                s_rxState = 1;
            }
            break;

        case 1:
            // Attesa sync byte 2 (0x55)
            if (rx == 0x55) {
                s_rxState = 2;
                s_rxCnt   = 0;
            } else {
                s_rxState = 0; // Reset se non è 0x55
            }
            break;

        case 2:
            // Leggi lunghezza messaggio
            s_rxMsgLen   = rx;
            s_rxBuffer[0] = rx;
            s_rxCnt       = 1;
            s_rxState     = 3;
            break;

        case 3:
            // Accumula byte payload
            if (s_rxCnt < DSP_MAX_FRAME_SIZE) {
                s_rxBuffer[s_rxCnt++] = rx;
            }
            // Quando abbiamo abbastanza byte, processa il frame
            if (s_rxCnt >= s_rxMsgLen + 1) {
                // Verifica checksum (ultimo byte del frame)
                uint8_t expectedChk = dsp_calc_checksum(s_rxBuffer, (uint8_t)(s_rxCnt - 1));
                uint8_t receivedChk = s_rxBuffer[s_rxCnt - 1];
                if (expectedChk == receivedChk) {
                    dsp_uctrl_cmd_processor();
                } else {
                    Serial.printf("[DSP_PROTO] Checksum errato: atteso=0x%02X ricevuto=0x%02X\n",
                                  expectedChk, receivedChk);
                }
                s_rxState = 0; // Reset state machine
                s_rxCnt   = 0;
            }
            break;

        default:
            s_rxState = 0;
            break;
    }
}

bool dsp_connect(DspId id) {
    // Sequenza connessione:
    // 1. Invia CommandSend([3, 0x30, 0, 0, 0, 0, 0, 0])
    // 2. Attendi DSP_CONNECT_TIMEOUT_MS
    // 3. Verifica Connected_OK
    Serial.println("[DSP_PROTO] Avvio sequenza connessione CQ260D...");

    s_status.connected = false;

    const uint8_t connectCmd[] = { 3, DSP_TXCMD_CONNECT, 0, 0, 0, 0, 0, 0 };
    dsp_command_send(connectCmd, sizeof(connectCmd), id);

    // Attendi risposta entro il timeout
    uint32_t start = millis();
    while ((millis() - start) < DSP_CONNECT_TIMEOUT_MS) {
        // Leggi byte disponibili dalla UART RS-485
        uint8_t rxBuf[64];
        size_t rxLen = 0;
        if (rs485_receive(rxBuf, sizeof(rxBuf), rxLen, 10)) {
            for (size_t i = 0; i < rxLen; i++) {
                dsp_rx_process_byte(rxBuf[i]);
                if (s_status.connected) {
                    Serial.println("[DSP_PROTO] Connessione riuscita");
                    return true;
                }
            }
        }
        delay(10);
    }

    Serial.println("[DSP_PROTO] Timeout connessione – nessuna risposta dal DSP");
    return false;
}

void dsp_heartbeat_tick() {
    // Heartbeat: decrementa il contatore ogni chiamata
    // Quando arriva a 0, invia pacchetto keepalive
    if (s_status.connected) {
        s_status.heartbeatCounter--;
        if (s_status.heartbeatCounter <= 0) {
            s_status.heartbeatCounter = 100;
            // Richiedi livelli VU meter come keepalive
            DspId broadcastId = { 0, 0xFFFF };
            const uint8_t levelReqCmd[] = { 1, DSP_TXCMD_LEVEL_REQ };
            dsp_command_send(levelReqCmd, sizeof(levelReqCmd), broadcastId);
        }
    }
}

void dsp_upload_parameters(DspId id) {
    // Richiede al DSP di inviare tutti i parametri (upload dal DSP al controller)
    Serial.println("[DSP_PROTO] Avvio upload parametri dal DSP...");
    const uint8_t uploadCmd[] = { 1, DSP_TXCMD_UPLOAD };
    dsp_command_send(uploadCmd, sizeof(uploadCmd), id);
}

void dsp_download_parameters(DspId id) {
    // Scarica i parametri del SYSREG locale al DSP in blocchi da DSP_UPLOAD_BLOCK_SIZE byte
    Serial.println("[DSP_PROTO] Avvio download parametri al DSP...");

    uint8_t* sysRegPtr  = (uint8_t*)&s_sysReg;
    size_t   sysRegSize = sizeof(SysReg);
    uint16_t blockNum   = 0;

    for (size_t offset = 0; offset < sysRegSize; offset += DSP_UPLOAD_BLOCK_SIZE) {
        size_t blockSize = sysRegSize - offset;
        if (blockSize > DSP_UPLOAD_BLOCK_SIZE) blockSize = DSP_UPLOAD_BLOCK_SIZE;

        // Costruisce payload: [len, CMD_DOWNLOAD, blockNum_L, blockNum_H, data...]
        uint8_t payload[DSP_UPLOAD_BLOCK_SIZE + 4];
        payload[0] = (uint8_t)(blockSize + 3);     // lunghezza
        payload[1] = DSP_TXCMD_DOWNLOAD;            // comando
        payload[2] = (uint8_t)(blockNum & 0xFF);    // numero blocco LE
        payload[3] = (uint8_t)(blockNum >> 8);
        memcpy(&payload[4], sysRegPtr + offset, blockSize);

        dsp_command_send(payload, blockSize + 4, id);
        blockNum++;
    }

    Serial.printf("[DSP_PROTO] Download completato: %d blocchi inviati\n", blockNum);
}

SysReg*       dsp_get_sysreg()     { return &s_sysReg;     }
DspStatus*    dsp_get_status()     { return &s_status;      }
DspLevelData* dsp_get_level_data() { return &s_levelData;   }
