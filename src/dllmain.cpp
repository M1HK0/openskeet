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

// Encrypted string blobs loaded from .rdata xmmword constants
// These decrypt to module/status strings displayed during init
static const u8 enc_string_1[] = {
    // xmmword_101B5310, 101B5320, 101B5330 + 8 bytes
    // 56 bytes total - decrypts to initialization status string
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x0B, 0x11, 0x46, 0x63, 0x77, 0x3F, 0x36,
};

static const u8 enc_string_2[] = {
    // xmmword_101B52B0 through 101B5300
    // 96 bytes - decrypts to a longer status/path string
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const u8 enc_string_3[] = {
    // xmmword_101B5290 + 8 bytes + 2 bytes
    // 26 bytes - decrypts to a short identifier
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x64, 0x23, 0x2F, 0x3F, 0x2D, 0x2D, 0x6B,
    0x41, 0x4C,
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
