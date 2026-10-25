#include "sdk/offsets.h"
#include "sdk/sig_scanner.h"

namespace sdk {

modules g_modules = {};
offsets g_offsets = {};

bool modules::resolve() {
    client              = GetModuleHandleA("client.dll");
    engine              = GetModuleHandleA("engine.dll");
    vstdlib             = GetModuleHandleA("vstdlib.dll");
    vgui2               = GetModuleHandleA("vgui2.dll");
    vguimatsurface      = GetModuleHandleA("vguimatsurface.dll");
    materialsystem      = GetModuleHandleA("materialsystem.dll");
    studiorender        = GetModuleHandleA("studiorender.dll");
    shaderapidx9        = GetModuleHandleA("shaderapidx9.dll");
    panorama            = GetModuleHandleA("panorama.dll");
    panoramauiclient    = GetModuleHandleA("panoramauiclient.dll");
    inputsystem         = GetModuleHandleA("inputsystem.dll");
    filesystem_stdio    = GetModuleHandleA("filesystem_stdio.dll");
    server              = GetModuleHandleA("server.dll");
    matchmaking         = GetModuleHandleA("matchmaking.dll");
    vphysics            = GetModuleHandleA("vphysics.dll");
    gameoverlayrenderer = GetModuleHandleA("gameoverlayrenderer.dll");
    soundsystem         = GetModuleHandleA("soundsystem.dll");
    datacache           = GetModuleHandleA("datacache.dll");
    localize            = GetModuleHandleA("localize.dll");

    return client && engine;
}

bool offsets::resolve(const modules& m) {
    // Find game window
    game_window = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");

    // shaderapidx9.dll offsets
    if (m.shaderapidx9)
        d3d_device = reinterpret_cast<u8*>(m.shaderapidx9) + 0x29830;

    // engine.dll offsets
    if (m.engine) {
        auto eng = reinterpret_cast<u8*>(m.engine);
        engine_client = eng + 0xAF708;
        client_state  = eng + 0xAF408;
        model_render  = eng + 0x2DDD4;
    }

    // studiorender.dll offsets
    if (m.studiorender) {
        auto sr = reinterpret_cast<u8*>(m.studiorender);
        studio_render = sr + 0x15F7C;
    }

    // client.dll offsets
    if (m.client) {
        auto cl = reinterpret_cast<u8*>(m.client);
        global_vars   = cl + 0x10F978;
        glow_manager  = cl + 0x442881;
        client_mode   = cl + 0x76660;
        input         = cl + 0x7A760;
        move_helper   = cl + 0x14CA70;
        prediction    = cl + 0x110E68;
        weapon_system = cl + 0x1C3C20;
    }

    // Signature-based offsets
    if (m.gameoverlayrenderer) {
        u8* result = find_pattern(m.gameoverlayrenderer,
            "3D ? ? ? ? 73 1A 68 ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? 83 C4 04 8B 01 6A 00 FF 50 14 3B 7B 3C");
        // Store the result

        result = find_pattern(m.gameoverlayrenderer,
            "89 1D ? ? ? ? F3 0F 10 83");
        if (result)
            ; // *(u32**)(result + 2)
    }

    // panorama.dll offsets
    if (m.panorama) {
        auto pan = reinterpret_cast<u8*>(m.panorama);
        panorama_engine = pan + 0x27888;
    }

    return true;
}

}
