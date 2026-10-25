#pragma once

#include "common.h"
#include "features/aimbot.h"

namespace features {

struct color_t {
    int r, g, b, a;
    color_t() : r(255), g(255), b(255), a(255) {}
    color_t(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
};

class esp {
public:
    static esp& instance();

    void paint();

private:
    void draw_player(int index);
    void draw_box(int x, int y, int w, int h, const color_t& col);
    void draw_health_bar(int x, int y, int h, int health);
    void draw_name(int x, int y, const char* name);
    void draw_weapon(int x, int y, int w, const char* weapon);
    void draw_skeleton(int index);
    void draw_snaplines(int x, int y);

    bool world_to_screen(const vec3& world, vec3& screen);
};

class chams {
public:
    static chams& instance();

    void on_draw_model_execute(void* ctx, void* state, void* info, void* bone_to_world);

private:
    void* create_material(const char* name, const char* shader, bool wireframe, bool ignorez);
    void  override_material(bool ignorez, const color_t& col);
    void  reset_material();

    void* material_flat_     = nullptr;
    void* material_textured_ = nullptr;
    void* material_glow_     = nullptr;
};

class glow {
public:
    static glow& instance();

    void run();

private:
    struct glow_object {
        void* entity;
        float color[4];
        char  pad[16];
        bool  render_when_occluded;
        bool  render_when_unoccluded;
        bool  full_bloom;
        char  pad2[13];
    };
};

}
