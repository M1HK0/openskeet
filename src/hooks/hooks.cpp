#include "hooks/hooks.h"
#include "sdk/interfaces.h"
#include "sdk/offsets.h"
#include "sdk/engine.h"
#include "features/aimbot.h"
#include "features/visuals.h"
#include "features/movement.h"
#include "features/skinchanger.h"
#include "features/misc.h"
#include "features/scripting.h"

namespace hooks {

vmt_hook client_hook;
vmt_hook client_mode_hook;
vmt_hook surface_hook;
vmt_hook panel_hook;
vmt_hook model_render_hook;
vmt_hook studio_render_hook;
vmt_hook render_view_hook;

static int count_vtable_entries(void** vtable) {
    int count = 0;
    MEMORY_BASIC_INFORMATION mbi;
    while (VirtualQuery(vtable[count], &mbi, sizeof(mbi)) &&
           mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
        count++;
    }
    return count;
}

vmt_hook::~vmt_hook() {
    unhook_all();
}

bool vmt_hook::init(void* instance) {
    instance_ = static_cast<void***>(instance);
    original_vtable_ = *instance_;

    vtable_size_ = count_vtable_entries(original_vtable_);
    if (vtable_size_ <= 0)
        return false;

    hooked_vtable_ = new void*[vtable_size_];
    memcpy(hooked_vtable_, original_vtable_, vtable_size_ * sizeof(void*));

    DWORD old_protect;
    VirtualProtect(instance_, sizeof(void*), PAGE_READWRITE, &old_protect);
    *instance_ = hooked_vtable_;
    VirtualProtect(instance_, sizeof(void*), old_protect, &old_protect);

    return true;
}

void vmt_hook::unhook_all() {
    if (instance_ && original_vtable_) {
        DWORD old_protect;
        VirtualProtect(instance_, sizeof(void*), PAGE_READWRITE, &old_protect);
        *instance_ = original_vtable_;
        VirtualProtect(instance_, sizeof(void*), old_protect, &old_protect);
    }

    delete[] hooked_vtable_;
    hooked_vtable_ = nullptr;
    original_vtable_ = nullptr;
    instance_ = nullptr;
}

bool vmt_hook::hook_function(int index, void* hook_fn) {
    if (!hooked_vtable_ || index < 0 || index >= vtable_size_)
        return false;

    hooked_vtable_[index] = hook_fn;
    return true;
}

LONG CALLBACK vectored_handler(EXCEPTION_POINTERS* info) {
    if (info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

bool install_hooks() {
    AddVectoredExceptionHandler(0, vectored_handler);

    auto& ifaces = sdk::g_interfaces;

    // CHLClient hooks (VClient018)
    if (ifaces.client) {
        client_hook.init(ifaces.client);
        client_hook.hook_function(37, hk_frame_stage_notify); // FrameStageNotify
    }

    // IClientMode hooks
    // ClientMode pointer is resolved from client.dll offsets
    // TODO: resolve client mode pointer from sig scan

    // ISurface hooks (VGUI_Surface031)
    if (ifaces.surface) {
        surface_hook.init(ifaces.surface);
        surface_hook.hook_function(67, hk_lock_cursor); // LockCursor
    }

    // IPanel hooks (VGUI_Panel009)
    if (ifaces.panel) {
        panel_hook.init(ifaces.panel);
        panel_hook.hook_function(41, hk_paint_traverse); // PaintTraverse
    }

    // IVModelRender hooks (VEngineModel016)
    if (ifaces.model_render) {
        model_render_hook.init(ifaces.model_render);
        model_render_hook.hook_function(21, hk_draw_model_execute); // DrawModelExecute
    }

    // IStudioRender hooks
    if (ifaces.studio_render) {
        studio_render_hook.init(ifaces.studio_render);
        studio_render_hook.hook_function(29, hk_draw_model); // DrawModel
    }

    // IVRenderView hooks
    if (ifaces.render_view) {
        render_view_hook.init(ifaces.render_view);
        render_view_hook.hook_function(9, hk_scene_end); // SceneEnd
    }

    return true;
}

void remove_hooks() {
    client_hook.unhook_all();
    client_mode_hook.unhook_all();
    surface_hook.unhook_all();
    panel_hook.unhook_all();
    model_render_hook.unhook_all();
    studio_render_hook.unhook_all();
    render_view_hook.unhook_all();
    RemoveVectoredExceptionHandler(vectored_handler);
}

// === HOOK IMPLEMENTATIONS ===

void __fastcall hk_frame_stage_notify(void* ecx, void* edx, int stage) {
    // Dispatch to features before calling original
    features::skinchanger::instance().on_frame_stage_notify(stage);
    features::scripting_api::instance().fire_event(features::script_event::frame_stage_notify, &stage);

    client_hook.get_original<void(__thiscall*)(void*, int)>(37)(ecx, stage);
}

bool __fastcall hk_create_move(void* ecx, void* edx, float sample_time, void* cmd) {
    auto original = client_mode_hook.get_original<bool(__thiscall*)(void*, float, void*)>(24);
    bool result = original(ecx, sample_time, cmd);

    if (!cmd) return result;
    auto* ucmd = static_cast<features::cmd_t*>(cmd);

    if (!sdk::is_in_game()) return result;

    // Save original angles for movement fix
    auto old_angles = ucmd->viewangles;
    bool send_packet = true;

    // Run features in order
    features::scripting_api::instance().fire_event(features::script_event::create_move, cmd);
    features::misc_features::instance().run(ucmd);
    features::movement::instance().run(ucmd);
    features::aimbot::instance().run(ucmd);
    features::triggerbot::instance().run(ucmd);
    features::anti_aim::instance().run(ucmd, &send_packet);
    features::fakelag::instance().run(ucmd, &send_packet);

    // Fix movement after angle changes
    features::movement::instance().run(ucmd);

    features::scripting_api::instance().fire_event(features::script_event::setup_command, cmd);

    return false;
}

void __fastcall hk_lock_cursor(void* ecx, void* edx) {
    // Unlock cursor when menu is open
    // TODO: check menu state

    surface_hook.get_original<void(__thiscall*)(void*)>(67)(ecx);
}

void __fastcall hk_paint_traverse(void* ecx, void* edx, unsigned int panel, bool force_repaint, bool allow_force) {
    panel_hook.get_original<void(__thiscall*)(void*, unsigned int, bool, bool)>(41)(ecx, panel, force_repaint, allow_force);

    // Only render on the top-level panel (MatSystemTopPanel / FocusOverlayPanel)
    // IPanel::GetName is vtable[36]
    if (sdk::g_interfaces.panel) {
        using get_name_fn = const char*(__thiscall*)(void*, unsigned int);
        const char* name = (*reinterpret_cast<get_name_fn**>(sdk::g_interfaces.panel))[36](sdk::g_interfaces.panel, panel);
        if (name && name[0] == 'M' && name[3] == 'S') { // "MatSystemTopPanel"
            features::esp::instance().paint();
            features::misc_features::instance().paint();
            features::scripting_api::instance().fire_event(features::script_event::paint, nullptr);
        }
    }
}

void __fastcall hk_draw_model_execute(void* ecx, void* edx, void* ctx, void* state, void* info, void* bone_to_world) {
    features::chams::instance().on_draw_model_execute(ctx, state, info, bone_to_world);
    model_render_hook.get_original<void(__thiscall*)(void*, void*, void*, void*, void*)>(21)(ecx, ctx, state, info, bone_to_world);
}

void __fastcall hk_draw_model(void* ecx, void* edx, void* results, void* info, void* bone_to_world, float* flex_weights, float* flex_delayed_weights, void* origin, int flags) {
    // Alternative chams hook point

    studio_render_hook.get_original<void(__thiscall*)(void*, void*, void*, void*, float*, float*, void*, int)>(29)(
        ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);
}

void __fastcall hk_scene_end(void* ecx, void* edx) {
    render_view_hook.get_original<void(__thiscall*)(void*)>(9)(ecx);
    features::glow::instance().run();
}

}
