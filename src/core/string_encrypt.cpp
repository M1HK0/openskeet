#include "core/string_encrypt.h"

namespace core {

// Reconstructed from sub_1018ED00
// XOR decryption: dst[i] = src[i] ^ ((i % 52) + 51)
// SSE2/SSE4.1 optimized path when available
void decrypt_string(u8* dst, const u8* src, int len) {
    static const bool has_sse41 = [] {
        int cpuinfo[4];
        __cpuid(cpuinfo, 1);
        return (cpuinfo[2] & (1 << 19)) != 0;
    }();

    if (has_sse41 && len >= 8 && (uintptr_t(dst) + len <= uintptr_t(src) || uintptr_t(src) + len <= uintptr_t(dst))) {
        // SSE path from original binary
        const __m128i base_indices = _mm_setr_epi32(0, 1, 2, 3);
        const __m128i mod_divisor = _mm_set1_epi32(52);
        const __m128i add_val = _mm_set1_epi32(51);
        const __m128i mask_low = _mm_set1_epi32(0xFF);

        int i = 0;
        for (; i + 7 < len; i += 8) {
            __m128i idx0 = _mm_add_epi32(_mm_set1_epi32(i), base_indices);
            // i % 52 via multiply-high trick
            __m128i q0 = _mm_srli_epi32(
                _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_mul_epi32(_mm_unpacklo_epi32(idx0, idx0), _mm_set1_epi64x(0x4EC4EC4F))),
                    _mm_castsi128_ps(_mm_mul_epi32(_mm_unpackhi_epi32(idx0, idx0), _mm_set1_epi64x(0x4EC4EC4F))),
                    0xDD)),
                4);
            __m128i rem0 = _mm_and_si128(
                _mm_sub_epi32(idx0, _mm_mullo_epi32(_mm_add_epi32(_mm_srli_epi32(q0, 31), q0), mod_divisor)),
                mask_low);
            __m128i key0 = _mm_packus_epi16(_mm_packus_epi16(rem0, rem0), _mm_setzero_si128());
            key0 = _mm_add_epi8(key0, _mm_set1_epi8(51));
            u32 src_val0 = *(const u32*)(src + i);
            *(u32*)(dst + i) = _mm_cvtsi128_si32(_mm_xor_si128(key0, _mm_cvtsi32_si128(src_val0)));

            __m128i idx1 = _mm_add_epi32(_mm_set1_epi32(i + 4), base_indices);
            __m128i q1 = _mm_srli_epi32(
                _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_mul_epi32(_mm_unpacklo_epi32(idx1, idx1), _mm_set1_epi64x(0x4EC4EC4F))),
                    _mm_castsi128_ps(_mm_mul_epi32(_mm_unpackhi_epi32(idx1, idx1), _mm_set1_epi64x(0x4EC4EC4F))),
                    0xDD)),
                4);
            __m128i rem1 = _mm_and_si128(
                _mm_sub_epi32(idx1, _mm_mullo_epi32(_mm_add_epi32(_mm_srli_epi32(q1, 31), q1), mod_divisor)),
                mask_low);
            __m128i key1 = _mm_packus_epi16(_mm_packus_epi16(rem1, rem1), _mm_setzero_si128());
            key1 = _mm_add_epi8(key1, _mm_set1_epi8(51));
            u32 src_val1 = *(const u32*)(src + i + 4);
            *(u32*)(dst + i + 4) = _mm_cvtsi128_si32(_mm_xor_si128(key1, _mm_cvtsi32_si128(src_val1)));
        }
    }

    // Scalar fallback
    for (int i = 0; i < len; i++) {
        dst[i] = src[i] ^ (u8)((i % 52) + 51);
    }
}

}
