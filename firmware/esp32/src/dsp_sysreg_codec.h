#pragma once
#include <cstdint>
#include <cstddef>
#include "dsp_registers.h"

// ======= Serializzazione/Deserializzazione SysReg (little-endian) =======
//
// L'ESP32 è nativo little-endian e SysReg usa #pragma pack(1), quindi
// la serializzazione è un semplice memcpy byte-per-byte.
// Le funzioni espongono un'API pulita per eventuali porting futuri.

// Ritorna la dimensione prevista del buffer serializzato
static inline size_t sysreg_serialized_size() {
    return sizeof(SysReg);
}

// Serializza SysReg in un buffer piatto little-endian
// Parametri:
//   src     : puntatore alla struct SysReg sorgente
//   dst     : buffer destinazione (almeno sysreg_serialized_size() byte)
//   max_len : dimensione disponibile del buffer dst
// Ritorna: numero di byte scritti, oppure 0 se dst è nullptr o max_len insufficiente
size_t sysreg_serialize(const SysReg* src, uint8_t* dst, size_t max_len);

// Deserializza un buffer piatto little-endian in SysReg
// Parametri:
//   dst : puntatore alla struct SysReg destinazione
//   src : buffer sorgente (almeno sysreg_serialized_size() byte)
//   len : lunghezza disponibile del buffer src
// Ritorna: true se la deserializzazione è completata, false se src è nullptr o len insufficiente
bool sysreg_deserialize(SysReg* dst, const uint8_t* src, size_t len);
