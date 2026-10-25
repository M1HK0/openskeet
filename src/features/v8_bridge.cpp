#include "features/v8_bridge.h"

namespace features {

v8_bridge& v8_bridge::instance() {
    static v8_bridge bridge;
    return bridge;
}

// Reconstructed from sub_100A54A0
// Resolves V8 API exports from v8.dll using GetProcAddress
bool v8_bridge::init() {
    if (initialized_)
        return true;

    v8_module_ = GetModuleHandleA("v8.dll");
    if (!v8_module_)
        return false;

    // The original binary resolves these V8 C++ API functions
    // Note: V8's C++ API uses mangled names
    create_handle_ = reinterpret_cast<v8_create_handle_fn>(
        GetProcAddress(v8_module_, "?CreateHandle@HandleScope@v8@@CAPAPAV_Object@internal@2@PAV342@@Z"));

    enter_context_ = reinterpret_cast<v8_enter_context_fn>(
        GetProcAddress(v8_module_, "?Enter@Context@v8@@QAEXXZ"));

    is_function_ = reinterpret_cast<v8_is_function_fn>(
        GetProcAddress(v8_module_, "?IsFunction@Value@v8@@QBE_NXZ"));

    is_object_ = reinterpret_cast<v8_is_object_fn>(
        GetProcAddress(v8_module_, "?IsObject@Value@v8@@QBE_NXZ"));

    to_object_ = reinterpret_cast<v8_to_object_fn>(
        GetProcAddress(v8_module_, "?ToObject@Value@v8@@QBE?AV?$Local@VObject@v8@@@2@XZ"));

    is_callable_ = reinterpret_cast<v8_is_callable_fn>(
        GetProcAddress(v8_module_, "?IsCallable@Object@v8@@QAE_NXZ"));

    call_function_ = reinterpret_cast<v8_call_function_fn>(
        GetProcAddress(v8_module_, "?Call@Function@v8@@QAE?AV?$Local@VValue@v8@@@2@V?$Handle@VObject@v8@@@2@HQAPAV?$Handle@VValue@v8@@@2@@Z"));

    initialized_ = create_handle_ && enter_context_ && is_function_;
    return initialized_;
}

bool v8_bridge::execute_script(const char* source) {
    if (!initialized_)
        return false;

    // TODO: full script execution pipeline
    // The original binary uses this for the Lua/JS scripting menu
    return true;
}

}
