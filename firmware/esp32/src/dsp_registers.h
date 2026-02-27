#pragma once
#include <cstdint>

// ======= SYSREG – Registro completo parametri DSP CQ260D =======
// Struttura estratta dal reverse engineering dell'applicazione Windows KTV2i3oForm
// Tutti i valori sono Little-Endian (LE)

struct SysReg {
    // ——— Gain ingressi ———
    uint16_t InGain[4];           // Gain canali ingresso (0=mute, 65535=max)

    // ——— Gain uscite ———
    uint16_t OutGain[4];          // Gain canali uscita

    // ——— EQ Parametrico ingressi (124 bande: 4 ingressi x 31 bande) ———
    uint16_t EQ_Freq_In[124];     // Frequenza EQ ingresso (Hz, LE)
    uint16_t EQ_Gain_In[124];     // Gain EQ ingresso (valore scalato ±24dB)
    uint8_t  EQ_Q_In[124];        // Fattore Q EQ ingresso
    uint8_t  EQ_Type_In[124];     // Tipo filtro: 0=Parametric, 1=LowShelf, 2=HighShelf, 3=LowPass, 4=HighPass, 5=Notch

    // ——— EQ Parametrico uscite (28 bande: 4 uscite x 7 bande) ———
    uint16_t EQ_Freq[28];         // Frequenza EQ uscita
    uint16_t EQ_Gain[28];         // Gain EQ uscita
    uint8_t  EQ_Q[28];            // Fattore Q EQ uscita
    uint8_t  EQ_Type[28];         // Tipo filtro EQ uscita
    uint8_t  EQ_Bypass[8];        // Bypass EQ: 0=attivo, 1=bypass (2 byte per canale)

    // ——— Filtri HPF/LPF ———
    uint16_t HLPF_Freq[8];        // Frequenza HPF/LPF (4 HPF + 4 LPF)
    uint8_t  HLPF_Type[8];        // Pendenza: 0=12dB/oct, 1=24dB/oct, 2=48dB/oct
    uint8_t  HLPF_Bypass[8];      // Bypass HPF/LPF

    // ——— Delay ———
    uint16_t Delay[4];            // Delay in campioni (1 campione = 1/48000 s ≈ 20.8 µs)

    // ——— Noise Gate ———
    uint8_t  Threhold_Noise[4];   // Soglia noise gate (0-255)
    uint8_t  ATack_Noise[4];      // Attack noise gate (0-255 ms)
    uint8_t  Decay_Noise[4];      // Decay noise gate (0-255 ms)

    // ——— Compressore ———
    uint8_t  Threhold[4];         // Soglia compressore (0-255)
    uint8_t  ATack[4];            // Attack compressore (0-255 ms)
    uint8_t  Decay[4];            // Decay/Release compressore (0-255 ms)
    uint8_t  CompRatio[4];        // Ratio compressore (1=1:1, 2=2:1, ... 20=inf:1)
    uint8_t  CompMakeup[4];       // Makeup gain compressore (0-48 dB)

    // ——— Mute e Fase ———
    uint8_t  Mute[4];             // Mute: 0=attivo, 1=muto
    uint8_t  Phase[4];            // Inversione fase: 0=normale, 1=invertito

    // ——— Matrice Mixer ———
    uint16_t MixVol[16];          // Volume matrice mixer (4 ingressi x 4 uscite)
    uint8_t  MixRoute[16];        // Routing matrice mixer

    // ——— Controllo Generale ———
    uint16_t masterVol;           // Volume master globale
    uint8_t  mode;                // Modalità operativa
    uint8_t  lock;                // Lock pannello (0=libero, 1=bloccato)
    uint8_t  password[6];         // Password (6 cifre ASCII)
    uint8_t  bootChar[7];         // Carattere avvio display
    uint8_t  ver;                 // Versione firmware DSP
};

// Dimensione massima blocco upload (50 byte per frame)
#define DSP_UPLOAD_BLOCK_SIZE   50
// Dimensione massima frame RS-485
#define DSP_MAX_FRAME_SIZE      601
// Delay tra invii (ms)
#define DSP_SEND_DELAY_MS       30
// Timeout risposta connessione (ms)
#define DSP_CONNECT_TIMEOUT_MS  300
// Baud rate RS-485 per DSP CQ260D
#define DSP_BAUD_RATE           115200
