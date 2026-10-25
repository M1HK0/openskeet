#include "core/json.h"
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace core {

json_value::json_value() : type_(json_type::null_value), uint_val_(0) {}
json_value::json_value(bool val) : type_(json_type::boolean_value), bool_val_(val) {}
json_value::json_value(int val) : type_(json_type::number_int), int_val_(val) {}
json_value::json_value(unsigned int val) : type_(json_type::number_uint), uint_val_(val) {}
json_value::json_value(float val) : type_(json_type::number_float), float_val_(val) {}
json_value::json_value(double val) : type_(json_type::number_float), float_val_(val) {}
json_value::json_value(const char* val) : type_(json_type::string_value), string_val_(new std::string(val)) {}
json_value::json_value(const std::string& val) : type_(json_type::string_value), string_val_(new std::string(val)) {}

json_value::json_value(const json_value& other) : type_(json_type::null_value), uint_val_(0) {
    *this = other;
}

json_value::json_value(json_value&& other) noexcept : type_(json_type::null_value), uint_val_(0) {
    *this = std::move(other);
}

json_value::~json_value() {
    destroy();
}

void json_value::destroy() {
    switch (type_) {
    case json_type::string_value: delete string_val_; break;
    case json_type::array_value:  delete array_val_;  break;
    case json_type::object_value: delete object_val_; break;
    default: break;
    }
    type_ = json_type::null_value;
    uint_val_ = 0;
}

json_value& json_value::operator=(const json_value& other) {
    if (this == &other) return *this;
    destroy();
    type_ = other.type_;
    switch (type_) {
    case json_type::boolean_value: bool_val_ = other.bool_val_; break;
    case json_type::number_int:    int_val_ = other.int_val_; break;
    case json_type::number_uint:   uint_val_ = other.uint_val_; break;
    case json_type::number_float:  float_val_ = other.float_val_; break;
    case json_type::string_value:  string_val_ = new std::string(*other.string_val_); break;
    case json_type::array_value:   array_val_ = new std::vector<json_value>(*other.array_val_); break;
    case json_type::object_value:  object_val_ = new std::unordered_map<std::string, json_value>(*other.object_val_); break;
    default: break;
    }
    return *this;
}

json_value& json_value::operator=(json_value&& other) noexcept {
    if (this == &other) return *this;
    destroy();
    type_ = other.type_;
    uint_val_ = other.uint_val_;
    other.type_ = json_type::null_value;
    other.uint_val_ = 0;
    return *this;
}

json_value& json_value::operator[](const std::string& key) {
    if (type_ != json_type::object_value) {
        destroy();
        type_ = json_type::object_value;
        object_val_ = new std::unordered_map<std::string, json_value>();
    }
    return (*object_val_)[key];
}

json_value& json_value::operator[](const char* key) {
    return (*this)[std::string(key)];
}

const json_value& json_value::at(const std::string& key) const {
    static json_value null_val;
    if (type_ != json_type::object_value) return null_val;
    auto it = object_val_->find(key);
    if (it == object_val_->end()) return null_val;
    return it->second;
}

bool json_value::contains(const std::string& key) const {
    if (type_ != json_type::object_value) return false;
    return object_val_->count(key) > 0;
}

json_value& json_value::operator[](size_t index) {
    if (type_ != json_type::array_value) {
        destroy();
        type_ = json_type::array_value;
        array_val_ = new std::vector<json_value>();
    }
    if (index >= array_val_->size())
        array_val_->resize(index + 1);
    return (*array_val_)[index];
}

void json_value::push_back(const json_value& val) {
    if (type_ != json_type::array_value) {
        destroy();
        type_ = json_type::array_value;
        array_val_ = new std::vector<json_value>();
    }
    array_val_->push_back(val);
}

size_t json_value::size() const {
    switch (type_) {
    case json_type::array_value:  return array_val_->size();
    case json_type::object_value: return object_val_->size();
    default: return 0;
    }
}

bool         json_value::get_bool()   const { return type_ == json_type::boolean_value ? bool_val_ : false; }
int          json_value::get_int()    const { return type_ == json_type::number_int ? static_cast<int>(int_val_) : 0; }
unsigned int json_value::get_uint()   const { return type_ == json_type::number_uint ? static_cast<unsigned int>(uint_val_) : 0; }
float        json_value::get_float()  const { return type_ == json_type::number_float ? static_cast<float>(float_val_) : 0.0f; }
double       json_value::get_double() const { return type_ == json_type::number_float ? float_val_ : 0.0; }
std::string  json_value::get_string() const { return type_ == json_type::string_value ? *string_val_ : std::string(); }

// === PARSER (reconstructed from sub_1019A3C0 hierarchy) ===

void json_value::parser::skip_whitespace() {
    while (input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n' || input[pos] == '\r')
        pos++;
}

char json_value::parser::peek() {
    skip_whitespace();
    return input[pos];
}

char json_value::parser::advance() {
    return input[pos++];
}

json_value json_value::parser::parse_string() {
    pos++; // skip opening "
    std::string result;
    while (input[pos] != '"') {
        if (input[pos] == '\0') {
            error = "invalid string: missing closing quote";
            return json_value();
        }
        if (input[pos] == '\\') {
            pos++;
            switch (input[pos]) {
            case '"':  result += '"';  break;
            case '\\': result += '\\'; break;
            case '/':  result += '/';  break;
            case 'b':  result += '\b'; break;
            case 'f':  result += '\f'; break;
            case 'n':  result += '\n'; break;
            case 'r':  result += '\r'; break;
            case 't':  result += '\t'; break;
            case 'u': {
                pos++;
                char hex[5] = {};
                for (int i = 0; i < 4; i++) hex[i] = input[pos++];
                result += static_cast<char>(strtol(hex, nullptr, 16));
                continue;
            }
            default:
                error = "invalid string: invalid escape character";
                return json_value();
            }
        } else {
            result += input[pos];
        }
        pos++;
    }
    pos++; // skip closing "
    return json_value(result);
}

json_value json_value::parser::parse_number() {
    const char* start = input + pos;
    char* end = nullptr;

    bool is_float = false;
    bool is_negative = (input[pos] == '-');

    // Scan to determine type
    size_t scan = pos;
    if (input[scan] == '-') scan++;
    while (input[scan] >= '0' && input[scan] <= '9') scan++;
    if (input[scan] == '.' || input[scan] == 'e' || input[scan] == 'E')
        is_float = true;

    if (is_float) {
        double val = strtod(start, &end);
        pos = end - input;
        return json_value(val);
    } else if (is_negative) {
        long long val = strtoll(start, &end, 10);
        pos = end - input;
        json_value v;
        v.type_ = json_type::number_int;
        v.int_val_ = val;
        return v;
    } else {
        unsigned long long val = strtoull(start, &end, 10);
        pos = end - input;
        json_value v;
        v.type_ = json_type::number_uint;
        v.uint_val_ = val;
        return v;
    }
}

json_value json_value::parser::parse_array() {
    pos++; // skip [
    json_value arr;
    arr.type_ = json_type::array_value;
    arr.array_val_ = new std::vector<json_value>();

    if (peek() == ']') { pos++; return arr; }

    while (true) {
        arr.array_val_->push_back(parse_value());
        if (error) return json_value();
        if (peek() == ',') { pos++; continue; }
        if (peek() == ']') { pos++; return arr; }
        error = "expected ',' or ']'";
        return json_value();
    }
}

json_value json_value::parser::parse_object() {
    pos++; // skip {
    json_value obj;
    obj.type_ = json_type::object_value;
    obj.object_val_ = new std::unordered_map<std::string, json_value>();

    if (peek() == '}') { pos++; return obj; }

    while (true) {
        if (peek() != '"') { error = "expected string key"; return json_value(); }
        json_value key = parse_string();
        if (error) return json_value();

        if (peek() != ':') { error = "expected ':'"; return json_value(); }
        pos++;

        json_value val = parse_value();
        if (error) return json_value();

        (*obj.object_val_)[key.get_string()] = std::move(val);

        if (peek() == ',') { pos++; continue; }
        if (peek() == '}') { pos++; return obj; }
        error = "expected ',' or '}'";
        return json_value();
    }
}

json_value json_value::parser::parse_value() {
    char c = peek();
    switch (c) {
    case '"': return parse_string();
    case '{': return parse_object();
    case '[': return parse_array();
    case 't':
        if (strncmp(input + pos, "true", 4) == 0) { pos += 4; return json_value(true); }
        error = "invalid value"; return json_value();
    case 'f':
        if (strncmp(input + pos, "false", 5) == 0) { pos += 5; return json_value(false); }
        error = "invalid value"; return json_value();
    case 'n':
        if (strncmp(input + pos, "null", 4) == 0) { pos += 4; return json_value(); }
        error = "invalid value"; return json_value();
    default:
        if (c == '-' || (c >= '0' && c <= '9')) return parse_number();
        error = "unexpected character";
        return json_value();
    }
}

json_value json_value::parse(const char* input) {
    parser p{ input, 0, nullptr };
    json_value result = p.parse_value();
    if (p.error) return json_value();
    return result;
}

json_value json_value::parse(const std::string& input) {
    return parse(input.c_str());
}

// === SERIALIZATION ===

static void write_string(std::string& out, const std::string& s) {
    out += '"';
    for (char c : s) {
        switch (c) {
        case '"':  out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default: out += c; break;
        }
    }
    out += '"';
}

void json_value::dump_impl(std::string& out, int indent, int depth) const {
    std::string pad;
    std::string pad_inner;
    if (indent > 0) {
        pad = std::string(depth * indent, ' ');
        pad_inner = std::string((depth + 1) * indent, ' ');
    }

    switch (type_) {
    case json_type::null_value:    out += "null"; break;
    case json_type::boolean_value: out += bool_val_ ? "true" : "false"; break;
    case json_type::number_int: {
        char buf[32];
        snprintf(buf, sizeof(buf), "%lld", int_val_);
        out += buf;
        break;
    }
    case json_type::number_uint: {
        char buf[32];
        snprintf(buf, sizeof(buf), "%llu", uint_val_);
        out += buf;
        break;
    }
    case json_type::number_float: {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", float_val_);
        out += buf;
        break;
    }
    case json_type::string_value:
        write_string(out, *string_val_);
        break;
    case json_type::array_value: {
        out += '[';
        bool first = true;
        for (auto& v : *array_val_) {
            if (!first) out += ',';
            if (indent > 0) out += '\n' + pad_inner;
            v.dump_impl(out, indent, depth + 1);
            first = false;
        }
        if (indent > 0 && !array_val_->empty()) out += '\n' + pad;
        out += ']';
        break;
    }
    case json_type::object_value: {
        out += '{';
        bool first = true;
        for (auto& [k, v] : *object_val_) {
            if (!first) out += ',';
            if (indent > 0) out += '\n' + pad_inner;
            write_string(out, k);
            out += ':';
            if (indent > 0) out += ' ';
            v.dump_impl(out, indent, depth + 1);
            first = false;
        }
        if (indent > 0 && !object_val_->empty()) out += '\n' + pad;
        out += '}';
        break;
    }
    }
}

std::string json_value::dump(int indent) const {
    std::string out;
    dump_impl(out, indent, 0);
    return out;
}

}
