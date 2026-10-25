#pragma once

#include "common.h"

namespace features {

// Reconstructed from sub_100A54A0
// V8 JavaScript engine bridge for the scripting system
// Dynamically loads v8.dll exports to provide JS scripting
class v8_bridge {
public:
    static v8_bridge& instance();

    bool init();
    bool execute_script(const char* source);

private:
    // V8 API function pointers resolved from v8.dll
    using v8_create_handle_fn  = void* (*)(void*);
    using v8_enter_context_fn  = void (*)(void*);
    using v8_is_function_fn    = bool (*)(void*);
    using v8_is_object_fn      = bool (*)(void*);
    using v8_to_object_fn      = void* (*)(void*);
    using v8_is_callable_fn    = bool (*)(void*);
    using v8_call_function_fn  = void* (*)(void*, void*, int, void**);

    v8_create_handle_fn  create_handle_  = nullptr;
    v8_enter_context_fn  enter_context_  = nullptr;
    v8_is_function_fn    is_function_    = nullptr;
    v8_is_object_fn      is_object_      = nullptr;
    v8_to_object_fn      to_object_      = nullptr;
    v8_is_callable_fn    is_callable_    = nullptr;
    v8_call_function_fn  call_function_  = nullptr;

    HMODULE v8_module_ = nullptr;
    bool    initialized_ = false;
};

}
