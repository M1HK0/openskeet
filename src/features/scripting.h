#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace features {

// Script callback types discovered from string analysis:
// set_event_callback, find_signature, get_prop, new_checkbox, new_slider,
// new_combobox, new_color_picker, execute_javascript
enum class script_event : int {
    paint = 0,
    create_move,
    frame_stage_notify,
    fire_game_event,
    player_hurt,
    item_purchase,
    round_start,
    round_end,
    setup_command,
    override_view,
    shutdown,
    count
};

struct script_callback {
    int         id;
    std::string name;
    script_event event;
    void*       callback_fn;
};

// UI element types from strings: new_checkbox, new_slider, new_combobox, new_color_picker
enum class ui_element_type : int {
    checkbox,
    slider,
    combobox,
    color_picker,
    label,
    button,
    hotkey
};

struct ui_element {
    ui_element_type type;
    std::string     name;
    std::string     group;
    int             int_value;
    float           float_value;
    float           min_value;
    float           max_value;
    float           color[4];
    std::vector<std::string> combo_items;
};

class scripting_api {
public:
    static scripting_api& instance();

    bool init();
    void shutdown();

    // Callback registration (from set_event_callback)
    int  register_callback(const char* event_name, void* callback);
    void unregister_callback(int id);
    void fire_event(script_event event, void* data);

    // Pattern scanning (from find_signature)
    u32  find_signature(const char* module_name, const char* pattern);

    // Netvar access (from get_prop)
    int  get_prop(const char* table, const char* prop);

    // UI element creation
    int  new_checkbox(const char* group, const char* name, bool default_val);
    int  new_slider(const char* group, const char* name, float min_val, float max_val, float default_val);
    int  new_combobox(const char* group, const char* name, const char** items, int item_count);
    int  new_color_picker(const char* group, const char* name, float r, float g, float b, float a);

    // UI value getters
    bool  get_checkbox(int id);
    float get_slider(int id);
    int   get_combobox(int id);
    void  get_color(int id, float out[4]);

    // JS execution (from execute_javascript)
    bool execute_javascript(const char* code);

private:
    std::vector<script_callback> callbacks_;
    std::vector<ui_element>      ui_elements_;
    int next_callback_id_ = 1;
    int next_ui_id_ = 0;
    bool initialized_ = false;
};

}
