#pragma once

#include "common.h"

namespace sdk {

void* get_interface(const char* module_name, const char* interface_name);

template<typename T>
T* get_interface(const char* module_name, const char* interface_name) {
    return static_cast<T*>(get_interface(module_name, interface_name));
}

}
