#include "sdk/events.h"
#include "sdk/interfaces.h"
#include "features/misc.h"
#include "features/scripting.h"
#include <cstring>

namespace sdk {

event_manager& event_manager::instance() {
    static event_manager mgr;
    return mgr;
}

bool event_manager::init() {
    if (initialized_) return true;

    // Resolve IGameEventManager2 from engine.dll
    manager_ = get_interface("engine.dll", "GAMEEVENTSMANAGER002");
    if (!manager_) return false;

    // IGameEventManager2::AddListener is vtable[3]
    // AddListener(listener, event_name, server_side)
    using add_fn = bool(__thiscall*)(void*, event_listener*, const char*, bool);
    auto add_listener = (*reinterpret_cast<add_fn**>(manager_))[3];

    add_listener(manager_, &listener_, "player_hurt", false);
    add_listener(manager_, &listener_, "item_purchase", false);
    add_listener(manager_, &listener_, "round_start", false);
    add_listener(manager_, &listener_, "round_end", false);
    add_listener(manager_, &listener_, "player_death", false);
    add_listener(manager_, &listener_, "bomb_beginplant", false);
    add_listener(manager_, &listener_, "bomb_begindefuse", false);

    initialized_ = true;
    return true;
}

void event_manager::shutdown() {
    if (!initialized_ || !manager_) return;

    // IGameEventManager2::RemoveListener is vtable[5]
    using remove_fn = void(__thiscall*)(void*, event_listener*);
    (*reinterpret_cast<remove_fn**>(manager_))[5](manager_, &listener_);

    initialized_ = false;
}

void event_manager::listener_impl::fire_game_event(game_event* event) {
    if (!event) return;

    // Dispatch to feature systems
    features::misc_features::instance().on_fire_game_event(event);

    // Dispatch to scripting API
    const char* name = event->get_name();
    if (name) {
        if (strcmp(name, "player_hurt") == 0)
            features::scripting_api::instance().fire_event(features::script_event::player_hurt, event);
        else if (strcmp(name, "round_start") == 0)
            features::scripting_api::instance().fire_event(features::script_event::round_start, event);
        else if (strcmp(name, "round_end") == 0)
            features::scripting_api::instance().fire_event(features::script_event::round_end, event);
        else if (strcmp(name, "item_purchase") == 0)
            features::scripting_api::instance().fire_event(features::script_event::item_purchase, event);
    }

    features::scripting_api::instance().fire_event(features::script_event::fire_game_event, event);
}

}
