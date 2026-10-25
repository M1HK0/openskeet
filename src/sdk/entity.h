#pragma once

#include "common.h"
#include "features/aimbot.h"
#include <cstring>

namespace sdk {

// Source Engine entity access helpers
// These map to the VClientEntityList003 interface

using matrix3x4 = float[3][4];

class entity_t {
public:
    template<typename T>
    T& get(int offset) {
        return *reinterpret_cast<T*>(reinterpret_cast<u8*>(this) + offset);
    }

    void*  renderable()       { return reinterpret_cast<void*>(reinterpret_cast<u8*>(this) + 4); }
    void*  networkable()      { return reinterpret_cast<void*>(reinterpret_cast<u8*>(this) + 8); }
    int    index();
    bool   is_player();
    bool   is_alive();
    bool   is_dormant();
    int    team();
    int    health();
    int    flags();
    float  simulation_time();

    features::vec3  origin();
    features::vec3  eye_position();
    features::vec3  velocity();
    features::qangle  view_angles();

    bool   setup_bones(matrix3x4* out, int max_bones, int mask, float time);

    void*  active_weapon();
    int    weapon_id();

    // Netvar-resolved offsets (set by offset resolver)
    static int off_health;
    static int off_team;
    static int off_flags;
    static int off_origin;
    static int off_velocity;
    static int off_view_offset;
    static int off_sim_time;
    static int off_dormant;
    static int off_active_weapon;
    static int off_aim_punch;
    static int off_spotted;
    static int off_bone_cache;
};

class entity_list {
public:
    static entity_list& instance();

    entity_t* get_entity(int index);
    entity_t* get_local_player();
    int       highest_entity_index();

private:
    void* list_interface_ = nullptr;
};

}
