cmake_minimum_required(VERSION 3.20)
project(openskeet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(openskeet SHARED
    src/dllmain.cpp
    src/core/context.cpp
    src/core/memory.cpp
    src/core/resources.cpp
    src/core/imports.cpp
    src/core/thread_hijack.cpp
    src/core/string_encrypt.cpp
    src/core/console.cpp
    src/core/vm_decoder.cpp
    src/core/config.cpp
    src/core/json.cpp
    src/sdk/netvars.cpp
    src/sdk/interfaces.cpp
    src/sdk/offsets.cpp
    src/sdk/sig_scanner.cpp
    src/hooks/hooks.cpp
    src/features/v8_bridge.cpp
    src/features/scripting.cpp
    src/features/aimbot.cpp
    src/features/visuals.cpp
    src/features/movement.cpp
    src/features/skinchanger.cpp
    src/features/misc.cpp
)

target_include_directories(openskeet PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(openskeet PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MSVC)
    target_compile_options(openskeet PRIVATE /W3 /arch:SSE2)
    target_link_options(openskeet PRIVATE /DEF:${CMAKE_CURRENT_SOURCE_DIR}/src/openskeet.def)
endif()

target_link_libraries(openskeet PRIVATE
    ntdll
)

set_target_properties(openskeet PROPERTIES
    OUTPUT_NAME "openskeet"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
