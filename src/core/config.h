#pragma once

#include "common.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

// The original binary embeds a full JSON parser (nlohmann::json-like)
// for configuration handling. Rather than reconstructing the parser
// byte-for-byte, we use a minimal config system matching the original's
// data model.

struct config_value {
    enum type_t { NONE, BOOL, INT, FLOAT, STRING };

    type_t type = NONE;
    bool   bool_val = false;
    int    int_val = 0;
    float  float_val = 0.f;
    std::string string_val;
};

class config {
public:
    static config& instance();

    void set(const std::string& key, bool val);
    void set(const std::string& key, int val);
    void set(const std::string& key, float val);
    void set(const std::string& key, const std::string& val);

    bool get_bool(const std::string& key, bool def = false) const;
    int get_int(const std::string& key, int def = 0) const;
    float get_float(const std::string& key, float def = 0.f) const;
    std::string get_string(const std::string& key, const std::string& def = "") const;

    bool load(const char* path);
    bool save(const char* path) const;

private:
    std::unordered_map<std::string, config_value> values_;
};

}
