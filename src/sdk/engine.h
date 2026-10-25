#pragma once

#include "common.h"
#include "features/aimbot.h"

namespace sdk {

// Source Engine helper wrappers for common engine client operations

struct trace_t {
    features::vec3 startpos;
    features::vec3 endpos;
    char pad0[20];
    float fraction;
    int contents;
    unsigned short disp_flags;
    bool all_solid;
    bool start_solid;
    char pad1[4];
    struct {
        float normal[3];
        float dist;
        int type;
    } plane;
    int surface_flags;
    int surface_name_index;
    void* entity;
    int hitbox;
};

struct ray_t {
    features::vec3 start;
    char pad0[4];
    features::vec3 delta;
    char pad1[4];
    features::vec3 start_offset;
    char pad2[4];
    features::vec3 extents;
    char pad3[4];
    void* world_axis_transform;
    bool is_ray;
    bool is_swept;
};

struct trace_filter_t {
    void** vtable;
    void* skip_entity;
};

bool trace_ray(const features::vec3& start, const features::vec3& end, unsigned int mask, void* skip, trace_t& trace);
bool is_visible(const features::vec3& start, const features::vec3& end, void* skip);
bool world_to_screen(const features::vec3& world, features::vec3& screen);
features::vec3 get_view_matrix_row(int row);

// Engine client wrappers
bool is_in_game();
bool is_connected();
int  get_local_player_index();
void execute_client_cmd(const char* cmd);

}
