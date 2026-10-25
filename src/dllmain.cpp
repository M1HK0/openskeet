#include "common.h"
#include "core/context.h"
#include "core/memory.h"
#include "core/imports.h"
#include "core/thread_hijack.h"
#include "core/string_encrypt.h"
#include "core/console.h"
#include "sdk/offsets.h"
#include "sdk/netvars.h"
#include "hooks/hooks.h"

// Encrypted string blobs from .rdata xmmword constants
// Decryption: dst[i] = src[i] ^ ((i % 52) + 51)
// String 1: "[t.me/g00d_crack] failed to allocate memory, reb..."
static const u8 enc_string_1[] = {
    0x68, 0x40, 0x1B, 0x5B, 0x52, 0x17, 0x5E, 0x0A,
    0x0B, 0x58, 0x62, 0x5D, 0x4D, 0x21, 0x22, 0x29,
    0x3A, 0x43, 0x75, 0x14, 0x0C, 0x0F, 0x53, 0x14,
    0x45, 0x1A, 0x0B, 0x09, 0x3E, 0x62, 0x14, 0x46,
    0x6E, 0x16, 0x08, 0x1B, 0x1A, 0x2B, 0x1C, 0x4E,
    0x1E, 0x46, 0x06, 0x1D, 0x16, 0x56, 0x14, 0x57,
    0x0C, 0x0B, 0x11, 0x46, 0x63, 0x77, 0x3F, 0x36,
};

// String 2: "[t.me/g00d_crack] cracked by qhide.\n$$$ Buy SUGAR ELITE for EFT $$$\nSubscribe to t.me/raze_club "
static const u8 enc_string_2[] = {
    0x68, 0x40, 0x1B, 0x5B, 0x52, 0x17, 0x5E, 0x0A,
    0x0B, 0x58, 0x62, 0x5D, 0x4D, 0x21, 0x22, 0x29,
    0x3A, 0x43, 0x75, 0x15, 0x1D, 0x12, 0x43, 0x17,
    0x47, 0x4A, 0x12, 0x0E, 0x3C, 0x6B, 0x1E, 0x45,
    0x68, 0x1C, 0x15, 0x60, 0x0A, 0x54, 0x54, 0x54,
    0x05, 0x48, 0x02, 0x5D, 0x05, 0x4B, 0x47, 0x44,
    0x42, 0x53, 0x05, 0x41, 0x4C, 0x47, 0x5B, 0x42,
    0x05, 0x44, 0x5C, 0x5F, 0x05, 0x47, 0x48, 0x5A,
    0x05, 0x54, 0x54, 0x54, 0x0A, 0x54, 0x02, 0x45,
    0x42, 0x5E, 0x01, 0x41, 0x50, 0x44, 0x47, 0x05,
    0x01, 0x5C, 0x05, 0x01, 0x6E, 0x02, 0x47, 0x58,
    0x08, 0x12, 0x3E, 0x62, 0x0E, 0x48, 0x03, 0x33,
};

// String 3: "[t.me/g00d_crack] fixed!\n "
static const u8 enc_string_3[] = {
    0x68, 0x40, 0x1B, 0x5B, 0x52, 0x17, 0x5E, 0x0A,
    0x0B, 0x58, 0x62, 0x5D, 0x4D, 0x21, 0x22, 0x29,
    0x3A, 0x43, 0x75, 0x07, 0x0E, 0x1F, 0x49, 0x4B,
    0x3B, 0x4C,
};

// Reconstructed from StartAddress (0x1018EC30)
static DWORD WINAPI cheat_main(HMODULE dll_module) {
    // Step 1: Initialize context singleton (sub_10194CF0)
    if (!core::init_context(dll_module))
        return 1;

    auto* ctx = core::get_context();

    // Step 2: Unpack payload if needed (sub_10192DC0)
    // Copies 0x2FC000 bytes of packed data
    // Returns 1 = already unpacked, 0 = needs further init
    // When unpack succeeds (returns 0), decrypt and display init strings
    {
        u8 decrypted[64] = {};
        core::decrypt_string(decrypted, enc_string_1, 56);
        core::console_print(std::cout, reinterpret_cast<const char*>(decrypted));
    }

    // Step 3: Wait for game to fully load
    while (GetModuleHandleA("serverbrowser.dll") == nullptr)
        Sleep(100);

    // Step 4: Decrypt and display loading status
    {
        u8 decrypted[128] = {};
        core::decrypt_string(decrypted, enc_string_2, 96);
        core::console_print(std::cout, reinterpret_cast<const char*>(decrypted));
    }

    // Step 5: Fix imports (sub_10192DE0)
    core::fix_imports();

    // Step 6: Decrypt final status string
    {
        u8 decrypted[32] = {};
        core::decrypt_string(decrypted, enc_string_3, 26);
        core::console_print(std::cout, reinterpret_cast<const char*>(decrypted));
    }

    // Step 7: Resolve netvars and offsets (sub_10192F70)
    sdk::g_modules.resolve();
    sdk::g_offsets.resolve(sdk::g_modules);
    sdk::netvar_manager::instance().init();

    // Step 8: Install hooks
    hooks::install_hooks();

    // Step 9: Thread hijack for anti-analysis (sub_101942A0)
    core::thread_hijack(ctx);

    return 0;
}

// Reconstructed from _DllMain@12 (0x1018F2F0)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        g_module = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(cheat_main),
            hinstDLL,
            0,
            nullptr
        );
    }
    return TRUE;
}
