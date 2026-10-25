#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

namespace core {

// Reconstructed from sub_1019A3C0 (main parser), sub_10198990 (string parser),
// sub_101984C0 (number parser). Embedded JSON parser similar to nlohmann::json.

enum class json_type : int {
    null_value = 0,
    object_value,
    array_value,
    string_value,
    boolean_value,
    number_int,
    number_uint,
    number_float
};

class json_value {
public:
    json_value();
    json_value(bool val);
    json_value(int val);
    json_value(unsigned int val);
    json_value(float val);
    json_value(double val);
    json_value(const char* val);
    json_value(const std::string& val);
    json_value(const json_value& other);
    json_value(json_value&& other) noexcept;
    ~json_value();

    json_value& operator=(const json_value& other);
    json_value& operator=(json_value&& other) noexcept;

    json_type type() const { return type_; }
    bool is_null()    const { return type_ == json_type::null_value; }
    bool is_object()  const { return type_ == json_type::object_value; }
    bool is_array()   const { return type_ == json_type::array_value; }
    bool is_string()  const { return type_ == json_type::string_value; }
    bool is_bool()    const { return type_ == json_type::boolean_value; }
    bool is_number()  const { return type_ == json_type::number_int || type_ == json_type::number_uint || type_ == json_type::number_float; }

    // Object access
    json_value& operator[](const std::string& key);
    json_value& operator[](const char* key);
    const json_value& at(const std::string& key) const;
    bool contains(const std::string& key) const;

    // Array access
    json_value& operator[](size_t index);
    void push_back(const json_value& val);
    size_t size() const;

    // Value getters
    bool         get_bool()   const;
    int          get_int()    const;
    unsigned int get_uint()   const;
    float        get_float()  const;
    double       get_double() const;
    std::string  get_string() const;

    // Serialization
    std::string dump(int indent = -1) const;

    // Parse from string
    static json_value parse(const char* input);
    static json_value parse(const std::string& input);

private:
    json_type type_;

    union {
        bool                                              bool_val_;
        __int64                                           int_val_;
        unsigned __int64                                  uint_val_;
        double                                            float_val_;
        std::string*                                      string_val_;
        std::vector<json_value>*                          array_val_;
        std::unordered_map<std::string, json_value>*      object_val_;
    };

    void destroy();

    // Parser internals (reconstructed from sub_10198990, sub_101984C0, sub_1019A3C0)
    struct parser {
        const char* input;
        size_t      pos;
        const char* error;

        json_value parse_value();
        json_value parse_object();
        json_value parse_array();
        json_value parse_string();
        json_value parse_number();
        void       skip_whitespace();
        char       peek();
        char       advance();
    };

    void dump_impl(std::string& out, int indent, int depth) const;
};

}
