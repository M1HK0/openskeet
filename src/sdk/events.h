#pragma once

#include "common.h"

namespace sdk {

// IGameEventManager2 wrapper
// Resolved from engine.dll CreateInterface("GAMEEVENTSMANAGER002")

class game_event {
public:
    const char* get_name() {
        using fn = const char*(__thiscall*)(void*);
        return (*reinterpret_cast<fn**>(this))[1](this);
    }

    bool get_bool(const char* key, bool def = false) {
        using fn = bool(__thiscall*)(void*, const char*, bool);
        return (*reinterpret_cast<fn**>(this))[6](this, key, def);
    }

    int get_int(const char* key, int def = 0) {
        using fn = int(__thiscall*)(void*, const char*, int);
        return (*reinterpret_cast<fn**>(this))[7](this, key, def);
    }

    float get_float(const char* key, float def = 0.0f) {
        using fn = float(__thiscall*)(void*, const char*, float);
        return (*reinterpret_cast<fn**>(this))[8](this, key, def);
    }

    const char* get_string(const char* key, const char* def = "") {
        using fn = const char*(__thiscall*)(void*, const char*, const char*);
        return (*reinterpret_cast<fn**>(this))[9](this, key, def);
    }
};

class event_listener {
public:
    virtual ~event_listener() {}
    virtual void fire_game_event(game_event* event) = 0;
    virtual int  get_event_debug_id() { return 42; }
};

class event_manager {
public:
    static event_manager& instance();

    bool init();
    void shutdown();

private:
    class listener_impl : public event_listener {
    public:
        void fire_game_event(game_event* event) override;
    };

    void* manager_ = nullptr;
    listener_impl listener_;
    bool initialized_ = false;
};

}
