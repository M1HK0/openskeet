#pragma once

#include "common.h"
#include <vector>
#include <string>

namespace core {

// Reconstructed from sub_10192150
// Custom virtual machine for data decryption/deobfuscation
// Uses operations: ROL, DEC, NOT, ADD, XOR, ROR, INC, NEG, SUB
enum class vm_op : u8 {
    ROL = 0,
    DEC = 1,
    NOT = 2,
    // op 3 = unknown (sub_10196440 instead of sub_10196410)
    ADD = 4,
    XOR = 5,
    ROR = 6,
    INC = 7,
    NEG = 8,
    SUB = 9,
};

struct vm_instruction {
    u64     address;
    u32     key;
    vm_op   operation;
};

struct vm_program {
    std::vector<vm_instruction> operations;
    u8      size;
    u64     value;
    u64     vip;
};

class vm_decoder {
public:
    static vm_decoder& instance();

    void add_program(const vm_program& prog);
    bool decode(u8* data, u32 len, const vm_program& prog);

private:
    u8 execute_op(vm_op op, u8 value, u8 key);
};

}
