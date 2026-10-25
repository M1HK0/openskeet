#include "features/movement.h"
#include "sdk/interfaces.h"
#include <cmath>

namespace features {

static constexpr float PI = 3.14159265358979323846f;
static constexpr float DEG2RAD = PI / 180.0f;

static constexpr int FL_ONGROUND = (1 << 0);
static constexpr int IN_JUMP     = (1 << 1);

movement& movement::instance() {
    static movement m;
    return m;
}

void movement::bhop(cmd_t* cmd) {
    // If not on ground, release jump so it can be re-pressed next tick
    // TODO: get flags from local player entity
    int flags = 0; // local_player->m_fFlags

    if (!(flags & FL_ONGROUND)) {
        cmd->buttons &= ~IN_JUMP;
    }
}

void movement::autostrafe(cmd_t* cmd) {
    // Air strafe: set sidemove based on mouse movement direction
    // TODO: get local player velocity for speed-optimal strafing
    if (cmd->mousedx > 0)
        cmd->sidemove = 450.0f;
    else if (cmd->mousedx < 0)
        cmd->sidemove = -450.0f;
}

void movement::fix_movement(cmd_t* cmd, const qangle& old_angles) {
    // Fix movement after viewangle changes (e.g., after anti-aim)
    float yaw_delta = (cmd->viewangles.yaw - old_angles.yaw) * DEG2RAD;
    float cos_val = cosf(yaw_delta);
    float sin_val = sinf(yaw_delta);

    float old_forward = cmd->forwardmove;
    float old_side    = cmd->sidemove;

    cmd->forwardmove = cos_val * old_forward + sin_val * old_side;
    cmd->sidemove    = cos_val * old_side    - sin_val * old_forward;
}

void movement::edge_bug(cmd_t* cmd) {
    // Detect edge and release crouch at the right tick
    (void)cmd;
}

void movement::jump_bug(cmd_t* cmd) {
    // Jump + uncrouch on landing tick for 0-velocity landing
    (void)cmd;
}

void movement::run(cmd_t* cmd) {
    if (cmd->buttons & IN_JUMP)
        bhop(cmd);
    autostrafe(cmd);
}

// === FAKELAG ===

fakelag& fakelag::instance() {
    static fakelag fl;
    return fl;
}

void fakelag::run(cmd_t* cmd, bool* send_packet) {
    // Choke packets to make player harder to hit
    // The original binary supports: factor, adaptive, fluctuate, break-lag-comp modes
    (void)cmd;

    if (choke_count_ < max_choke_) {
        *send_packet = false;
        choke_count_++;
    } else {
        *send_packet = true;
        choke_count_ = 0;
    }
}

}
