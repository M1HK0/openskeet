#pragma once

#include "common.h"

namespace core {

// Reconstructed from sub_101942A0
// Creates a suspended thread, allocates 1MB RWX memory,
// sets up NtCreateSection-based execution, hijacks thread context
bool thread_hijack(void* ctx);

}
