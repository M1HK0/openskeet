#pragma once

#include "common.h"
#include "valve/source_engine.h"

namespace sdk {

// Reconstructed from sub_10192F70 (partial) and sub_10191130
// Walks client.dll's RecvTable tree to resolve netvar offsets at runtime
class netvar_manager {
public:
    static netvar_manager& instance();

    bool init();
    int get_offset(const char* table_name, const char* prop_name);

private:
    int get_prop(valve::recv_table* table, const char* prop_name, valve::recv_prop** out = nullptr);

    bool initialized_ = false;
};

}
