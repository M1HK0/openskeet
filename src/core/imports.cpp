#include "core/imports.h"
#include "core/context.h"
#include "core/console.h"

namespace core {

struct import_entry {
    u8* code_ptr;
};

// Resolve a call/jmp target from code bytes
static u32 resolve_target(u8* code) {
    if (*code == 0xE8 || *code == 0xE9) {
        // Relative call/jmp: target = addr + offset + 5
        i32 offset = *reinterpret_cast<i32*>(code + 1);
        return reinterpret_cast<u32>(code) + offset + 5;
    }
    // Indirect call/jmp
    return *reinterpret_cast<u32*>(code + 1);
}

// Reconstructed from sub_10192DE0
bool fix_imports() {
    auto* ctx = get_context();
    if (!ctx || !ctx->resource_data)
        return true;

    auto get_proc = GetProcAddress;
    auto load_lib = LoadLibraryA;

    u8* table_start = nullptr; // sub_101956E0() returns start
    u8* table_end = nullptr;   // sub_101956D0() returns end

    // TODO: resolve table boundaries from unpacked data
    if (table_start == table_end)
        return true;

    for (u8** entry = reinterpret_cast<u8**>(table_start + 4);
         entry < reinterpret_cast<u8**>(table_end);
         /* advanced inside */) {
        u8* code = *entry;
        u32 target = resolve_target(code);

        // Look up the target in our import resolution table
        // The table maps RVAs to (module_name, proc_name) pairs
        // Uses the decrypted PE resource data

        // If resolution fails, log it
        // sub_1018F410(std::cout, "(fix_imports) failed to get ");

        ++entry;
    }

    return true;
}

}
