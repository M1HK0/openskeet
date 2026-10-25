#pragma once

#include "common.h"
#include "features/visuals.h"
#include <string>

namespace sdk {

// ISurface wrapper for VGUI rendering
// VGUI_Surface031 vtable indices from Source SDK
class render {
public:
    static render& instance();

    void init();

    // Drawing
    void draw_set_color(int r, int g, int b, int a);
    void draw_filled_rect(int x0, int y0, int x1, int y1);
    void draw_outlined_rect(int x0, int y0, int x1, int y1);
    void draw_line(int x0, int y0, int x1, int y1);

    // Text
    void draw_set_text_font(unsigned long font);
    void draw_set_text_color(int r, int g, int b, int a);
    void draw_set_text_pos(int x, int y);
    void draw_print_text(const wchar_t* text, int len);

    // Fonts
    unsigned long create_font();
    bool set_font_glyph_set(unsigned long font, const char* name, int tall, int weight, int blur, int scanlines, int flags);

    // Screen info
    void get_screen_size(int& width, int& height);

    // Helper functions
    void draw_string(int x, int y, unsigned long font, const features::color_t& col, const char* text);
    void draw_rect(int x, int y, int w, int h, const features::color_t& col);
    void draw_outlined_box(int x, int y, int w, int h, const features::color_t& col);
    void draw_filled_box(int x, int y, int w, int h, const features::color_t& col);

    unsigned long font_esp = 0;
    unsigned long font_watermark = 0;

private:
    void* surface_ = nullptr;
};

}
