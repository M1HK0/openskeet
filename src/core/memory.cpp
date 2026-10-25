#include "core/memory.h"
#include "core/context.h"

namespace core {

// Reconstructed from sub_10192DC0
// Copies packed data into the allocated buffer
bool unpack_payload(void* base, const void* packed_data, u32 size) {
    if (!base || !packed_data)
        return false;

    memcpy(base, packed_data, size);
    return true;
}

}
