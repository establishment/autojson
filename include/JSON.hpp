#ifndef AUTOJSON_JSON_HPP
#define AUTOJSON_JSON_HPP

#include <initializer_list>
#include <ostream>
#include <map>
#include <string>
#include <vector>

namespace autojson {

enum JSONType {
    INVALID,
    PRIMITIVE,
    STRING,
    VECTOR,
    OBJECT
};

class StringifyPart;

class JSON {
  public:
    int type;
    void *content;

    JSON() : type(JSONType::INVALID), content(nullptr) { }

    JSON(JSONType type);

    // takes ownership of pointer
    JSON(JSONType type, void *content) : type(type), content(content) { }

    ~JSON();

    JSON(const JSON&);

    JSON& operator=(const JSON&);

    JSON(JSON&&);

    JSON& operator=(JSON&&);

    // if the JSON is invalid (just declared or it's not in a map)
    // the | or || operator will give it a valud for easier use when writing
    // deserialize specialised methods
    template<typename T>
    JSON& operator||(const T&);

    template<typename T>
    JSON& operator||(T&&);

    JSON& operator||(std::initializer_list<JSON>);

    template<typename T>
    JSON& operator|(const T&);

    template<typename T>
    JSON& operator|(T&&);

    JSON& operator|(std::initializer_list<JSON>);

    // strings
    JSON(char c) : type(JSONType::STRING), content(new std::string(1, c)) { }

    JSON(const char *c) : type(JSONType::STRING), content(new std::string(c)) { }

    JSON(const std::string &c) : type(JSONType::STRING), content(new std::string(c)) { }

    // Deep copy pointers
    template<typename T>
    JSON(const T*);

    // Deep copy pointers
    template<typename T>
    JSON(T*);

    // Light stl constructors
    template<typename Type>
    JSON(const std::vector<Type>&);

    template<typename T>
    JSON(const std::map<std::string, T> &els);

    // std initializer constructor - does good things
    JSON(std::initializer_list<JSON> list);

    // ParseJson
    static JSON parse(const char *&content);

    static JSON parse(const std::string &content);

    static JSON readFromFile(const std::string &file_name);

    // Output JSON to String
    std::string stringify(int shrink=true) const;

    void stringify(StringifyPart part) const;

    // helper functions.
    // The non-const method casts the JSON to the desired type
    // if it's INVALID
    void checkType(JSONType type);

    void checkType(JSONType type) const;

    friend std::ostream& operator<<(std::ostream &stream, const JSON &JSON);

    /// primitive

    void stringifyPrimitive(StringifyPart part) const;

    // if the JSON is invalid and not const, it will change type
    // to the converted value and assign it with a default value
    // * 0 for numbers and empty for the other
    // better than throwing an exception
    operator bool();
    operator bool() const;
    JSON(bool b) : type(PRIMITIVE), content(new std::string(b ? "true" : "false")) { }

    operator int();
    operator int() const;
    JSON(int i) : type(PRIMITIVE), content(new std::string(std::to_string(i))) { }

    operator long();
    operator long() const;
    JSON(long l) : type(PRIMITIVE), content(new std::string(std::to_string(l))) { }

    operator unsigned long();
    operator unsigned long() const;
    JSON(unsigned long ul) : type(PRIMITIVE), content(new std::string(std::to_string(ul))) { }

    operator long long();
    operator long long() const;
    JSON(long long ll) : type(PRIMITIVE), content(new std::string(std::to_string(ll))) { }

    operator unsigned long long();
    operator unsigned long long() const;
    JSON(unsigned long long ull) : type(PRIMITIVE), content(new std::string(std::to_string(ull))) { }

    operator float();
    operator float() const;
    JSON(float f) : type(PRIMITIVE), content(new std::string(std::to_string(f))) { }

    operator double();
    operator double() const;
    JSON(double d) : type(PRIMITIVE), content(new std::string(std::to_string(d))) { }

    operator long double();
    operator long double() const;
    JSON(long double ld) : type(PRIMITIVE), content(new std::string(std::to_string(ld))) { }

    bool isInteger() const;
    bool isReal() const;
    bool isBool() const;
    bool isHex() const;
    bool isHex16() const;
    bool isHex32() const;
    bool isHex64() const;
    
    bool isString() const;
    bool isArray() const;

    bool isInteger(const std::string& key) const;
    bool isReal(const std::string& key) const;
    bool isBool(const std::string& key) const;
    bool isHex(const std::string& key) const;
    bool isHex16(const std::string& key) const;
    bool isHex32(const std::string& key) const;
    bool isHex64(const std::string& key) const;
    
    bool isString(const std::string& key) const;
    bool isArray(const std::string& key) const;

    bool exists(const std::string& key) const;
    void set(const std::string& key, const JSON& value);
    const JSON& get(const std::string& key, const JSON& defaultValue = JSON(), bool setIfNotExists = false);
    const JSON& get(const std::string& key, const JSON& defaultValue = JSON()) const;

    operator JSON();
    operator JSON() const;

    // string

    void stringifyString(StringifyPart part) const;

    operator std::string();
    operator std::string() const;

    // vector

    static JSON parseVector(const char *&content);

    void stringifyVector(StringifyPart part) const;

    JSON& operator[](int key);

    template<typename Type>
    operator std::vector<Type>();

    template<typename Type>
    operator std::vector<Type>() const;

    // for for-based loops
    std::vector<JSON>::iterator begin();
    std::vector<JSON>::const_iterator begin() const;

    std::vector<JSON>::iterator end();
    std::vector<JSON>::const_iterator end() const;

    template<typename Type>
    JSON& push_back(const Type&);

    template<typename Type>
    JSON& emplace_back(Type&&);

    void pop_back();

    int size() const;

    // object
    static JSON parseObject(const char *&content);

    void stringifyObject(StringifyPart part) const;

    JSON& operator[](const std::string &key);

    JSON& operator[](const char* key);

    template<typename Type>
    operator std::map<std::string, Type>();

    template<typename Type>
    operator std::map<std::string, Type>() const;

    bool valid() { return type; }

    // very very general stuff

    template<typename Type>
    operator Type(); // RIP camelCase

    // If type is unknown, treat the object like a primitive
    // This constructor can be specialised for custom classes
    template<typename Type>
    JSON(const Type &a);

    // if you want to call it explicitely
    template<typename T>
    void loadInto(T &x);

    template<typename T>
    T get();
};

class StringifyPart {
  public:
    std::string& result;
    int indentLevel;
    bool shrink;
    bool continueLine;

    StringifyPart(std::string& result, int indentLevel, bool shrink, bool continueLine)
        : result(result), indentLevel(indentLevel), shrink(shrink), continueLine(continueLine) {
    }

    StringifyPart increaseIndent();

    StringifyPart isContinueLine();

    StringifyPart& indent(int bonus = 0);

    StringifyPart& endLine();

    StringifyPart& deleteComma();
};

}  // namespace autojson

#include "JSON.tpp"

#ifndef autojsonuselib
#include "autojson_src/JSON.cpp"
#endif

#endif // AUTOJSON_JSON_HPP
