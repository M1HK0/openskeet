#include "sdk/engine.h"
#include "sdk/interfaces.h"
#include "sdk/offsets.h"
#include <cstring>
#include <cmath>

namespace sdk {

bool trace_ray(const features::vec3& start, const features::vec3& end, unsigned int mask, void* skip, trace_t& trace) {
    if (!g_interfaces.engine_trace)
        return false;

    ray_t ray;
    memset(&ray, 0, sizeof(ray));
    ray.start = start;
    ray.delta = end - start;
    ray.is_ray = true;
    ray.is_swept = true;

    // IEngineTrace::TraceRay is vtable[5]
    using fn = void(__thiscall*)(void*, const ray_t&, unsigned int, void*, trace_t&);
    // TODO: proper trace filter vtable
    (*reinterpret_cast<fn**>(g_interfaces.engine_trace))[5](g_interfaces.engine_trace, ray, mask, skip, trace);
    return true;
}

bool is_visible(const features::vec3& start, const features::vec3& end, void* skip) {
    trace_t tr;
    memset(&tr, 0, sizeof(tr));
    trace_ray(start, end, 0x4600400B, skip, tr); // MASK_SHOT | CONTENTS_HITBOX
    return tr.fraction > 0.97f;
}

bool world_to_screen(const features::vec3& world, features::vec3& screen) {
    // View matrix is at engine.dll + offset (varies by build)
    // The address is resolved in sub_10192F70 and stored in the offset table
    // For now use a placeholder
    auto* view_matrix = g_offsets.view_matrix;
    if (!view_matrix)
        return false;

    auto* m = reinterpret_cast<float*>(view_matrix);

    float w = m[12] * world.x + m[13] * world.y + m[14] * world.z + m[15];
    if (w < 0.001f)
        return false;

    float inv_w = 1.0f / w;
    screen.x = (m[0] * world.x + m[1] * world.y + m[2] * world.z + m[3]) * inv_w;
    screen.y = (m[4] * world.x + m[5] * world.y + m[6] * world.z + m[7]) * inv_w;

    // Convert from [-1,1] to screen coordinates
    int w_screen = 0, h_screen = 0;
    // TODO: get screen size from ISurface::GetScreenSize
    w_screen = 1920; h_screen = 1080; // placeholder

    screen.x = (w_screen / 2.0f) * (1.0f + screen.x);
    screen.y = (h_screen / 2.0f) * (1.0f - screen.y);
    screen.z = 0;

    return true;
}

bool is_in_game() {
    if (!g_interfaces.engine_client)
        return false;
    // IVEngineClient::IsInGame is vtable[26]
    using fn = bool(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(g_interfaces.engine_client))[26](g_interfaces.engine_client);
}

bool is_connected() {
    if (!g_interfaces.engine_client)
        return false;
    // IVEngineClient::IsConnected is vtable[27]
    using fn = bool(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(g_interfaces.engine_client))[27](g_interfaces.engine_client);
}

int get_local_player_index() {
    if (!g_interfaces.engine_client)
        return 0;
    // IVEngineClient::GetLocalPlayer is vtable[12]
    using fn = int(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(g_interfaces.engine_client))[12](g_interfaces.engine_client);
}

void execute_client_cmd(const char* cmd) {
    if (!g_interfaces.engine_client)
        return;
    // IVEngineClient::ExecuteClientCmd is vtable[108]
    using fn = void(__thiscall*)(void*, const char*);
    (*reinterpret_cast<fn**>(g_interfaces.engine_client))[108](g_interfaces.engine_client, cmd);
}

}
