#pragma once
#include <cstdint>
#include <cassert>

// ======= SYSREG – Registro completo parametri DSP CQ260D =======
// Struttura estratta dal reverse engineering dell'applicazione Windows KTV2i3oForm
// Campi e ordine allineati alla struct C# SYSREG in reverse_data_parts/reverse_data_part001.md
// Tutti i valori sono Little-Endian (LE)
// Dimensione totale serializzata: 1092 byte (verificata con static_assert)

#pragma pack(push, 1)
struct SysReg {
    // ——— Gain ingressi ———                             offset    dim
    uint16_t InGain[4];           // Gain canali ingresso  0       8B

    // ——— Gain uscite ———
    uint16_t OutGain[4];          // Gain canali uscita    8       8B

    // ——— EQ Parametrico ingressi (124 bande: 4 ingressi x 31 bande) ———
    uint16_t EQ_Freq_In[124];     // Frequenza EQ ingresso  16     248B
    uint16_t EQ_Gain_In[124];     // Gain EQ ingresso       264    248B
    uint8_t  EQ_Q_In[124];        // Fattore Q EQ ingresso  512    124B
    uint8_t  EQ_Type_In[124];     // Tipo filtro ingresso   636    124B
    // 0=Parametric 1=LowShelf 2=HighShelf 3=LowPass 4=HighPass 5=Notch

    // ——— EQ Parametrico uscite (28 bande: 4 uscite x 7 bande) ———
    uint16_t EQ_Freq[28];         // Frequenza EQ uscita   760     56B
    uint16_t EQ_Gain[28];         // Gain EQ uscita        816     56B
    uint8_t  EQ_Q[28];            // Fattore Q EQ uscita   872     28B
    uint8_t  EQ_Type[28];         // Tipo filtro EQ uscita 900     28B
    uint8_t  EQ_Bypass[8];        // Bypass EQ uscite      928      8B

    // ——— Filtri HPF/LPF ———
    uint16_t HLPF_Freq[8];        // Frequenza HPF/LPF     936     16B
    uint8_t  HLPF_Type[8];        // Pendenza HPF/LPF      952      8B
    // 0=12dB/oct 1=24dB/oct 2=48dB/oct

    // ——— Filtri FIR ———
    uint16_t Fir_Freq[8];         // Frequenza FIR         960     16B
    uint8_t  Fir_Type[8];         // Tipo FIR              976      8B

    // ——— Delay ———
    uint16_t Delay[4];            // Delay in campioni     984      8B
    // 1 campione = 1/48000 s ≈ 20.8 µs

    // ——— Noise Gate ———
    uint8_t  Threhold_Noise[4];   // Soglia noise gate     992      4B
    uint8_t  ATack_Noise[4];      // Attack noise gate     996      4B
    uint8_t  Decay_Noise[4];      // Decay noise gate     1000      4B

    // ——— Compressore ———
    uint8_t  Threhold[4];         // Soglia compressore   1004      4B
    uint8_t  ATack[4];            // Attack compressore   1008      4B
    uint8_t  Decay[4];            // Release compressore  1012      4B

    // ——— Mute e Fase ———
    uint8_t  Mute[4];             // Mute: 0=attivo 1=muto  1016   4B
    uint8_t  Phase[4];            // Fase: 0=normale 1=inv  1020   4B

    // ——— Matrice Mixer ———
    uint16_t MixVol[16];          // Volume matrice mixer  1024    32B
    uint8_t  MixRoute[16];        // Routing matrice mixer 1056    16B

    // ——— Controllo Generale ———
    uint8_t  mode;                // Modalità operativa    1072     1B
    uint8_t  lock;                // Lock pannello         1073     1B
    uint8_t  password[6];         // Password (6 cifre)   1074     6B
    uint8_t  bootChar[7];         // Stringa avvio display 1080    7B
    uint16_t masterVol;           // Volume master globale 1087    2B
    uint8_t  AC_V_Sel;            // Selezione tensione AC 1089    1B
    uint8_t  Clip_TH;             // Soglia clip           1090    1B
    uint8_t  ver;                 // Versione firmware DSP 1091    1B
};
#pragma pack(pop)

// Verifica dimensione a compile-time (1092 byte come da struct C# di riferimento)
static_assert(sizeof(SysReg) == 1092, "SysReg deve essere 1092 byte – controllare padding/ordine campi");

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
