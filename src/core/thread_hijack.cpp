#include "core/thread_hijack.h"
#include "core/context.h"

using fn_NtCreateSection = LONG(__stdcall*)(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    PVOID ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle
);

static DWORD WINAPI hijack_stub(LPVOID param) {
    // sub_10192140 - JUMPOUT(0x1337)
    // This is the stub that gets its context rewritten via SetThreadContext.
    // The real execution starts at the address set in Context.Eip by thread_hijack.
    return 0;
}

namespace core {

// Reconstructed from sub_101942A0
bool thread_hijack(void* ctx_ptr) {
    auto* ctx = static_cast<u32*>(ctx_ptr);

    DWORD thread_id = 0;
    HANDLE thread = CreateThread(
        nullptr,
        0,
        hijack_stub,
        nullptr,
        CREATE_SUSPENDED,
        &thread_id
    );

    if (thread == INVALID_HANDLE_VALUE)
        return false;

    // Allocate 1MB RWX memory for the payload execution
    void* rwx_mem = VirtualAlloc(
        nullptr,
        0x100000,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );

    if (ctx)
        ctx[4] = reinterpret_cast<u32>(rwx_mem);

    // Set up thread context to redirect execution
    CONTEXT thread_ctx = {};
    thread_ctx.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

    // The original sets EIP/ESP/ESI/EDI to specific addresses
    // 0x434AD7AC = unpacked code entry point (runtime address)
    thread_ctx.Eip = 0x434AD7AC;
    thread_ctx.Esp = reinterpret_cast<DWORD>(rwx_mem) + 0x20000;
    thread_ctx.Esi = 0x434AD7AC;
    thread_ctx.Edi = 0x434AD7AC;

    // Create a section via NtCreateSection for code injection
    auto nt_create_section = reinterpret_cast<fn_NtCreateSection>(
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateSection")
    );

    if (nt_create_section) {
        HANDLE section = nullptr;
        LARGE_INTEGER section_size;
        section_size.QuadPart = 0x100000;

        nt_create_section(
            &section,
            0xE, // SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE
            nullptr,
            &section_size,
            PAGE_EXECUTE_READWRITE,
            0x8000000, // SEC_COMMIT
            nullptr
        );

        // Store section handle on the hijacked thread's stack
        *reinterpret_cast<HANDLE*>(thread_ctx.Esp + 12) = section;
    }

    SetThreadContext(thread, &thread_ctx);
    ResumeThread(thread);
    CloseHandle(thread);

    return true;
}

}
