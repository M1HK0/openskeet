#include "sdk/interfaces.h"

namespace sdk {

game_interfaces g_interfaces = {};

using create_interface_fn = void* (*)(const char*, int*);

void* get_interface(const char* module_name, const char* interface_name) {
    HMODULE module = GetModuleHandleA(module_name);
    if (!module)
        return nullptr;

    auto create_interface = reinterpret_cast<create_interface_fn>(
        GetProcAddress(module, "CreateInterface"));
    if (!create_interface)
        return nullptr;

    int ret = 0;
    return create_interface(interface_name, &ret);
}

bool game_interfaces::resolve() {
    client           = get_interface("client.dll",            "VClient018");
    engine_client    = get_interface("engine.dll",            "VEngineClient014");
    surface          = get_interface("vguimatsurface.dll",    "VGUI_Surface031");
    engine_trace     = get_interface("engine.dll",            "EngineTraceClient004");
    entity_list      = get_interface("client.dll",            "VClientEntityList003");
    model_info       = get_interface("engine.dll",            "VModelInfoClient004");
    model_render     = get_interface("engine.dll",            "VEngineModel016");
    material_system  = get_interface("materialsystem.dll",    "VMaterialSystem080");
    game_movement    = get_interface("client.dll",            "GameMovement001");
    prediction       = get_interface("client.dll",            "VClientPrediction001");
    panel            = get_interface("vgui2.dll",             "VGUI_Panel009");
    render_view      = get_interface("engine.dll",            "VEngineRenderView005");
    engine_effects   = get_interface("engine.dll",            "VEngineEffects001");
    studio_render    = get_interface("studiorender.dll",      "VStudioRender026");
    cvar             = get_interface("vstdlib.dll",           "VEngineCvar007");
    input_system     = get_interface("inputsystem.dll",       "InputSystemVersion001");
    localize         = get_interface("localize.dll",          "Localize_001");

    return client && engine_client;
}

}
