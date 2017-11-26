#ifndef AUTOJSON_JSON_HPP
#define AUTOJSON_JSON_HPP

#include <type_traits>
#include <initializer_list>
#include <ostream>
#include <map>
#include <string>
#include <vector>
#include <cxxabi.h>

#include "error.hpp"

namespace AutoJson {

enum JsonType {
    INVALID,
    PRIMITIVE,
    STRING,
    VECTOR,
    OBJECT
};

struct StringifyPart;

struct Json {
    int type;
    void* content;

    Json() : type(JsonType::INVALID), content(nullptr) { }

    Json(JsonType type);

    /// takes ownership of pointer
    Json(JsonType type, void* content) : type(type), content(content) { }

    ~Json();

    Json(const Json&);

    Json& operator=(const Json&);

    Json(Json&&);

    Json& operator=(Json&&);

    /// if the JSON is invalid (just declared or it's not in a map)
    /// the | or || operator will give it a valud for easier use when writing
    /// deserialize specialised methods
    template<typename T>
    Json& operator||(const T&);

    template<typename T>
    Json& operator||(T&&);

    Json& operator||(std::initializer_list<Json>);

    template<typename T>
    Json& operator|(const T&);

    template<typename T>
    Json& operator|(T&&);

    Json& operator|(std::initializer_list<Json>);

    /// strings
    Json(char c) : type(JsonType::STRING), content(new std::string(1, c)) { }

    Json(const char* c) : type(JsonType::STRING), content(new std::string(c)) { }

    Json(const std::string& c) : type(JsonType::STRING), content(new std::string(c)) { }

    /// Deep copy pointers
    template<typename T>
    Json(const T*);

    /// Deep copy pointers
    template<typename T>
    Json(T*);

    /// Light stl constructors
    template<typename Type>
    Json(const std::vector<Type>&);

    template<typename T>
    Json(const std::map<std::string, T>& els);

    /// std initializer constructor - does good things
    Json(std::initializer_list<Json> list);

    /// ParseJson
    static Json Parse(const char*& content);

    static Json Parse(const std::string& content);

    static Json ReadFromFile(const std::string& file_name);

    /// Output Json to String
    std::string Stringify(int shrink=true) const;

    void Stringify(StringifyPart part) const;

    /// helper functions.
    /// The non-const method casts the JSON to the desired type
    /// if it's INVALID
    void CheckType(JsonType type);

    void CheckType(JsonType type) const;

    friend std::ostream& operator<<(std::ostream& stream, const Json& json);

    /// primitive

    void StringifyPrimitive(StringifyPart part) const;

    // if the JSON is invalid and not const, it will change type
    // to the converted value and assign it with a default value
    // * 0 for numbers and empty for the other
    // better than throwing an exception
    operator bool();
    operator bool() const;
    Json(bool b) : type(PRIMITIVE), content(new std::string(b ? "true" : "false")) { }

    operator int();
    operator int() const;
    Json(int i) : type(PRIMITIVE), content(new std::string(std::to_string(i))) { }

    operator long();
    operator long() const;
    Json(long l) : type(PRIMITIVE), content(new std::string(std::to_string(l))) { }

    operator unsigned long();
    operator unsigned long() const;
    Json(unsigned long ul) : type(PRIMITIVE), content(new std::string(std::to_string(ul))) { }

    operator long long();
    operator long long() const;
    Json(long long ll) : type(PRIMITIVE), content(new std::string(std::to_string(ll))) { }

    operator unsigned long long();
    operator unsigned long long() const;
    Json(unsigned long long ull) : type(PRIMITIVE), content(new std::string(std::to_string(ull))) { }

    operator float();
    operator float() const;
    Json(float f) : type(PRIMITIVE), content(new std::string(std::to_string(f))) { }

    operator double();
    operator double() const;
    Json(double d) : type(PRIMITIVE), content(new std::string(std::to_string(d))) { }

    operator long double();
    operator long double() const;
    Json(long double ld) : type(PRIMITIVE), content(new std::string(std::to_string(ld))) { }

    /// string

    void StringifyString(StringifyPart part) const;

    operator std::string();
    operator std::string() const;

    /// vector

    static Json ParseVector(const char*& content);

    void StringifyVector(StringifyPart part) const;

    Json& operator[](int key);

    template<typename Type>
    operator std::vector<Type>();

    template<typename Type>
    operator std::vector<Type>() const;

    /// for for-based loops
    std::vector<Json>::iterator begin();
    std::vector<Json>::const_iterator begin() const;

    std::vector<Json>::iterator end();
    std::vector<Json>::const_iterator end() const;

    template<typename Type>
    Json& push_back(const Type&);

    template<typename Type>
    Json& emplace_back(Type&&);

    void pop_back();

    int size() const;

    /// object
    static Json ParseObject(const char*& content);

    void StringifyObject(StringifyPart part) const;

    Json& operator[](const std::string& key);

    Json& operator[](const char* key);

    template<typename Type>
    operator std::map<std::string, Type>();

    template<typename Type>
    operator std::map<std::string, Type>() const;

    bool Valid() { return type; }

    // very very general stuff

    template<typename Type>
    operator Type();

    /// If type is unknown, treat the object like a primitive
    /// This constructor can be specialised for custom classes
    template<typename Type>
    Json(const Type& a);

    // if you want to call it explicitely
    template<typename T>
    void LoadInto(T& x);

    template<typename T>
    T Get();
};

struct StringifyPart {
    std::string& result;
    int indent_level;
    bool shrink;
    bool continue_line;

    StringifyPart(std::string& result, int indent_level, bool shrink, bool continue_line)
        : result(result), indent_level(indent_level), shrink(shrink), continue_line(continue_line) {
    }

    StringifyPart IncreaseIndent();

    StringifyPart ContinueLine();

    StringifyPart& Indent(int bonus=0);

    StringifyPart& EndLine();

    StringifyPart& DeleteComma();
};


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

template<typename T>
Json& Json::operator|(const T& rhs) {
    return *this || rhs;
}

template<typename T>
Json& Json::operator|(T&& rhs) {
    return *this || std::move(rhs);
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

template<typename Type>
Json::operator std::vector<Type>() {
    CheckType(JsonType::VECTOR);
    std::vector<Type> v;
    for (Json& itr : (*this)) {
        v.emplace_back(itr.Get<Type>());
    }
    return v;
}

template<typename Type>
Json::operator std::vector<Type>() const {
    CheckType(JsonType::VECTOR);
    std::vector<Type> v;
    for (const Json& itr : (*this)) {
        v.emplace_back(itr.Get<Type>());
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


template<typename Type>
Json::operator std::map<std::string, Type>() {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, Json>*)(content);
    for (auto& itr : mp) {
        m[itr.first] = itr.second.Get<Type>();
    }
    return m;
}

template<typename Type>
Json::operator std::map<std::string, Type>() const {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, Json>*)(content);
    for (const auto& itr : mp) {
        m[itr.first] = itr.second.Get<Type>();
    }
    return m;
}

template<typename Type>
Json::operator Type() {
    char * name = 0;
    int status;
    name = abi::__cxa_demangle(typeid(Type).name(), 0, 0, &status);

    std::string type_name = "";

    if (name != 0) {
        type_name = name;
    } else {
        type_name = typeid(Type).name();
    }

    free(name);

    std::string message = "";
    message += "Unknown conversion from Json to ";
    message += type_name;
    message += "\n";
    JsonError(message);
}

/// If type is unknown, treat the object like a primitive
/// This constructor can be specialised for custom classes
template<typename Type>
Json::Json(__attribute__((unused)) const Type& a) {
    char * name = 0;
    int status;
    name = abi::__cxa_demangle(typeid(Type).name(), 0, 0, &status);

    std::string type_name = "";

    if (name != 0) {
        type_name = name;
    } else {
        type_name = typeid(Type).name();
    }

    free(name);

    std::string message = "";
    message += "Undefined constructor for Json from type ";
    message += type_name;
    message += "\n";
    JsonError(message);
}

// if you want to call it explicitely
template<typename T>
void Json::LoadInto(T& x) {
    x = this->operator T();
}

template<typename T>
T Json::Get() {
    return this->operator T();
}



}  // namespace AutoJson
#endif
