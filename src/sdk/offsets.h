#pragma once

#include "common.h"

namespace sdk {

// Game module handles resolved at runtime
struct modules {
    HMODULE client;
    HMODULE engine;
    HMODULE vstdlib;
    HMODULE vgui2;
    HMODULE vguimatsurface;
    HMODULE materialsystem;
    HMODULE studiorender;
    HMODULE shaderapidx9;
    HMODULE panorama;
    HMODULE panoramauiclient;
    HMODULE inputsystem;
    HMODULE filesystem_stdio;
    HMODULE server;
    HMODULE matchmaking;
    HMODULE vphysics;
    HMODULE gameoverlayrenderer;
    HMODULE soundsystem;
    HMODULE datacache;
    HMODULE localize;

    bool resolve();
};

// Runtime-resolved offsets from game modules
// Reconstructed from sub_10192F70
struct offsets {
    HWND game_window;

    // shaderapidx9.dll
    u8* d3d_device;  // +0x29830

    // engine.dll
    u8* engine_client;
    u8* client_state;
    u8* model_render;
    u8* render_view;
    u8* engine_sound;

    // client.dll
    u8* client_mode;
    u8* global_vars;
    u8* glow_manager;
    u8* move_helper;
    u8* prediction;
    u8* game_movement;
    u8* view_render;
    u8* input;
    u8* weapon_system;

    // studiorender.dll
    u8* studio_render;

    // panorama.dll
    u8* panorama_engine;

    // engine.dll view matrix
    u8* view_matrix;

    bool resolve(const modules& mods);
};

extern modules g_modules;
extern offsets g_offsets;

}
