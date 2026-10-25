#pragma once

#include "common.h"
#include "features/aimbot.h"

namespace features {

class misc_features {
public:
    static misc_features& instance();

    void run(cmd_t* cmd);
    void paint();
    void on_fire_game_event(void* event);

private:
    // Clantag animation
    void animate_clantag();
    void set_clantag(const char* tag);

    // Night mode
    void night_mode();

    // Hitmarker
    void hitmarker_paint();

    // Event log
    void log_event(void* event);

    // Reveal ranks
    void reveal_ranks();

    // Radar
    void radar_hack();

    int   clantag_timer_ = 0;
    float hitmarker_alpha_ = 0.0f;
    float hitmarker_time_ = 0.0f;
    bool  night_mode_applied_ = false;
};

}
