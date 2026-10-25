#include "core/context.h"
#include "core/resources.h"

namespace core {

static context* g_context = nullptr;

context* context::instance() {
    return g_context;
}

// Reconstructed from sub_10194CF0
bool context::init(HMODULE dll_module) {
    if (g_context)
        return true;

    auto* ctx = static_cast<context*>(operator new(0x2C));
    if (!ctx)
        return false;

    memset(ctx, 0, 0x2C);

    if (!load_resources(ctx, dll_module)) {
        delete ctx;
        return false;
    }

    g_context = ctx;
    return true;
}

bool init_context(HMODULE dll_module) {
    if (g_context)
        return true;

    auto* ctx = static_cast<context*>(operator new(0x2C));
    memset(ctx, 0, 0x2C);

    if (!load_resources(ctx, dll_module)) {
        operator delete(ctx);
        return false;
    }

    g_context = ctx;
    return true;
}

context* get_context() {
    return g_context;
}

}
