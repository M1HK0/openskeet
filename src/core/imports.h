#pragma once

#include "common.h"

namespace core {

// Reconstructed from sub_10192DE0
// Walks the import function table and resolves call/jmp targets
// Uses LoadLibraryA + GetProcAddress to fix up dynamic imports
bool fix_imports();

}
