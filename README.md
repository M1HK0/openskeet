# openskeet

Open-source reconstruction of the skeet (gamesense) CS:GO cheat DLL, reverse-engineered from the original binary using IDA Pro 9.4 with Hex-Rays decompiler.

## Binary Analysis Summary

| Property | Value |
|----------|-------|
| **Target** | `skeet.dll` (PE32 x86) |
| **Size** | 13.2 MB |
| **Functions** | 1,358 total (85 named, 1,273 unnamed, 57 library) |
| **Strings** | 7,253 |
| **Obfuscation** | Custom (encrypted PE resources, XOR string encryption, dynamic RWX code, thread hijacking) |
| **SHA-256** | `dd6799087cd1db3c42f6cd488c962ac1b77f6f06fc5c63ab821f31d08455d095` |

## Architecture

```
DllMain
  └─ CreateThread(StartAddress)
       ├─ init_context()          ← Singleton + PE resource loading (0x65/0x66)
       ├─ unpack_payload()        ← Copies 3MB packed code (0x2FC000 bytes)
       ├─ decrypt_string()        ← XOR(index % 52 + 51) with SSE4.1 optimization
       ├─ [wait for serverbrowser.dll]
       ├─ fix_imports()           ← Dynamic LoadLibraryA/GetProcAddress resolution
       ├─ resolve_modules()       ← GetModuleHandleA for 19 game DLLs
       ├─ resolve_offsets()       ← Hardcoded + signature-scanned offsets
       ├─ init_netvars()          ← RecvTable walk for runtime netvar resolution
       ├─ install_hooks()         ← VMT hooks + VEH-based INT3 hooks
       └─ thread_hijack()         ← NtCreateSection + SetThreadContext
```

### Game Modules Referenced
client.dll, engine.dll, vstdlib.dll, vgui2.dll, vguimatsurface.dll, materialsystem.dll, studiorender.dll, shaderapidx9.dll, panorama.dll, panoramauiclient.dll, inputsystem.dll, filesystem_stdio.dll, server.dll, matchmaking.dll, vphysics.dll, gameoverlayrenderer.dll, soundsystem.dll, datacache.dll, localize.dll

### Key Subsystems
- **String Encryption**: XOR cipher `dst[i] = src[i] ^ ((i % 52) + 51)` with SSE2/SSE4.1 SIMD path
- **PE Resource Decryption**: Resources 0x65 (interface table) and 0x66 (hook data)
- **Import Resolution**: Walks a table of call/jmp stubs, resolves via `LoadLibraryA`/`GetProcAddress`
- **Thread Hijacking**: Creates suspended thread, allocates 1MB RWX, uses `NtCreateSection`, rewrites EIP/ESP/ESI/EDI via `SetThreadContext`
- **V8 JavaScript Bridge**: Dynamically loads V8 API from `v8.dll` for scripting system
- **Signature Scanner**: IDA-style byte pattern matching with wildcard support

## Building

Requires Visual Studio 2022 with C++ desktop workload and CMake 3.20+.

```bash
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A Win32 ..
cmake --build . --config Release
```

Output: `build/bin/Release/openskeet.dll`

## Project Structure

```
src/
├── dllmain.cpp              # DLL entry point + main init thread
├── core/
│   ├── context.{h,cpp}      # Global singleton context
│   ├── memory.{h,cpp}       # Payload unpacking
│   ├── resources.{h,cpp}    # PE resource loading/decryption
│   ├── imports.{h,cpp}      # Dynamic import resolution
│   ├── thread_hijack.{h,cpp}# Anti-analysis thread hijacking
│   ├── string_encrypt.{h,cpp}# XOR string decryption (SSE optimized)
│   └── console.{h,cpp}      # Formatted console output
├── sdk/
│   ├── netvars.{h,cpp}      # RecvTable netvar resolver
│   ├── interfaces.{h,cpp}   # Source Engine CreateInterface wrapper
│   ├── offsets.{h,cpp}      # Hardcoded game offsets
│   └── sig_scanner.{h,cpp}  # Byte pattern scanner
├── hooks/
│   └── hooks.{h,cpp}        # VMT + VEH hooking system
├── features/
│   └── v8_bridge.{h,cpp}    # V8 JavaScript scripting bridge
└── include/
    ├── common.h              # Shared types and includes
    └── valve/
        └── source_engine.h   # Source Engine SDK types
```

## Reconstruction Progress

- [x] DllMain entry flow
- [x] Thread creation and init sequence
- [x] String encryption/decryption (XOR + SSE)
- [x] PE resource loading
- [x] Memory unpacking
- [x] Import resolution framework
- [x] Module resolution (19 game DLLs)
- [x] Offset resolution (hardcoded + sig scan)
- [x] Netvar system (RecvTable walker)
- [x] VMT hook framework
- [x] VEH hook handler
- [x] Thread hijacking (NtCreateSection)
- [x] V8 bridge (API resolution)
- [x] Signature scanner
- [ ] Full feature implementations (aimbot, ESP, etc.)
- [ ] Complete import table reconstruction
- [ ] Encrypted resource content reconstruction
- [ ] Full V8 scripting system
- [ ] Anti-aim, resolver, rage/legit bot
- [ ] Menu/UI system

## Disclaimer

This project is for **educational and research purposes only**. It documents the internal architecture of a game cheat for security research and reverse engineering study. Do not use this code to cheat in online games.

## License

MIT
