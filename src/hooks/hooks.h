#pragma once

#include "common.h"

namespace hooks {

class vmt_hook {
public:
    vmt_hook() = default;
    ~vmt_hook();

    bool init(void* instance);
    void unhook_all();

    template<typename T>
    T get_original(int index) {
        return reinterpret_cast<T>(original_vtable_[index]);
    }

    bool hook_function(int index, void* hook_fn);

private:
    void**  original_vtable_ = nullptr;
    void**  hooked_vtable_   = nullptr;
    void*** instance_        = nullptr;
    int     vtable_size_     = 0;
};

// Hook instances for each hooked interface
extern vmt_hook client_hook;
extern vmt_hook client_mode_hook;
extern vmt_hook surface_hook;
extern vmt_hook panel_hook;
extern vmt_hook model_render_hook;
extern vmt_hook studio_render_hook;
extern vmt_hook render_view_hook;

// Vectored exception handler for INT3-based hooks
LONG CALLBACK vectored_handler(EXCEPTION_POINTERS* info);

bool install_hooks();
void remove_hooks();

// Hook callbacks (Source Engine vtable hooks)
// These are the actual feature entry points

// CHLClient::FrameStageNotify - called each frame stage
void __fastcall hk_frame_stage_notify(void* ecx, void* edx, int stage);

// IClientMode::CreateMove - player input processing
bool __fastcall hk_create_move(void* ecx, void* edx, float sample_time, void* cmd);

// ISurface::LockCursor - cursor lock control
void __fastcall hk_lock_cursor(void* ecx, void* edx);

// IPanel::PaintTraverse - VGUI rendering
void __fastcall hk_paint_traverse(void* ecx, void* edx, unsigned int panel, bool force_repaint, bool allow_force);

// IVModelRender::DrawModelExecute - model rendering (chams)
void __fastcall hk_draw_model_execute(void* ecx, void* edx, void* ctx, void* state, void* info, void* bone_to_world);

// IStudioRender::DrawModel - alternative model render hook
void __fastcall hk_draw_model(void* ecx, void* edx, void* results, void* info, void* bone_to_world, float* flex_weights, float* flex_delayed_weights, void* origin, int flags);

// IVRenderView::SceneEnd - scene rendering end
void __fastcall hk_scene_end(void* ecx, void* edx);

}
