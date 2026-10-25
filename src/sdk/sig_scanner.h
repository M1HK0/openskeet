#pragma once

#include "common.h"

namespace sdk {

// Reconstructed from sub_10191160
// IDA-style pattern scanning: "3D ? ? ? ? 73 1A 68"
// ? = wildcard byte
u8* find_pattern(HMODULE module, const char* pattern);

}
