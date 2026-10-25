#pragma once

#include "common.h"
#include <ostream>

namespace core {

// Reconstructed from sub_1018F410
// Wrapper around std::ostream::operator<< with width/fill handling
std::ostream& console_print(std::ostream& os, const char* str);
std::ostream& console_print(std::ostream& os, int value);

}
