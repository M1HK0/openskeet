#include "sdk/sig_scanner.h"
#include <vector>

namespace sdk {

struct pattern_byte {
    u8   value;
    bool wildcard;
};

static std::vector<pattern_byte> parse_pattern(const char* pattern) {
    std::vector<pattern_byte> bytes;

    while (*pattern) {
        if (*pattern == ' ') {
            ++pattern;
            continue;
        }

        if (*pattern == '?') {
            bytes.push_back({0, true});
            ++pattern;
            if (*pattern == '?')
                ++pattern;
            continue;
        }

        u8 val = 0;
        for (int i = 0; i < 2 && *pattern; i++, pattern++) {
            val <<= 4;
            char c = *pattern;
            if (c >= '0' && c <= '9')      val |= c - '0';
            else if (c >= 'A' && c <= 'F') val |= c - 'A' + 10;
            else if (c >= 'a' && c <= 'f') val |= c - 'a' + 10;
        }
        bytes.push_back({val, false});
    }

    return bytes;
}

u8* find_pattern(HMODULE module, const char* pattern) {
    if (!module || !pattern)
        return nullptr;

    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<u8*>(module) + dos->e_lfanew);

    auto* section = IMAGE_FIRST_SECTION(nt);
    u8* scan_start = reinterpret_cast<u8*>(module) + section->VirtualAddress;
    u32 scan_size = section->Misc.VirtualSize;

    auto bytes = parse_pattern(pattern);
    if (bytes.empty())
        return nullptr;

    for (u32 i = 0; i <= scan_size - bytes.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < bytes.size(); j++) {
            if (!bytes[j].wildcard && scan_start[i + j] != bytes[j].value) {
                found = false;
                break;
            }
        }
        if (found)
            return scan_start + i;
    }

    return nullptr;
}

}
