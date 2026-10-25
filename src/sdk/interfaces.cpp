#include "sdk/interfaces.h"
#include "valve/source_engine.h"

namespace sdk {

using create_interface_fn = void* (*)(const char*, int*);

void* get_interface(const char* module_name, const char* interface_name) {
    HMODULE module = GetModuleHandleA(module_name);
    if (!module)
        return nullptr;

    auto create_interface = reinterpret_cast<create_interface_fn>(
        GetProcAddress(module, "CreateInterface"));
    if (!create_interface)
        return nullptr;

    int ret = 0;
    return create_interface(interface_name, &ret);
}

}
