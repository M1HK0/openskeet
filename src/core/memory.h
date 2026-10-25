#pragma once

#include "common.h"

namespace core {

// Reconstructed from sub_10192DC0
// Unpacks the main payload (0x2FC000 = 3,129,344 bytes)
bool unpack_payload(void* base, const void* packed_data, u32 size = 0x2FC000);

}
