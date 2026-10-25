#include "features/aimbot.h"
#include "sdk/interfaces.h"
#include "sdk/netvars.h"
#include <cmath>
#include <algorithm>

namespace features {

static constexpr float PI = 3.14159265358979323846f;
static constexpr float RAD2DEG = 180.0f / PI;
static constexpr float DEG2RAD = PI / 180.0f;

float vec3::length() const {
    return sqrtf(x * x + y * y + z * z);
}

static qangle calc_angle(const vec3& src, const vec3& dst) {
    vec3 delta = dst - src;
    float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);
    qangle angles;
    angles.pitch = -(atan2f(delta.z, hyp) * RAD2DEG);
    angles.yaw   = atan2f(delta.y, delta.x) * RAD2DEG;
    angles.roll  = 0.0f;
    return angles;
}

static void clamp_angles(qangle& angles) {
    if (angles.pitch > 89.0f) angles.pitch = 89.0f;
    if (angles.pitch < -89.0f) angles.pitch = -89.0f;
    while (angles.yaw > 180.0f) angles.yaw -= 360.0f;
    while (angles.yaw < -180.0f) angles.yaw += 360.0f;
    angles.roll = 0.0f;
}

static void normalize_angles(qangle& angles) {
    while (angles.pitch > 89.0f)   angles.pitch -= 180.0f;
    while (angles.pitch < -89.0f)  angles.pitch += 180.0f;
    while (angles.yaw > 180.0f)    angles.yaw -= 360.0f;
    while (angles.yaw < -180.0f)   angles.yaw += 360.0f;
}

// === AIMBOT ===

aimbot& aimbot::instance() {
    static aimbot aim;
    return aim;
}

float aimbot::get_fov(const qangle& view, const vec3& src, const vec3& dst) {
    qangle aim = calc_angle(src, dst);
    qangle delta;
    delta.pitch = view.pitch - aim.pitch;
    delta.yaw   = view.yaw   - aim.yaw;
    normalize_angles(delta);
    return sqrtf(delta.pitch * delta.pitch + delta.yaw * delta.yaw);
}

bool aimbot::can_hit(const vec3& start, const vec3& end, int skip_entity) {
    // TODO: use engine_trace->TraceRay for visibility check
    (void)start; (void)end; (void)skip_entity;
    return true;
}

void aimbot::scan_targets(cmd_t* cmd) {
    targets_.clear();
    (void)cmd;
    // TODO: iterate entity list, check team, health, dormancy
    // For each valid enemy, get hitbox positions from bone matrix
    // Calculate FOV and damage for each hitbox
}

void aimbot::select_target() {
    best_target_ = nullptr;
    float best_fov = 180.0f;

    for (auto& target : targets_) {
        if (target.fov < best_fov) {
            best_fov = target.fov;
            best_target_ = &target;
        }
    }
}

void aimbot::compute_aim_angle(cmd_t* cmd) {
    if (!best_target_)
        return;

    // TODO: get local player eye position
    vec3 eye_pos(0, 0, 0);
    qangle aim = calc_angle(eye_pos, best_target_->position);
    clamp_angles(aim);

    cmd->viewangles = aim;
}

void aimbot::apply_rcs(qangle& angle) {
    // Recoil control system - subtract punch angle
    // TODO: read m_aimPunchAngle from local player
    (void)angle;
}

void aimbot::run(cmd_t* cmd) {
    // TODO: check if aimbot is enabled in config
    scan_targets(cmd);
    select_target();
    compute_aim_angle(cmd);
}

// === TRIGGERBOT ===

triggerbot& triggerbot::instance() {
    static triggerbot trig;
    return trig;
}

void triggerbot::run(cmd_t* cmd) {
    // TODO: trace from crosshair, check if hitting enemy
    // if (trace hits enemy && delay expired) cmd->buttons |= IN_ATTACK;
    (void)cmd;
}

// === ANTI-AIM ===

anti_aim& anti_aim::instance() {
    static anti_aim aa;
    return aa;
}

void anti_aim::desync(cmd_t* cmd, bool* send_packet) {
    // Desync: alternate between real and fake yaw each tick
    // send_packet controls whether the server receives this tick's angles
    float yaw = cmd->viewangles.yaw;

    if (*send_packet) {
        cmd->viewangles.yaw = yaw + (side_ > 0 ? 58.0f : -58.0f);
        fake_angle_ = cmd->viewangles.yaw;
    } else {
        cmd->viewangles.yaw = yaw + (side_ > 0 ? -58.0f : 58.0f);
        real_angle_ = cmd->viewangles.yaw;
    }
}

void anti_aim::jitter(cmd_t* cmd) {
    static int tick = 0;
    float offset = (tick++ % 2 == 0) ? 45.0f : -45.0f;
    cmd->viewangles.yaw += offset;
}

void anti_aim::freestanding(cmd_t* cmd) {
    // TODO: trace rays to determine which side is more exposed
    // and set side_ accordingly
    (void)cmd;
}

void anti_aim::run(cmd_t* cmd, bool* send_packet) {
    // TODO: check if anti-aim enabled, check if attacking
    // Original binary has: desync, jitter, freestanding, manual override, etc.

    // Don't anti-aim while shooting
    if (cmd->buttons & (1 << 0)) // IN_ATTACK
        return;

    desync(cmd, send_packet);
}

}
