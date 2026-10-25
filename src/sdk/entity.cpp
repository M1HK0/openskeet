#include "sdk/entity.h"
#include "sdk/interfaces.h"
#include "sdk/netvars.h"

namespace sdk {

// Static offsets - resolved at init time from netvars
int entity_t::off_health = 0x100;
int entity_t::off_team = 0xF4;
int entity_t::off_flags = 0x104;
int entity_t::off_origin = 0x138;
int entity_t::off_velocity = 0x114;
int entity_t::off_view_offset = 0x108;
int entity_t::off_sim_time = 0x268;
int entity_t::off_dormant = 0xED;
int entity_t::off_active_weapon = 0x2EF8;
int entity_t::off_aim_punch = 0x302C;
int entity_t::off_spotted = 0x93D;
int entity_t::off_bone_cache = 0x2914;

int entity_t::index() {
    // IClientNetworkable::entindex is vtable[10]
    auto* networkable_ptr = networkable();
    if (!networkable_ptr) return -1;
    using fn = int(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(networkable_ptr))[10](networkable_ptr);
}

bool entity_t::is_player() {
    // IClientNetworkable::GetClientClass then check class id
    return true; // Simplified - real check is class ID based
}

bool entity_t::is_alive() {
    return health() > 0;
}

bool entity_t::is_dormant() {
    return get<bool>(off_dormant);
}

int entity_t::team() {
    return get<int>(off_team);
}

int entity_t::health() {
    return get<int>(off_health);
}

int entity_t::flags() {
    return get<int>(off_flags);
}

float entity_t::simulation_time() {
    return get<float>(off_sim_time);
}

features::vec3 entity_t::origin() {
    return get<features::vec3>(off_origin);
}

features::vec3 entity_t::eye_position() {
    auto org = origin();
    auto view_offset = get<features::vec3>(off_view_offset);
    return org + view_offset;
}

features::vec3 entity_t::velocity() {
    return get<features::vec3>(off_velocity);
}

bool entity_t::setup_bones(matrix3x4* out, int max_bones, int mask, float time) {
    auto* renderable_ptr = renderable();
    if (!renderable_ptr) return false;
    // IClientRenderable::SetupBones is vtable[13]
    using fn = bool(__thiscall*)(void*, matrix3x4*, int, int, float);
    return (*reinterpret_cast<fn**>(renderable_ptr))[13](renderable_ptr, out, max_bones, mask, time);
}

void* entity_t::active_weapon() {
    int handle = get<int>(off_active_weapon);
    // Entity handle -> entity pointer via entity list
    return nullptr; // TODO: resolve via entity list
}

int entity_t::weapon_id() {
    return 0; // TODO
}

// Entity list
entity_list& entity_list::instance() {
    static entity_list list;
    return list;
}

entity_t* entity_list::get_entity(int index) {
    if (!list_interface_)
        list_interface_ = g_interfaces.entity_list;
    if (!list_interface_)
        return nullptr;

    // IClientEntityList::GetClientEntity is vtable[3]
    using fn = entity_t*(__thiscall*)(void*, int);
    return (*reinterpret_cast<fn**>(list_interface_))[3](list_interface_, index);
}

entity_t* entity_list::get_local_player() {
    // Engine client GetLocalPlayer returns player index
    if (!g_interfaces.engine_client)
        return nullptr;

    using fn = int(__thiscall*)(void*);
    int local_idx = (*reinterpret_cast<fn**>(g_interfaces.engine_client))[12](g_interfaces.engine_client);
    return get_entity(local_idx);
}

int entity_list::highest_entity_index() {
    if (!list_interface_)
        list_interface_ = g_interfaces.entity_list;
    if (!list_interface_)
        return 64;

    // IClientEntityList::GetHighestEntityIndex is vtable[6]
    using fn = int(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(list_interface_))[6](list_interface_);
}

}
