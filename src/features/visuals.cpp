#include "features/visuals.h"
#include "sdk/interfaces.h"
#include "sdk/netvars.h"
#include "sdk/offsets.h"

namespace features {

// === ESP ===

esp& esp::instance() {
    static esp e;
    return e;
}

bool esp::world_to_screen(const vec3& world, vec3& screen) {
    // The view matrix is at engine.dll + some offset
    // w2s: screen.x = (vm[0]*w.x + vm[1]*w.y + vm[2]*w.z + vm[3]) / w
    // TODO: get view matrix from engine client
    (void)world; (void)screen;
    return false;
}

void esp::paint() {
    // TODO: iterate entity list, get valid enemies
    // For each: get bounding box, call draw functions
    for (int i = 1; i <= 64; i++) {
        draw_player(i);
    }
}

void esp::draw_player(int index) {
    // TODO: get entity from entity list
    // Check: alive, not dormant, enemy team
    // Get bone positions for bounding box
    (void)index;
}

void esp::draw_box(int x, int y, int w, int h, const color_t& col) {
    auto* surface = static_cast<void*>(sdk::g_interfaces.surface);
    if (!surface) return;

    // ISurface::DrawSetColor(r, g, b, a)
    // ISurface::DrawOutlinedRect(x0, y0, x1, y1)
    // vtable indices: DrawSetColor = 15, DrawOutlinedRect = 18
    (void)x; (void)y; (void)w; (void)h; (void)col;
}

void esp::draw_health_bar(int x, int y, int h, int health) {
    int bar_h = static_cast<int>(h * (health / 100.0f));
    int green = static_cast<int>(255 * (health / 100.0f));
    int red = 255 - green;
    draw_box(x - 6, y, 4, h, { 0, 0, 0, 200 });
    draw_box(x - 5, y + (h - bar_h), 2, bar_h, { red, green, 0, 255 });
}

void esp::draw_name(int x, int y, const char* name) {
    // ISurface::DrawSetTextFont, DrawSetTextColor, DrawSetTextPos, DrawPrintText
    (void)x; (void)y; (void)name;
}

void esp::draw_weapon(int x, int y, int w, const char* weapon) {
    (void)x; (void)y; (void)w; (void)weapon;
}

void esp::draw_skeleton(int index) {
    // Draw bones between bone positions using ISurface::DrawLine
    (void)index;
}

void esp::draw_snaplines(int x, int y) {
    // Draw line from bottom of screen to player
    (void)x; (void)y;
}

// === CHAMS ===

chams& chams::instance() {
    static chams c;
    return c;
}

void* chams::create_material(const char* name, const char* shader, bool wireframe, bool ignorez) {
    // IMaterialSystem::CreateMaterial creates a runtime material
    // KeyValues with $basetexture, $model, $ignorez, $wireframe, $selfillum
    (void)name; (void)shader; (void)wireframe; (void)ignorez;
    return nullptr;
}

void chams::override_material(bool ignorez, const color_t& col) {
    // IVModelRender::ForcedMaterialOverride
    (void)ignorez; (void)col;
}

void chams::reset_material() {
    // IVModelRender::ForcedMaterialOverride(nullptr)
}

void chams::on_draw_model_execute(void* ctx, void* state, void* info, void* bone_to_world) {
    // TODO: check if entity is player, apply material override before calling original
    // The original binary checks model name for player models
    // Apply hidden material first (ignorez=true), then visible material
    (void)ctx; (void)state; (void)info; (void)bone_to_world;
}

// === GLOW ===

glow& glow::instance() {
    static glow g;
    return g;
}

void glow::run() {
    // Glow object manager is at client.dll + offset
    // Iterate glow objects, set color/render flags for enemies/teammates
    // TODO: resolve glow object manager address from sig scan
}

}
