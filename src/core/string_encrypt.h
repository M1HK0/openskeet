#pragma once

#include "common.h"

namespace core {

void decrypt_string(u8* dst, const u8* src, int len = 56);

template<int N>
struct encrypted_string {
    u8 data[N];

    encrypted_string(const u8 (&src)[N]) {
        memcpy(data, src, N);
    }

    void decrypt(u8* dst) const {
        decrypt_string(dst, data, N);
    }
};

}
