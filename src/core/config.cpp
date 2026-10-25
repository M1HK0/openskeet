#include "core/config.h"
#include <fstream>

namespace core {

config& config::instance() {
    static config cfg;
    return cfg;
}

void config::set(const std::string& key, bool val) {
    values_[key] = {config_value::BOOL, val, 0, 0.f, ""};
}

void config::set(const std::string& key, int val) {
    values_[key] = {config_value::INT, false, val, 0.f, ""};
}

void config::set(const std::string& key, float val) {
    values_[key] = {config_value::FLOAT, false, 0, val, ""};
}

void config::set(const std::string& key, const std::string& val) {
    values_[key] = {config_value::STRING, false, 0, 0.f, val};
}

bool config::get_bool(const std::string& key, bool def) const {
    auto it = values_.find(key);
    if (it == values_.end() || it->second.type != config_value::BOOL)
        return def;
    return it->second.bool_val;
}

int config::get_int(const std::string& key, int def) const {
    auto it = values_.find(key);
    if (it == values_.end() || it->second.type != config_value::INT)
        return def;
    return it->second.int_val;
}

float config::get_float(const std::string& key, float def) const {
    auto it = values_.find(key);
    if (it == values_.end() || it->second.type != config_value::FLOAT)
        return def;
    return it->second.float_val;
}

std::string config::get_string(const std::string& key, const std::string& def) const {
    auto it = values_.find(key);
    if (it == values_.end() || it->second.type != config_value::STRING)
        return def;
    return it->second.string_val;
}

bool config::load(const char* path) {
    // TODO: JSON parser matching the original binary's embedded parser
    // The original uses a custom JSON parser (sub_10198990, sub_1019A3C0, etc.)
    return false;
}

bool config::save(const char* path) const {
    // TODO: JSON serialization
    return false;
}

}
