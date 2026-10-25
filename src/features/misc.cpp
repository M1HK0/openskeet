#include "features/misc.h"
#include "sdk/interfaces.h"
#include "sdk/offsets.h"
#include <cstring>
#include <cmath>

namespace features {

misc_features& misc_features::instance() {
    static misc_features m;
    return m;
}

void misc_features::set_clantag(const char* tag) {
    // ClanTagChanged is at engine.dll + offset
    // typedef void(__fastcall* fn)(const char*, const char*);
    // Resolved by sig scan from engine.dll
    (void)tag;
}

void misc_features::animate_clantag() {
    // Rotate/animate the clantag string each tick
    static const char* base_tag = "openskeet";
    static int offset = 0;

    clantag_timer_++;
    if (clantag_timer_ < 15)
        return;
    clantag_timer_ = 0;

    char buf[16] = {};
    int len = static_cast<int>(strlen(base_tag));
    for (int i = 0; i < len; i++)
        buf[i] = base_tag[(i + offset) % len];
    buf[len] = 0;
    offset = (offset + 1) % len;

    set_clantag(buf);
}

void misc_features::night_mode() {
    if (night_mode_applied_)
        return;

    // TODO: iterate materials with IMaterialSystem::FirstMaterial/NextMaterial
    // Set $envmaptint on world materials to darken the map
    night_mode_applied_ = true;
}

void misc_features::hitmarker_paint() {
    if (hitmarker_alpha_ <= 0.0f)
        return;

    // Draw crosshair lines at screen center
    // ISurface methods: DrawSetColor, DrawLine
    // TODO: get screen center

    hitmarker_alpha_ -= 0.05f;
    if (hitmarker_alpha_ < 0.0f)
        hitmarker_alpha_ = 0.0f;
}

void misc_features::log_event(void* event) {
    // Log kills, purchases, etc. to console
    (void)event;
}

void misc_features::reveal_ranks() {
    // Send a special message to reveal competitive ranks
    // MsgFunc_ServerRankRevealAll at client.dll + offset
}

void misc_features::radar_hack() {
    // Set m_bSpotted on all enemy entities
    // Iterates entity list and sets the netvar to true
}

void misc_features::run(cmd_t* cmd) {
    (void)cmd;
    animate_clantag();
    radar_hack();
}

void misc_features::paint() {
    hitmarker_paint();
}

void misc_features::on_fire_game_event(void* event) {
    if (!event) return;

    // Check event name for "player_hurt" to trigger hitmarker
    // IGameEvent::GetName(), GetInt("attacker"), etc.
    hitmarker_alpha_ = 1.0f;
    log_event(event);
}

}
