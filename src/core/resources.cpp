#include "core/resources.h"
#include "core/context.h"

namespace core {

static void* load_pe_resource(HMODULE module, int resource_id, size_t& out_size) {
    HRSRC res = FindResourceW(module, MAKEINTRESOURCEW(resource_id), MAKEINTRESOURCEW(10));
    if (!res)
        return nullptr;

    HGLOBAL data = LoadResource(module, res);
    if (!data)
        return nullptr;

    out_size = SizeofResource(module, res);
    return LockResource(data);
}

// Reconstructed from sub_10192640
bool load_resources(context* ctx, HMODULE dll_module) {
    if (!ctx || !dll_module)
        return false;

    // Load resource 0x65 - module/interface table
    size_t res65_size = 0;
    void* res65_data = load_pe_resource(dll_module, 0x65, res65_size);
    if (res65_data && res65_size > 0) {
        ctx->resource_data = VirtualAlloc(nullptr, res65_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (ctx->resource_data) {
            memcpy(ctx->resource_data, res65_data, res65_size);
        }
    }

    // Load resource 0x66 - hook data
    size_t res66_size = 0;
    void* res66_data = load_pe_resource(dll_module, 0x66, res66_size);
    if (res66_data && res66_size > 0) {
        ctx->hook_table = VirtualAlloc(nullptr, res66_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (ctx->hook_table) {
            memcpy(ctx->hook_table, res66_data, res66_size);
        }
    }

    // Parse module/interface structure from resource 0x65
    // The resource contains a nested structure:
    //   [module_count]
    //   for each module:
    //     [module_name (encrypted wchar)]
    //     [interface_count]
    //     for each interface:
    //       [interface_name (encrypted wchar)]
    //       [function_table_rva]
    if (ctx->resource_data) {
        auto* ptr = static_cast<u8*>(ctx->resource_data);
        ctx->module_count = *reinterpret_cast<u32*>(ptr);
    }

    return true;
}

}
