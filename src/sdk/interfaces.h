#pragma once

#include "common.h"

namespace sdk {

void* get_interface(const char* module_name, const char* interface_name);

template<typename T>
T* get_interface(const char* module_name, const char* interface_name) {
    return static_cast<T*>(get_interface(module_name, interface_name));
}

// All Source Engine interfaces used by the cheat
// Resolved from strings found at 0x102140cf, 0x10233c27, etc.
struct game_interfaces {
    void* client;                // VClient018
    void* engine_client;         // VEngineClient014
    void* surface;               // VGUI_Surface031
    void* engine_trace;          // EngineTraceClient004
    void* entity_list;           // VClientEntityList003
    void* model_info;            // VModelInfoClient004
    void* model_render;          // VEngineModel016
    void* material_system;       // VMaterialSystem080
    void* engine_sound;          // IEngineSoundClient003
    void* game_movement;         // GameMovement001
    void* prediction;            // VClientPrediction001
    void* panel;                 // VGUI_Panel009
    void* render_view;           // VEngineRenderView005
    void* engine_effects;        // VEngineEffects001
    void* studio_render;         // VStudioRender026
    void* cvar;                  // VEngineCvar007
    void* input_system;          // InputSystemVersion001
    void* localize;              // Localize_001
    void* file_system;           // VFileSystem017

    bool resolve();
};

extern game_interfaces g_interfaces;

}
