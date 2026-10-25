#pragma once

#include "common.h"

namespace core {

// Reconstructed from sub_10194CF0 / sub_10192640
// Singleton context that holds the cheat's global state
struct context {
    void*   base;           // 0x00 - allocated memory base
    u32     payload_size;   // 0x04 - size of unpacked payload (0x2FC000)
    void*   current_ptr;    // 0x08 - current write position
    void*   resource_data;  // 0x0C - decrypted PE resource data
    void*   hook_table;     // 0x10 - pointer to hook function table
    u32     module_count;   // 0x14
    u32     reserved[5];    // 0x18-0x2B

    static context* instance();
    bool init(HMODULE dll_module);
};

bool init_context(HMODULE dll_module);
context* get_context();

}
