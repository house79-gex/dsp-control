#pragma once
#include <Arduino.h>
#include <cstdint>
#include <cstddef>
#include "dsp_registers.h"

// ======= Protocollo RS-485 CQ260D (da reverse engineering KTV2i3oForm) =======
//
// Frame TX (Host → DSP):
//   [0] = 0xFF  (sync 1)
//   [1] = 0x55  (sync 2)
//   [2] = LEN   (lunghezza payload = buf[0] + 1 + 2)
//   [3] = GRP   (gruppo DSP)
//   [4] = ID_L  (DSP ID byte basso)
//   [5] = ID_H  (DSP ID byte alto)
//   [6..N] = DATA (payload comando)
//   [N+1] = CHK  (checksum: somma di tutti i byte & 0xFF)
//
// Frame RX (DSP → Host) state machine:
//   State 0: attesa 0xFF → State 1
//   State 1: attesa 0x55 → State 2
//   State 2: leggi LEN → State 3
//   State 3: accumula byte, quando cnt >= msglen+1 → processa

// Comandi risposta DSP
#define DSP_CMD_CONNECTED       0x30    // Connessione OK
#define DSP_CMD_DOWNLOAD_OK     0x31    // Download completato
#define DSP_CMD_UPLOAD_DATA     0x32    // Upload dati (blocchi 50 byte)
#define DSP_CMD_DEVICE_INFO     0x60    // Informazioni dispositivo
#define DSP_CMD_LEVEL_DISPLAY   0x80    // Dati VU meter real-time

// Comandi TX verso DSP
#define DSP_TXCMD_CONNECT       0x30    // Richiesta connessione
#define DSP_TXCMD_DOWNLOAD      0x31    // Download parametri al DSP
#define DSP_TXCMD_UPLOAD        0x32    // Upload parametri dal DSP
#define DSP_TXCMD_PARAM_SET     0x40    // Imposta singolo parametro
#define DSP_TXCMD_LEVEL_REQ     0x80    // Richiesta livelli VU meter

// Identificatore dispositivo DSP
struct DspId {
    uint8_t  grp;    // Gruppo DSP
    uint16_t dspID;  // ID dispositivo (16-bit LE)
};

// Stato connessione DSP
struct DspStatus {
    bool connected;             // Connessione stabilita
    bool downloadOk;            // Download completato
    uint8_t deviceGroup;        // Numero gruppi dispositivo
    uint16_t detectedDspId;     // ID DSP rilevato
    int8_t heartbeatCounter;    // Contatore heartbeat (100, decremento periodico)
    uint32_t lastActivity;      // Timestamp ultima attività (ms)
};

// Dati VU meter real-time
struct DspLevelData {
    int16_t inputLevels[4];     // Livelli ingressi (dB * 10)
    int16_t outputLevels[4];    // Livelli uscite
};

// ——— API pubblica ———

// Inizializza il modulo protocollo DSP
void dsp_protocol_init();

// Invia un comando al DSP (costruisce il frame completo con header+checksum)
// buf: payload comando (senza header 0xFF 0x55)
// len: lunghezza payload
// id:  identificatore dispositivo target
void dsp_command_send(const uint8_t* buf, size_t len, DspId id);

// Processa un byte ricevuto dalla UART RS-485 (state machine RX)
void dsp_rx_process_byte(uint8_t rx);

// Processa il buffer di comando accumulato (chiamata interna dopo ricezione frame completo)
void dsp_uctrl_cmd_processor();

// Esegue la sequenza di connessione completa al DSP
// Restituisce true se la connessione è avvenuta entro DSP_CONNECT_TIMEOUT_MS
bool dsp_connect(DspId id);

// Heartbeat periodico (chiamare ogni ~10ms dal loop o da un task)
void dsp_heartbeat_tick();

// Richiede l'upload dei parametri correnti dal DSP → SYSREG locale
void dsp_upload_parameters(DspId id);

// Scarica (download) i parametri dal SYSREG locale → DSP
void dsp_download_parameters(DspId id);

// Restituisce il puntatore alla struttura SYSREG locale
SysReg* dsp_get_sysreg();

// Restituisce lo stato di connessione corrente
DspStatus* dsp_get_status();

// Restituisce i dati VU meter più recenti
DspLevelData* dsp_get_level_data();

// Calcola il checksum del protocollo: somma di tutti i byte & 0xFF
uint8_t dsp_calc_checksum(const uint8_t* data, uint8_t length);
