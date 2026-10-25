#pragma once

#include "common.h"

namespace core {

struct context;

// Reconstructed from sub_10192640
// Loads and decrypts PE resources 0x65 and 0x66
// Resource 0x65: module/interface definition table
// Resource 0x66: hook function pointers
bool load_resources(context* ctx, HMODULE dll_module);

}
