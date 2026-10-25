#include "features/scripting.h"
#include "sdk/netvars.h"
#include "sdk/sig_scanner.h"
#include "features/v8_bridge.h"

namespace features {

scripting_api& scripting_api::instance() {
    static scripting_api api;
    return api;
}

bool scripting_api::init() {
    if (initialized_)
        return true;

    callbacks_.clear();
    ui_elements_.clear();
    next_callback_id_ = 1;
    next_ui_id_ = 0;
    initialized_ = true;
    return true;
}

void scripting_api::shutdown() {
    callbacks_.clear();
    ui_elements_.clear();
    initialized_ = false;
}

static script_event event_from_name(const char* name) {
    if (!name) return script_event::count;

    static const struct { const char* name; script_event event; } map[] = {
        { "paint",                script_event::paint },
        { "create_move",          script_event::create_move },
        { "frame_stage_notify",   script_event::frame_stage_notify },
        { "fire_game_event",      script_event::fire_game_event },
        { "player_hurt",          script_event::player_hurt },
        { "item_purchase",        script_event::item_purchase },
        { "round_start",          script_event::round_start },
        { "round_end",            script_event::round_end },
        { "setup_command",        script_event::setup_command },
        { "override_view",        script_event::override_view },
        { "shutdown",             script_event::shutdown },
    };

    for (auto& entry : map) {
        if (_stricmp(name, entry.name) == 0)
            return entry.event;
    }
    return script_event::count;
}

int scripting_api::register_callback(const char* event_name, void* callback) {
    auto event = event_from_name(event_name);
    if (event == script_event::count)
        return -1;

    int id = next_callback_id_++;
    callbacks_.push_back({ id, event_name, event, callback });
    return id;
}

void scripting_api::unregister_callback(int id) {
    for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
        if (it->id == id) {
            callbacks_.erase(it);
            return;
        }
    }
}

void scripting_api::fire_event(script_event event, void* data) {
    for (auto& cb : callbacks_) {
        if (cb.event == event && cb.callback_fn) {
            reinterpret_cast<void(*)(void*)>(cb.callback_fn)(data);
        }
    }
}

u32 scripting_api::find_signature(const char* module_name, const char* pattern) {
    HMODULE mod = GetModuleHandleA(module_name);
    if (!mod)
        return 0;

    auto* result = sdk::find_pattern(mod, pattern);
    return result ? reinterpret_cast<u32>(result) : 0;
}

int scripting_api::get_prop(const char* table, const char* prop) {
    return sdk::netvar_manager::instance().get_offset(table, prop);
}

int scripting_api::new_checkbox(const char* group, const char* name, bool default_val) {
    int id = next_ui_id_++;
    ui_element elem{};
    elem.type = ui_element_type::checkbox;
    elem.name = name;
    elem.group = group;
    elem.int_value = default_val ? 1 : 0;
    ui_elements_.push_back(elem);
    return id;
}

int scripting_api::new_slider(const char* group, const char* name, float min_val, float max_val, float default_val) {
    int id = next_ui_id_++;
    ui_element elem{};
    elem.type = ui_element_type::slider;
    elem.name = name;
    elem.group = group;
    elem.float_value = default_val;
    elem.min_value = min_val;
    elem.max_value = max_val;
    ui_elements_.push_back(elem);
    return id;
}

int scripting_api::new_combobox(const char* group, const char* name, const char** items, int item_count) {
    int id = next_ui_id_++;
    ui_element elem{};
    elem.type = ui_element_type::combobox;
    elem.name = name;
    elem.group = group;
    elem.int_value = 0;
    for (int i = 0; i < item_count; i++)
        elem.combo_items.push_back(items[i]);
    ui_elements_.push_back(elem);
    return id;
}

int scripting_api::new_color_picker(const char* group, const char* name, float r, float g, float b, float a) {
    int id = next_ui_id_++;
    ui_element elem{};
    elem.type = ui_element_type::color_picker;
    elem.name = name;
    elem.group = group;
    elem.color[0] = r;
    elem.color[1] = g;
    elem.color[2] = b;
    elem.color[3] = a;
    ui_elements_.push_back(elem);
    return id;
}

bool scripting_api::get_checkbox(int id) {
    if (id < 0 || id >= static_cast<int>(ui_elements_.size()))
        return false;
    return ui_elements_[id].int_value != 0;
}

float scripting_api::get_slider(int id) {
    if (id < 0 || id >= static_cast<int>(ui_elements_.size()))
        return 0.0f;
    return ui_elements_[id].float_value;
}

int scripting_api::get_combobox(int id) {
    if (id < 0 || id >= static_cast<int>(ui_elements_.size()))
        return 0;
    return ui_elements_[id].int_value;
}

void scripting_api::get_color(int id, float out[4]) {
    if (id < 0 || id >= static_cast<int>(ui_elements_.size())) {
        out[0] = out[1] = out[2] = 1.0f;
        out[3] = 1.0f;
        return;
    }
    memcpy(out, ui_elements_[id].color, sizeof(float) * 4);
}

bool scripting_api::execute_javascript(const char* code) {
    return v8_bridge::instance().execute_script(code);
}

}
