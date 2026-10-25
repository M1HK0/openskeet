#pragma once

#include "common.h"
#include "features/aimbot.h"

namespace features {

class movement {
public:
    static movement& instance();

    void run(cmd_t* cmd);

private:
    void bhop(cmd_t* cmd);
    void autostrafe(cmd_t* cmd);
    void fix_movement(cmd_t* cmd, const qangle& old_angles);
    void edge_bug(cmd_t* cmd);
    void jump_bug(cmd_t* cmd);

    bool was_on_ground_ = false;
};

class fakelag {
public:
    static fakelag& instance();
    void run(cmd_t* cmd, bool* send_packet);

private:
    int choke_count_ = 0;
    int max_choke_   = 14;
};

}
