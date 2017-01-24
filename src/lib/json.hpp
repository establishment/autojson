#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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

    /// If type is unknown, treat the object like a primitive
    /// This constructor can be specialised for custom classes
    template<typename T> 
    Json(const T& a) : type(JsonType::PRIMITIVE), content(new std::string(std::to_string(a))) { }

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
    std::string Stringify(int shrink=true);

    void Stringify(StringifyPart part);

    /// helper functions.
    /// The non-const method casts the JSON to the desired type
    /// if it's INVALID
    void CheckType(JsonType type);
    
    void CheckType(JsonType type) const;
    
    /// primitive

    void StringifyPrimitive(StringifyPart part);

    // if the JSON is invalid and not const, it will change type
    // to the converted value and assign it with a default value
    // * 0 for numbers and empty for the other
    // better than throwing an exception
    operator int();
    operator int() const;

    operator long();
    operator long() const;

    operator unsigned long();
    operator unsigned long() const;

    operator long long();
    operator long long() const;

    operator unsigned long long();
    operator unsigned long long() const;

    operator float();
    operator float() const;

    operator double();
    operator double() const;

    operator long double();
    operator long double() const;

    /// string

    void StringifyString(StringifyPart part);

    operator std::string();
    operator std::string() const;

    /// vector

    static Json ParseVector(const char*& content); 
    
    void StringifyVector(StringifyPart part);

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
    
    void StringifyObject(StringifyPart part);
    
    Json& operator[](const std::string& key);
    
    Json& operator[](const char* key);

    template<typename Type>
    operator std::map<std::string, Type>();

    template<typename Type>
    operator std::map<std::string, Type>() const;
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

}  // namespace AutoJson
