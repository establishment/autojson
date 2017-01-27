#pragma once
#include <fstream>
#include <ostream>

#include "error.hpp"
#include "parse.hpp"
#include "json.hpp"

namespace AutoJson {

/// Generic

Json::Json(JsonType type) : type(type), content(nullptr) {
    if (type == JsonType::STRING) {
        this->content = new std::string("");
    } else if (type == JsonType::PRIMITIVE) {
        this->content = new std::string("0");
    } else if (type == JsonType::VECTOR) {
        this->content = new std::vector<Json>;
    } else {
        this->content = new std::map<std::string, Json>;
    }
}


Json::~Json() {
    if (this->type == JsonType::PRIMITIVE || this->type == JsonType::STRING) {
        delete (std::string*)(this->content);
    } else if (this->type == JsonType::VECTOR) {
        delete (std::vector<Json>*)(this->content);
    } else if (this->type == JsonType::OBJECT) {
        delete (std::map<std::string, Json>*)(this->content);
    }
}

Json::Json(const Json& rhs) {
    *this = rhs;
} 

Json& Json::operator=(const Json& rhs) {
    this->type = rhs.type;
    if (rhs.type == JsonType::PRIMITIVE || rhs.type == JsonType::STRING) {
        this->content = new std::string(*(std::string*)(rhs.content));
    } else if (rhs.type == JsonType::VECTOR) {
        this->content = new std::vector<Json>(*(std::vector<Json>*)(rhs.content));
    } else if (rhs.type == JsonType::OBJECT) {
        this->content = new std::map<std::string, Json>(*(std::map<std::string, Json>*)(rhs.content));
    } else {
        this->content = nullptr;
    }

    return *this;
}

Json::Json(Json&& rhs) {
    *this = std::move(rhs);
}

Json& Json::operator=(Json&& rhs) {
    this->type = rhs.type;
    this->content = rhs.content;
    rhs.content = nullptr;
    rhs.type = JsonType::INVALID;
    return *this;
}

/// operators | and || for giving a default value to invaild JSONs
template<typename T>
Json& Json::operator||(const T& rhs) {
    if (this->type == JsonType::INVALID) {
        *this = Json(rhs);
    }
    return *this;
}

template<typename T>
Json& Json::operator||(T&& rhs) {
    if (this->type == JsonType::INVALID) {
        *this = Json(std::move(rhs));
    }
    return *this;
}

Json& Json::operator||(std::initializer_list<Json> list) {
    return *this | Json(list);
}

template<typename T>
Json& Json::operator|(const T& rhs) {
    return *this || rhs;
}

template<typename T>
Json& Json::operator|(T&& rhs) {
    return *this || std::move(rhs);
}

Json& Json::operator|(std::initializer_list<Json> list) {
    return *this | Json(list);
}

/// deep copy pointers

template<typename T>
Json::Json(const T* x) : type(INVALID), content(nullptr) {
    if (x != nullptr) {
        *this = Json(*x);
    }
}

template<typename T>
Json::Json(T* x) : type(INVALID), content(nullptr) {
    if (x != nullptr) {
        *this = Json(*x);
    }
}

/// small STL constructors

template<typename Type>
Json::Json(const std::vector<Type>& els) : type(JsonType::VECTOR) {
    auto v = new std::vector<Json>;

    for (const Type& itr : els) {
        v->emplace_back(Json(itr));
    }

    this->content = v;    
}

template<typename T>
Json::Json(const std::map<std::string, T>& els) : type(JsonType::OBJECT) {
    auto m = new std::map<std::string, Json>;

    for (const auto& itr : els) {
        (*m)[itr.first] = Json(itr.second);
    }
    this->content = m;
}

/// std::initialiser_list

Json::Json(std::initializer_list<Json> list) {
    auto vp = new std::vector<Json>;
    auto& v = *vp;

    bool is_object = true;
    for (auto& itr : list) {
        auto p = Json(itr);
        if (p.type == JsonType::VECTOR and p.size() == 2 and p[0].type == JsonType::STRING) {
        } else {
            is_object = false;
        }
        v.emplace_back(std::move(p));
    }

    if (is_object) {
        auto mp = new std::map<std::string, Json>;
        auto& m = *mp;
        for (auto& itr : v) {
            m[itr[0]] = itr[1];
        }

        this->type = JsonType::OBJECT;
        this->content = mp;
    } else {
        this->type = JsonType::VECTOR;
        this->content = vp;
    }
}

/// Parser for generic JSON

Json Json::Parse(const char*& content) {
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
        auto r =  ParseObject(content);
        SkipWhitespace(content, ",");
        return r;
    } else if (CurrentChar() == '[') {
        auto r = ParseVector(content);
        SkipWhitespace(content, ",");
        return r;
    } else if (CurrentChar() == '\"' or CurrentChar() == '\'') {
        auto word = ParseString(content);
        SkipWhitespace(content, ",");
        return Json(JsonType::STRING, new std::string(word));
    } else {
        auto word = ParseWord(content);
        Json j;
        SkipWhitespace(content, ",");
        return Json(JsonType::PRIMITIVE, new std::string(word));
    }       
}

Json Json::Parse(const std::string& content) {
    const char* content_p = content.c_str();
    return Parse(content_p);
}

Json Json::ReadFromFile(const std::string& file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string file_information;

    if (fin) {
        fin.seekg(0, fin.end);
        file_information.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(& file_information[0], file_information.size());
        fin.close();
    } else {
        JsonError("Error while reading");
    }

    const char* content_p = file_information.c_str();
    return Parse(content_p);
}

void Json::Stringify(StringifyPart part) const {
    if (type == JsonType::PRIMITIVE) {
        StringifyPrimitive(part);
    } else if (type == JsonType::STRING) {
        StringifyString(part);
    } else if (type == JsonType::VECTOR) {
        StringifyVector(part);
    } else if (type == JsonType::OBJECT) {
        StringifyObject(part);
    } else {
        JsonError("Invalid Json");
    }
}

std::string Json::Stringify(int shrink) const {
    std::string result;
    StringifyPart part(result, 0, shrink, false);
    Stringify(part);
    return result;
}

void Json::CheckType(JsonType type) {
    if (type != this->type) {
        if (this->type == JsonType::INVALID) {
            *this = Json(type); 
        } else {
            std::string message = "Wrong type. Expected ";
            message += std::to_string(this->type);
            message += " but got ";
            message += std::to_string(type);
            JsonError(message);
        }
    }
}

void Json::CheckType(JsonType type) const {
    if (type != this->type) {
        JsonError("Wrong type. Const checktype");
    }
}

std::ostream& operator<<(std::ostream& stream, const Json& json) {
    stream << json.Stringify();
    return stream;
}

/// Json Primitive
void Json::StringifyPrimitive(StringifyPart part) const {
    part.Indent();
    part.result += *((std::string*)content);
}


Json::operator int() {
    CheckType(JsonType::PRIMITIVE);
    return std::stoi(*(std::string*)(this->content));
}

Json::operator int() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stoi(*(std::string*)(this->content));
}


Json::operator long() {
    CheckType(JsonType::PRIMITIVE);
    return std::stol(*(std::string*)(this->content));
}

Json::operator long() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stol(*(std::string*)(this->content));
}


Json::operator unsigned long() {
    CheckType(JsonType::PRIMITIVE);
    return std::stoul(*(std::string*)(this->content));
}

Json::operator unsigned long() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stoul(*(std::string*)(this->content));
}


Json::operator long long() {
    CheckType(JsonType::PRIMITIVE);
    return std::stoll(*(std::string*)(this->content));
}

Json::operator long long() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stoll(*(std::string*)(this->content));
}


Json::operator unsigned long long() {
    CheckType(JsonType::PRIMITIVE);
    return std::stoull(*(std::string*)(this->content));
}

Json::operator unsigned long long() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stoull(*(std::string*)(this->content));
}


Json::operator float() {
    CheckType(JsonType::PRIMITIVE);
    return std::stof(*(std::string*)(this->content));
}

Json::operator float() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stof(*(std::string*)(this->content));
}


Json::operator double() {
    CheckType(JsonType::PRIMITIVE);
    return std::stod(*(std::string*)(this->content));
}

Json::operator double() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stod(*(std::string*)(this->content));
}


Json::operator long double() {
    CheckType(JsonType::PRIMITIVE);
    return std::stold(*(std::string*)(this->content));
}

Json::operator long double() const {
    CheckType(JsonType::PRIMITIVE);
    return std::stold(*(std::string*)(this->content));
}

/// Json String

void Json::StringifyString(StringifyPart part) const {
    part.Indent();
    part.result += "\"" + EscapeKeys(*((std::string*)content)) + "\"";
}

Json::operator std::string() {
    if (this->type == JsonType::STRING) {
        return std::string(*(std::string*)(this->content));
    } else {
        return this->Stringify();
    }
}

Json::operator std::string() const {
    if (this->type == JsonType::STRING) {
        return std::string(*(std::string*)(this->content));
    } else {
        return this->Stringify();
    }
}

/// Json Vector

void Json::StringifyVector(StringifyPart part) const {
    CheckType(JsonType::VECTOR);

    part.Indent();
    part.result += "[";
    part.EndLine();

    std::vector<Json>* m = (std::vector<Json>*)content;
    bool at_least_one_element = false;
    for (auto itr : *m) {
        at_least_one_element = true;

        itr.Stringify(part.IncreaseIndent());
        part.result += ",";
        part.EndLine();
    }

    if (at_least_one_element) {
        part.DeleteComma();
    }

    part.Indent();
    part.result += "]";
}

Json Json::ParseVector(const char*& content) { 
    auto v = new std::vector<Json>;

    Json j;
    j.type = JsonType::VECTOR;
    j.content = v;

    content++; // skip [

    while (1) {
        SkipWhitespace(content, ",");
        if (*content == ']') {
            content++;
            break;
        }

        v->emplace_back(Json::Parse(content));
    }

    return j;
}

Json& Json::operator[](int key) {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    if (0 <= key and key < (int)v.size()) {
        return v[key];
    } else {
        std::string message;
        message += "Vector out of bounds ";
        message += std::to_string(key);
        message += ":";
        message += std::to_string(v.size());
        JsonError(message);
        return v[key];
    }
}

std::vector<Json>::iterator Json::begin() {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    return v.begin();
}

std::vector<Json>::iterator Json::end() {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    return v.end();
}

std::vector<Json>::const_iterator Json::begin() const {
    CheckType(JsonType::VECTOR);
    const auto& v = *(std::vector<Json>*)(this->content);
    return v.begin();
}

std::vector<Json>::const_iterator Json::end() const {
    CheckType(JsonType::VECTOR);
    const auto& v = *(std::vector<Json>*)(this->content);
    return v.end();
}

template<typename Type>
Json::operator std::vector<Type>() {
    CheckType(JsonType::VECTOR);
    std::vector<Type> v;
    for (Json& itr : (*this)) {
        v.emplace_back(Type(itr));
    }
    return v;
}

template<typename Type>
Json::operator std::vector<Type>() const {
    CheckType(JsonType::VECTOR);
    std::vector<Type> v;
    for (const Json& itr : (*this)) {
        v.emplace_back(Type(itr));
    }
    return v;
}

template<typename Type>
Json& Json::push_back(const Type& rhs) {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    v.push_back(rhs);
    return v.back();
}

template<typename Type>
Json& Json::emplace_back(Type&& rhs) {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    v.emplace_back(rhs);
    return v.back();
}

void Json::pop_back() {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    v.pop_back();
}

int Json::size() const {
    CheckType(JsonType::VECTOR);
    auto& v = *(std::vector<Json>*)(this->content);
    return v.size();
}

/// Json Object

void Json::StringifyObject(StringifyPart part) const {
    CheckType(JsonType::OBJECT);

    part.Indent();
    part.result += "{";
    part.EndLine();

    std::map<std::string, Json>* m = (std::map<std::string, Json>*)content;

    bool at_least_one_element = false;
    for (auto itr : *m) {
        at_least_one_element = true;
        part.Indent(1);
        part.result += "\"" + itr.first + "\"";
        part.result += ":";

        itr.second.Stringify(part.IncreaseIndent().ContinueLine());
        part.result += ",";
        part.EndLine();
    }

    if (at_least_one_element) {
        part.DeleteComma();
    }

    part.Indent();
    part.result += "}";
}

Json Json::ParseObject(const char*& content) {
    auto m = new std::map<std::string, Json>;

    Json j;
    j.type = JsonType::OBJECT;
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
        (*m)[word] = Parse(content);
    }

    return j;
}

Json& Json::operator[](const std::string& key) {
    CheckType(JsonType::OBJECT);
    auto& m = *(std::map<std::string, Json>*)(this->content);
    return m[key];
}

Json& Json::operator[](const char* key) {
    return operator[](std::string(key));
}

template<typename Type>
Json::operator std::map<std::string, Type>() {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, Json>*)(content);
    for (auto& itr : mp) {
        m[itr.first] = Type(itr.second);
    }
    return m;
}

template<typename Type>
Json::operator std::map<std::string, Type>() const {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, Json>*)(content);
    for (const auto& itr : mp) {
        m[itr.first] = Type(itr.second);
    }
    return m;
}

/// StringifyPart
StringifyPart StringifyPart::IncreaseIndent() {
    return StringifyPart(result, indent_level + 1, shrink, continue_line);
}

StringifyPart StringifyPart::ContinueLine() {
    return StringifyPart(result, indent_level, shrink, true);
}

StringifyPart& StringifyPart::Indent(int bonus) {
    if (continue_line) {
        continue_line = false;
        return *this;
    }

    if (shrink) {
        result += "";
    } else {
        result += std::string((indent_level + bonus) * 4, ' ');
    }
    return *this;
}

StringifyPart& StringifyPart::EndLine(){
    continue_line = false;
    if (not shrink) {
        result += "\n";
    }
    return *this;
}

StringifyPart& StringifyPart::DeleteComma() {
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

}  // namespace AutoJson
