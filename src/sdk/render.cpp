#include "sdk/render.h"
#include "sdk/interfaces.h"
#include <cstring>

namespace sdk {

render& render::instance() {
    static render r;
    return r;
}

void render::init() {
    surface_ = g_interfaces.surface;
    if (!surface_) return;

    font_esp = create_font();
    set_font_glyph_set(font_esp, "Tahoma", 12, 400, 0, 0, 0x200); // FONTFLAG_OUTLINE

    font_watermark = create_font();
    set_font_glyph_set(font_watermark, "Verdana", 14, 700, 0, 0, 0x200);
}

// ISurface vtable indices (VGUI_Surface031):
// DrawSetColor(Color) = 15
// DrawFilledRect(x0,y0,x1,y1) = 16
// DrawOutlinedRect(x0,y0,x1,y1) = 18
// DrawLine(x0,y0,x1,y1) = 19
// DrawSetTextFont(font) = 23
// DrawSetTextColor(Color) = 25
// DrawSetTextPos(x,y) = 26
// DrawPrintText(wchar*,len,type) = 28
// CreateFont() = 71
// SetFontGlyphSet(font,name,tall,weight,blur,scanlines,flags,min,max) = 72
// GetScreenSize(&w,&h) = 44

void render::draw_set_color(int r, int g, int b, int a) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int, int, int);
    (*reinterpret_cast<fn**>(surface_))[15](surface_, r, g, b, a);
}

void render::draw_filled_rect(int x0, int y0, int x1, int y1) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int, int, int);
    (*reinterpret_cast<fn**>(surface_))[16](surface_, x0, y0, x1, y1);
}

void render::draw_outlined_rect(int x0, int y0, int x1, int y1) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int, int, int);
    (*reinterpret_cast<fn**>(surface_))[18](surface_, x0, y0, x1, y1);
}

void render::draw_line(int x0, int y0, int x1, int y1) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int, int, int);
    (*reinterpret_cast<fn**>(surface_))[19](surface_, x0, y0, x1, y1);
}

void render::draw_set_text_font(unsigned long font) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, unsigned long);
    (*reinterpret_cast<fn**>(surface_))[23](surface_, font);
}

void render::draw_set_text_color(int r, int g, int b, int a) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int, int, int);
    (*reinterpret_cast<fn**>(surface_))[25](surface_, r, g, b, a);
}

void render::draw_set_text_pos(int x, int y) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, int, int);
    (*reinterpret_cast<fn**>(surface_))[26](surface_, x, y);
}

void render::draw_print_text(const wchar_t* text, int len) {
    if (!surface_) return;
    using fn = void(__thiscall*)(void*, const wchar_t*, int, int);
    (*reinterpret_cast<fn**>(surface_))[28](surface_, text, len, 0);
}

unsigned long render::create_font() {
    if (!surface_) return 0;
    using fn = unsigned long(__thiscall*)(void*);
    return (*reinterpret_cast<fn**>(surface_))[71](surface_);
}

bool render::set_font_glyph_set(unsigned long font, const char* name, int tall, int weight, int blur, int scanlines, int flags) {
    if (!surface_) return false;
    using fn = bool(__thiscall*)(void*, unsigned long, const char*, int, int, int, int, int, int, int);
    return (*reinterpret_cast<fn**>(surface_))[72](surface_, font, name, tall, weight, blur, scanlines, flags, 0, 0);
}

void render::get_screen_size(int& width, int& height) {
    if (!surface_) { width = 1920; height = 1080; return; }
    using fn = void(__thiscall*)(void*, int&, int&);
    (*reinterpret_cast<fn**>(surface_))[44](surface_, width, height);
}

void render::draw_string(int x, int y, unsigned long font, const features::color_t& col, const char* text) {
    if (!text || !*text) return;

    wchar_t wbuf[256];
    int len = MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf, 256);
    if (len <= 0) return;
    len--; // exclude null terminator

    draw_set_text_font(font);
    draw_set_text_color(col.r, col.g, col.b, col.a);
    draw_set_text_pos(x, y);
    draw_print_text(wbuf, len);
}

void render::draw_rect(int x, int y, int w, int h, const features::color_t& col) {
    draw_set_color(col.r, col.g, col.b, col.a);
    draw_filled_rect(x, y, x + w, y + h);
}

void render::draw_outlined_box(int x, int y, int w, int h, const features::color_t& col) {
    draw_set_color(col.r, col.g, col.b, col.a);
    draw_outlined_rect(x, y, x + w, y + h);
}

void render::draw_filled_box(int x, int y, int w, int h, const features::color_t& col) {
    draw_set_color(col.r, col.g, col.b, col.a);
    draw_filled_rect(x, y, x + w, y + h);
}

}
