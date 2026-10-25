#include "hooks/hooks.h"

namespace hooks {

static int count_vtable_entries(void** vtable) {
    int count = 0;
    MEMORY_BASIC_INFORMATION mbi;
    while (VirtualQuery(vtable[count], &mbi, sizeof(mbi)) &&
           mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
        count++;
    }
    return count;
}

vmt_hook::~vmt_hook() {
    unhook_all();
}

bool vmt_hook::init(void* instance) {
    instance_ = static_cast<void***>(instance);
    original_vtable_ = *instance_;

    vtable_size_ = count_vtable_entries(original_vtable_);
    if (vtable_size_ <= 0)
        return false;

    hooked_vtable_ = new void*[vtable_size_];
    memcpy(hooked_vtable_, original_vtable_, vtable_size_ * sizeof(void*));

    DWORD old_protect;
    VirtualProtect(instance_, sizeof(void*), PAGE_READWRITE, &old_protect);
    *instance_ = hooked_vtable_;
    VirtualProtect(instance_, sizeof(void*), old_protect, &old_protect);

    return true;
}

void vmt_hook::unhook_all() {
    if (instance_ && original_vtable_) {
        DWORD old_protect;
        VirtualProtect(instance_, sizeof(void*), PAGE_READWRITE, &old_protect);
        *instance_ = original_vtable_;
        VirtualProtect(instance_, sizeof(void*), old_protect, &old_protect);
    }

    delete[] hooked_vtable_;
    hooked_vtable_ = nullptr;
    original_vtable_ = nullptr;
    instance_ = nullptr;
}

bool vmt_hook::hook_function(int index, void* hook_fn) {
    if (!hooked_vtable_ || index < 0 || index >= vtable_size_)
        return false;

    hooked_vtable_[index] = hook_fn;
    return true;
}

// Vectored exception handler (from sub_10192F70 -> AddVectoredExceptionHandler)
LONG CALLBACK vectored_handler(EXCEPTION_POINTERS* info) {
    // The original binary installs this to handle INT3 breakpoints
    // placed at hook points for stealthier hooking
    if (info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
        // Redirect execution to the hook function based on the exception address
        // The mapping is set up during hook installation
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

bool install_hooks() {
    // Hook installation is done through the unpacked payload
    // The resource data (0x65/0x66) contains the hook targets
    // Each entry maps: (module, interface, vtable_index) -> hook_function

    // Add vectored exception handler for INT3-based hooks
    AddVectoredExceptionHandler(0, vectored_handler);

    return true;
}

void remove_hooks() {
    RemoveVectoredExceptionHandler(vectored_handler);
}

}
