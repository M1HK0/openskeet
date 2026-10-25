#pragma once

#include "common.h"
#include <vector>

namespace features {

struct vec3 {
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3 operator-(const vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    vec3 operator+(const vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    float length() const;
    float dot(const vec3& o) const { return x * o.x + y * o.y + z * o.z; }
};

struct qangle {
    float pitch, yaw, roll;
};

struct cmd_t {
    int     command_number;
    int     tick_count;
    qangle  viewangles;
    qangle  aimdirection;
    float   forwardmove;
    float   sidemove;
    float   upmove;
    int     buttons;
    u8      impulse;
    int     weaponselect;
    int     weaponsubtype;
    int     random_seed;
    short   mousedx;
    short   mousedy;
    bool    hasbeenpredicted;
    char    pad[24];
};

// Aimbot hitbox IDs
enum class hitbox : int {
    head = 0,
    neck,
    pelvis,
    stomach,
    lower_chest,
    chest,
    upper_chest,
    right_thigh,
    left_thigh,
    right_calf,
    left_calf,
    right_foot,
    left_foot,
    right_hand,
    left_hand,
    right_upper_arm,
    left_upper_arm,
    right_forearm,
    left_forearm,
    count
};

class aimbot {
public:
    static aimbot& instance();

    void run(cmd_t* cmd);

private:
    struct aim_target {
        int     entity_index;
        hitbox  best_hitbox;
        vec3    position;
        float   fov;
        float   damage;
        int     health;
        bool    visible;
    };

    void scan_targets(cmd_t* cmd);
    void select_target();
    void compute_aim_angle(cmd_t* cmd);
    void apply_rcs(qangle& angle);
    bool can_hit(const vec3& start, const vec3& end, int skip_entity);
    float get_fov(const qangle& view, const vec3& src, const vec3& dst);

    std::vector<aim_target> targets_;
    aim_target*             best_target_ = nullptr;
};

class triggerbot {
public:
    static triggerbot& instance();
    void run(cmd_t* cmd);

private:
    int  delay_ticks_ = 0;
    bool should_shoot_ = false;
};

class anti_aim {
public:
    static anti_aim& instance();
    void run(cmd_t* cmd, bool* send_packet);

private:
    void desync(cmd_t* cmd, bool* send_packet);
    void jitter(cmd_t* cmd);
    void freestanding(cmd_t* cmd);

    float real_angle_ = 0.0f;
    float fake_angle_ = 0.0f;
    int   side_ = 1;
};

}
