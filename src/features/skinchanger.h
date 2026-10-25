#pragma once

#include "common.h"
#include <string>
#include <vector>

namespace features {

struct skin_info {
    int     item_definition_index;
    int     paint_kit;
    int     seed;
    float   wear;
    int     stat_trak;
    int     quality;
    int     entity_quality;
    std::string custom_name;
};

class skinchanger {
public:
    static skinchanger& instance();

    void on_frame_stage_notify(int stage);

private:
    void apply_skins();
    void apply_knife_model(int definition_index);
    void apply_glove_model(int definition_index);
    void force_update_skin(void* weapon, const skin_info& info);

    std::vector<skin_info> weapon_skins_;
    skin_info knife_skin_{};
    skin_info glove_skin_{};
};

}
