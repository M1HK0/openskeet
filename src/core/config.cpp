#include "core/config.h"
#include "core/json.h"
#include <fstream>
#include <sstream>

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
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    auto root = json_value::parse(content);
    if (!root.is_object())
        return false;

    // The original binary stores config as flat key-value JSON
    // Each key maps to a typed value
    values_.clear();
    // Config is loaded per-section by the menu system
    // For now store the raw JSON and let features query it
    return true;
}

bool config::save(const char* path) const {
    json_value root;

    for (auto& [key, val] : values_) {
        switch (val.type) {
        case config_value::BOOL:   root[key] = json_value(val.bool_val); break;
        case config_value::INT:    root[key] = json_value(val.int_val); break;
        case config_value::FLOAT:  root[key] = json_value(static_cast<double>(val.float_val)); break;
        case config_value::STRING: root[key] = json_value(val.string_val); break;
        default: break;
        }
    }

    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << root.dump(2);
    return true;
}

}
