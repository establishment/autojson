#include "JSON.hpp"

#include <fstream>
#include <ostream>
#include <typeinfo>

#include "Parse.hpp"
#include "Error.hpp"

namespace autojson {

/// Generic

JSON::JSON(JSONType type)
    : type(type), content(nullptr) {
    if (this->type == JSONType::STRING) {
        this->content = new std::string("");
    } else if (this->type == JSONType::PRIMITIVE) {
        this->content = new std::string("0");
    } else if (this->type == JSONType::VECTOR) {
        this->content = new std::vector<JSON>;
    } else {
        this->content = new std::map<std::string, JSON>;
    }
}

JSON::~JSON() {
    if (this->type == JSONType::PRIMITIVE || this->type == JSONType::STRING) {
        delete (std::string*)(this->content);
    } else if (this->type == JSONType::VECTOR) {
        delete (std::vector<JSON>*)(this->content);
    } else if (this->type == JSONType::OBJECT) {
        delete (std::map<std::string, JSON>*)(this->content);
    }
}

JSON::JSON(const JSON &rhs) {
    *this = rhs;
}

JSON& JSON::operator=(const JSON &rhs) {
    this->type = rhs.type;
    if (rhs.type == JSONType::PRIMITIVE || rhs.type == JSONType::STRING) {
        this->content = new std::string(*(std::string*)(rhs.content));
    } else if (rhs.type == JSONType::VECTOR) {
        this->content = new std::vector<JSON>(*(std::vector<JSON>*)(rhs.content));
    } else if (rhs.type == JSONType::OBJECT) {
        this->content = new std::map<std::string, JSON>(*(std::map<std::string, JSON>*)(rhs.content));
    } else {
        this->content = nullptr;
    }

    return *this;
}

JSON::JSON(JSON &&rhs) {
    *this = std::move(rhs);
}

JSON& JSON::operator=(JSON &&rhs) {
    this->type = rhs.type;
    this->content = rhs.content;
    rhs.content = nullptr;
    rhs.type = JSONType::INVALID;
    return *this;
}

JSON::JSON(std::initializer_list<JSON> list) {
    auto vp = new std::vector<JSON>;
    auto &v = *vp;

    bool isObject = true;
    for (auto& itr : list) {
        auto p = JSON(itr);
        if (p.type == JSONType::VECTOR and p.size() == 2 and p[0].type == JSONType::STRING) {
        } else {
            isObject = false;
        }
        v.emplace_back(std::move(p));
    }

    if (isObject) {
        auto mp = new std::map<std::string, JSON>;
        auto& m = *mp;
        for (auto& itr : v) {
            m[itr[0]] = itr[1];
        }

        this->type = JSONType::OBJECT;
        this->content = mp;
        delete vp;
    } else {
        this->type = JSONType::VECTOR;
        this->content = vp;
    }
}

JSON& JSON::operator||(std::initializer_list<JSON> list) {
    return *this | JSON(list);
}

JSON& JSON::operator|(std::initializer_list<JSON> list) {
    return *this | JSON(list);
}

/// Parser for generic JSON
JSON JSON::parse(const char*& content) {
    auto CurrentChar = [&]() {
        return *content;
    };

    auto Eof = [&]() {
        return (*content == '\0');
    };

    SkipWhitespace(content, ",");

    if (Eof()) {
        ParseError("Unexpected EOF", content);
    }

    if (CurrentChar() == '{') {
        auto r =  parseObject(content);
        SkipWhitespace(content, ",");
        return r;
    } else if (CurrentChar() == '[') {
        auto r = parseVector(content);
        SkipWhitespace(content, ",");
        return r;
    } else if (CurrentChar() == '\"' or CurrentChar() == '\'') {
        auto word = ParseString(content);
        SkipWhitespace(content, ",");
        return JSON(JSONType::STRING, new std::string(word));
    } else {
        auto word = ParseWord(content);
        JSON j;
        SkipWhitespace(content, ",");
        return JSON(JSONType::PRIMITIVE, new std::string(word));
    }
}

JSON JSON::parse(const std::string& content) {
    const char* content_p = content.c_str();
    return parse(content_p);
}

JSON JSON::readFromFile(const std::string& file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string file_information;

    if (fin) {
        fin.seekg(0, fin.end);
        file_information.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(& file_information[0], file_information.size());
        fin.close();
    } else {
        JSONError("Error while reading");
    }

    const char* content_p = file_information.c_str();
    return parse(content_p);
}

void JSON::stringify(StringifyPart part) const {
    if (this->type == JSONType::PRIMITIVE) {
        this->stringifyPrimitive(part);
    } else if (this->type == JSONType::STRING) {
        this->stringifyString(part);
    } else if (this->type == JSONType::VECTOR) {
        this->stringifyVector(part);
    } else if (this->type == JSONType::OBJECT) {
        this->stringifyObject(part);
    } else {
        JSONError("Cannot Stringify an invalid JSON");
    }
}

std::string JSON::stringify(int shrink) const {
    std::string result;
    StringifyPart part(result, 0, shrink, false);
    this->stringify(part);
    return result;
}

void JSON::checkType(JSONType type) {
    if (type != this->type) {
        if (this->type == JSONType::INVALID) {
            *this = JSON(type);
        } else {
            std::string message = "Wrong type. Expected ";
            message += std::to_string(this->type);
            message += " but got ";
            message += std::to_string(type);
            JSONError(message);
        }
    }
}

void JSON::checkType(JSONType type) const {
    if (type != this->type) {
        JSONError("Wrong type. Const checktype");
    }
}

std::ostream& operator<<(std::ostream& stream, const JSON& json) {
    stream << json.stringify();
    return stream;
}

/// JSON Primitive
void JSON::stringifyPrimitive(StringifyPart part) const {
    part.indent();
    part.result += *((std::string*)content);
}

JSON::operator bool() const {
    this->checkType(JSONType::PRIMITIVE);
    auto& txt = *(std::string*)(this->content);
    bool val = false;
    if (txt == "true" || txt == "1") {
        val = true;
    }
    return val;
}

JSON::operator int() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stoi(*(std::string*)(this->content));
}

JSON::operator long() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stol(*(std::string*)(this->content));
}

JSON::operator unsigned long() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stoul(*(std::string*)(this->content));
}

JSON::operator long long() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stoll(*(std::string*)(this->content));
}

JSON::operator unsigned long long() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stoull(*(std::string*)(this->content));
}

JSON::operator float() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stof(*(std::string*)(this->content));
}

JSON::operator double() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stod(*(std::string*)(this->content));
}

JSON::operator long double() const {
    this->checkType(JSONType::PRIMITIVE);
    return std::stold(*(std::string*)(this->content));
}

bool JSON::isInteger() const {
    return this->isReal() and ((std::string*)(this->content))->find('.') == std::string::npos;
}

bool JSON::isReal() const {
    this->checkType(JSONType::PRIMITIVE);
    try {
        std::stod(*(std::string*)(this->content));
        return true;
    } catch(...) {
        return false;
    }   
}

bool JSON::isBool() const {
    if (this->type != JSONType::PRIMITIVE) {
        return false;
    }

    auto& txt = *(std::string*)(this->content);
    if (txt == "true" || txt == "false") {
        return true;
    }

    return false;
}

bool JSON::isHex() const {
    if (this->type != JSONType::STRING) {
        return false;
    }

    auto& txt = *(std::string*)(this->content);
    bool ok = true;

    for (auto c : txt) {
        if ('0' <= c and c <= '9') {
        } else if ('A' <= c and c <= 'F') {
        } else if ('a' <= c and c <= 'f') {
        } else {
            ok = false;
            break;
        }
    }

    return ok;
}

bool JSON::isHex16() const {
    return this->isHex() and ((std::string*)(this->content))->size() == 16;
}

bool JSON::isHex32() const {
    return this->isHex() and ((std::string*)(this->content))->size() == 32;
}

bool JSON::isHex64() const {
    return this->isHex() and ((std::string*)(this->content))->size() == 64;
}

bool JSON::isHex128() const {
    return this->isHex() and ((std::string*)(this->content))->size() == 128;
}

bool JSON::isString() const {
    return this->type == JSONType::STRING;
}

bool JSON::isArray() const {
    return this->type == JSONType::VECTOR;
}

bool JSON::isInteger(const std::string& key) const {
    return this->exists(key) && this->get(key).isInteger();
}

bool JSON::isReal(const std::string& key) const {
    return this->exists(key) && this->get(key).isReal();
}

bool JSON::isBool(const std::string& key) const {
    return this->exists(key) && this->get(key).isBool();
}

bool JSON::isHex(const std::string& key) const {
    return this->exists(key) && this->get(key).isHex();
}

bool JSON::isHex16(const std::string& key) const {
    return this->exists(key) && this->get(key).isHex16();
}

bool JSON::isHex32(const std::string& key) const {
    return this->exists(key) && this->get(key).isHex32();
}

bool JSON::isHex64(const std::string& key) const {
    return this->exists(key) && this->get(key).isHex64();
}

bool JSON::isHex128(const std::string& key) const {
    return this->exists(key) && this->get(key).isHex128();
}

bool JSON::isString(const std::string& key) const {
    return this->exists(key) && this->get(key).isString();
}

bool JSON::isArray(const std::string& key) const {
    return this->exists(key) && this->get(key).isArray();
}

bool JSON::exists(const std::string& key) const {
    this->checkType(JSONType::OBJECT);

    auto& m = *(std::map<std::string, JSON>*)(this->content);
    return m.count(key);
}

void JSON::set(const std::string& key, const JSON& value) {
    this->checkType(JSONType::OBJECT);
    auto& m = *(std::map<std::string, JSON>*)(this->content);
    m[key] = value;
}

const JSON& JSON::getOrSet(const std::string& key, const JSON& defaultValue) {
    this->checkType(JSONType::OBJECT);
    auto& m = *(std::map<std::string, JSON>*)(this->content);
    auto itr = m.find(key);
    if (itr != m.end()) {
        return itr->second;
    }

    m[key] = defaultValue;

    return defaultValue;
}

const JSON& JSON::get(const std::string& key, const JSON& defaultValue) const {
    if (JSONType::OBJECT != this->type) {
        return defaultValue;
    }

    auto& m = *(std::map<std::string, JSON>*)(this->content);
    auto itr = m.find(key);

    if (itr != m.end()) {
        this->checkType(JSONType::OBJECT);
        return itr->second;
    } else {
        return defaultValue;
    }
}

JSON::operator JSON() {
    return *this;
}

JSON::operator JSON() const {
    return *this;
}

void JSON::stringifyString(StringifyPart part) const {
    part.indent();
    part.result += "\"" + EscapeKeys(*((std::string*)content)) + "\"";
}

JSON::operator std::string() {
    if (this->type == JSONType::STRING) {
        return std::string(*(std::string*)(this->content));
    } else {
        return this->stringify();
    }
}

JSON::operator std::string() const {
    if (this->type == JSONType::STRING) {
        return std::string(*(std::string*)(this->content));
    } else {
        return this->stringify();
    }
}

void JSON::stringifyVector(StringifyPart part) const {
    this->checkType(JSONType::VECTOR);

    part.indent();
    part.result += "[";
    part.endLine();

    std::vector<JSON>* m = (std::vector<JSON>*)content;
    bool atLeastOneElement = false;
    for (auto itr : *m) {
        atLeastOneElement = true;

        itr.stringify(part.increaseIndent());
        part.result += ",";
        part.endLine();
    }

    if (atLeastOneElement) {
        part.deleteComma();
    }

    part.indent();
    part.result += "]";
}

JSON JSON::parseVector(const char *&content) {
    auto v = new std::vector<JSON>;

    JSON j;
    j.type = JSONType::VECTOR;
    j.content = v;

    content++; // skip [

    while (1) {
        SkipWhitespace(content, ",");
        if (*content == ']') {
            content++;
            break;
        }

        v->emplace_back(JSON::parse(content));
    }

    return j;
}

JSON& JSON::operator[](int key) {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    if (0 <= key and key < (int)v.size()) {
        return v[key];
    } else {
        std::string message;
        message += "Vector out of bounds ";
        message += std::to_string(key);
        message += ":";
        message += std::to_string(v.size());
        JSONError(message);
        return v[key];
    }
}

std::vector<JSON>::iterator JSON::begin() {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    return v.begin();
}

std::vector<JSON>::iterator JSON::end() {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    return v.end();
}

std::vector<JSON>::const_iterator JSON::begin() const {
    this->checkType(JSONType::VECTOR);
    const auto& v = *(std::vector<JSON>*)(this->content);
    return v.begin();
}

std::vector<JSON>::const_iterator JSON::end() const {
    this->checkType(JSONType::VECTOR);
    const auto& v = *(std::vector<JSON>*)(this->content);
    return v.end();
}

void JSON::pop_back() {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    v.pop_back();
}

int JSON::size() const {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    return v.size();
}

void JSON::stringifyObject(StringifyPart part) const {
    this->checkType(JSONType::OBJECT);

    part.indent();
    part.result += "{";
    part.endLine();

    std::map<std::string, JSON>* m = (std::map<std::string, JSON>*)content;

    bool atLeastOneElement = false;
    for (auto itr : *m) {
        atLeastOneElement = true;
        part.indent(1);
        part.result += "\"" + itr.first + "\"";
        part.result += ":";

        itr.second.stringify(part.increaseIndent().isContinueLine());
        part.result += ",";
        part.endLine();
    }

    if (atLeastOneElement) {
        part.deleteComma();
    }

    part.indent();
    part.result += "}";
}

JSON JSON::parseObject(const char*& content) {
    auto m = new std::map<std::string, JSON>;

    JSON j;
    j.type = JSONType::OBJECT;
    j.content = m;

    content++; // skip {

    while (1) {
        SkipWhitespace(content, ",");
        if (*content == '}') {
            content++;
            break;
        }

        auto word = ParseString(content);
        SkipWhitespace(content, ""); // get to :
        if (*content != ':') {
            ParseError("Expected ':'. Got something else", content);
        }

        content++;
        (*m)[word] = parse(content);
    }

    return j;
}

JSON& JSON::operator[](const std::string& key) {
    this->checkType(JSONType::OBJECT);
    auto& m = *(std::map<std::string, JSON>*)(this->content);
    return m[key];
}

JSON& JSON::operator[](const char* key) {
    return operator[](std::string(key));
}

StringifyPart StringifyPart::increaseIndent() {
    return StringifyPart(this->result, this->indentLevel + 1, this->shrink, this->continueLine);
}

StringifyPart StringifyPart::isContinueLine() {
    return StringifyPart(this->result, this->indentLevel, this->shrink, true);
}

StringifyPart& StringifyPart::indent(int bonus) {
    if (this->continueLine) {
        this->continueLine = false;
        return *this;
    }

    if (this->shrink) {
        this->result += "";
    } else {
        this->result += std::string((this->indentLevel + bonus) * 4, ' ');
    }
    return *this;
}

StringifyPart& StringifyPart::endLine(){
    this->continueLine = false;
    if (not this->shrink) {
        result += "\n";
    }
    return *this;
}

StringifyPart& StringifyPart::deleteComma() {
    std::string erased = "";
    while (result.size() and result.back() != ',') {
        erased += result.back();
        result.pop_back();
    }

    if (result.size()) {
        result.pop_back();
    }

    while (erased.size()) {
        result += erased.back();
        erased.pop_back();
    }

    return *this;
}

}  // namespace autojson
