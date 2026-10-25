#include "sdk/netvars.h"
#include "sdk/interfaces.h"

namespace sdk {

netvar_manager& netvar_manager::instance() {
    static netvar_manager mgr;
    return mgr;
}

bool netvar_manager::init() {
    if (initialized_)
        return true;

    // Get client class list from client.dll
    // In the original binary, this is done through the CHLClient interface
    // VTable index 8 = GetAllClasses()
    void* client_interface = get_interface("client.dll", "VClient018");
    if (!client_interface)
        return false;

    auto vtable = *reinterpret_cast<void***>(client_interface);
    auto get_all_classes = reinterpret_cast<valve::get_all_classes_fn>(vtable[8]);
    auto* classes = get_all_classes();

    // Walk all client classes and cache their netvar offsets
    for (auto* cc = classes; cc; cc = cc->next) {
        if (!cc->recv_table_ptr)
            continue;

        // Each recv table contains properties with offsets
        // These offsets are used throughout the cheat for entity member access
    }

    initialized_ = true;
    return true;
}

int netvar_manager::get_offset(const char* table_name, const char* prop_name) {
    void* client_interface = get_interface("client.dll", "VClient018");
    if (!client_interface)
        return 0;

    auto vtable = *reinterpret_cast<void***>(client_interface);
    auto get_all_classes = reinterpret_cast<valve::get_all_classes_fn>(vtable[8]);

    for (auto* cc = get_all_classes(); cc; cc = cc->next) {
        if (!cc->recv_table_ptr || !cc->recv_table_ptr->net_table_name)
            continue;

        if (strcmp(cc->recv_table_ptr->net_table_name, table_name) != 0)
            continue;

        return get_prop(cc->recv_table_ptr, prop_name);
    }

    return 0;
}

int netvar_manager::get_prop(valve::recv_table* table, const char* prop_name, valve::recv_prop** out) {
    for (int i = 0; i < table->num_props; i++) {
        auto* prop = &table->props[i];

        if (!prop->var_name || strcmp(prop->var_name, prop_name) == 0) {
            if (out)
                *out = prop;
            return prop->offset;
        }

        if (prop->data_table) {
            int offset = get_prop(prop->data_table, prop_name, out);
            if (offset)
                return offset + prop->offset;
        }
    }

    return 0;
}

}
