#include "dsp_sysreg_codec.h"
#include <string.h>

size_t sysreg_serialize(const SysReg* src, uint8_t* dst, size_t max_len) {
    if (!src || !dst || max_len < sizeof(SysReg)) {
        return 0;
    }
    memcpy(dst, src, sizeof(SysReg));
    return sizeof(SysReg);
}

bool sysreg_deserialize(SysReg* dst, const uint8_t* src, size_t len) {
    if (!dst || !src || len < sizeof(SysReg)) {
        return false;
    }
    memcpy(dst, src, sizeof(SysReg));
    return true;
}
