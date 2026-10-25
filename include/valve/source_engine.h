#pragma once

#include "common.h"

namespace valve {

struct recv_prop;

struct recv_table {
    recv_prop* props;
    int        num_props;
    void*      decoder;
    char*      net_table_name;
    bool       initialized;
    bool       in_main_list;
};

struct recv_prop {
    char*       var_name;
    int         recv_type;
    int         flags;
    int         string_buffer_size;
    bool        inside_array;
    const void* extra_data;
    recv_prop*  array_prop;
    void*       array_length_proxy;
    void*       proxy_fn;
    void*       data_table_proxy_fn;
    recv_table* data_table;
    int         offset;
    int         element_stride;
    int         num_elements;
    const char* parent_array_prop_name;
};

struct client_class {
    void*         create_fn;
    void*         create_event_fn;
    char*         network_name;
    recv_table*   recv_table_ptr;
    client_class* next;
    int           class_id;
};

using create_interface_fn = void* (*)(const char* name, int* return_code);
using get_all_classes_fn  = client_class* (*)();

struct interface_reg {
    create_interface_fn create_fn;
    const char*         name;
    interface_reg*      next;
};

}
