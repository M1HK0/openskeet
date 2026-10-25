#pragma once

#include "common.h"

namespace hooks {

// VMT (Virtual Method Table) hook class
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

// Exception handler installed via AddVectoredExceptionHandler
// Reconstructed from Handler reference in sub_10192F70
LONG CALLBACK vectored_handler(EXCEPTION_POINTERS* info);

bool install_hooks();
void remove_hooks();

}
