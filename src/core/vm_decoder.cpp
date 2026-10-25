#include "core/vm_decoder.h"

namespace core {

vm_decoder& vm_decoder::instance() {
    static vm_decoder dec;
    return dec;
}

void vm_decoder::add_program(const vm_program& prog) {
    // Programs are stored for later execution during unpacking
}

// Execute a single VM operation on a byte
u8 vm_decoder::execute_op(vm_op op, u8 value, u8 key) {
    switch (op) {
        case vm_op::ROL:
            return _rotl8(value, key & 7);
        case vm_op::DEC:
            return value - 1;
        case vm_op::NOT:
            return ~value;
        case vm_op::ADD:
            return value + key;
        case vm_op::XOR:
            return value ^ key;
        case vm_op::ROR:
            return _rotr8(value, key & 7);
        case vm_op::INC:
            return value + 1;
        case vm_op::NEG:
            return static_cast<u8>(-static_cast<i32>(value));
        case vm_op::SUB:
            return value - key;
        default:
            return value;
    }
}

// Decode a data block using a VM program
bool vm_decoder::decode(u8* data, u32 len, const vm_program& prog) {
    if (!data || len == 0)
        return false;

    for (u32 i = 0; i < len; i++) {
        u8 val = data[i];
        for (const auto& insn : prog.operations) {
            val = execute_op(insn.operation, val, static_cast<u8>(insn.key));
        }
        data[i] = val;
    }

    return true;
}

}
