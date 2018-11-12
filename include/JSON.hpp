#ifndef AUTOJSON_JSON
#define AUTOJSON_JSON

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

    // small STL constructors
    template<typename Type>
    JSON(const std::vector<Type> &els) : type(JSONType::VECTOR) {
        auto v = new std::vector<JSON>;

        for (const Type& itr : els) {
            v->emplace_back(JSON(itr));
        }

        this->content = v;
    }

    template<typename T>
    JSON(const std::map<std::string, T> &els) : type(JSONType::OBJECT) {
        auto m = new std::map<std::string, JSON>;

        for (const auto& itr : els) {
            (*m)[itr.first] = JSON(itr.second);
        }
        this->content = m;
    }

    ~JSON();

    JSON(const JSON&);

    JSON& operator=(const JSON&);

    JSON(JSON&&);

    JSON& operator=(JSON&&);

    // if the JSON is invalid (just declared or it's not in a map)
    // the | or || operator will give it a valud for easier use when writing
    // deserialize specialised methods
    template<typename T>
    JSON& operator||(const T &rhs) {
        if (this->type == JSONType::INVALID) {
            *this = JSON(rhs);
        }
        return *this;
    }

    template<typename T>
    JSON& operator||(T &&rhs) {
        if (this->type == JSONType::INVALID) {
            *this = JSON(std::move(rhs));
        }
        return *this;
    }

    template<typename T>
    JSON& operator|(const T &rhs) {
        return *this || rhs;
    }

    template<typename T>
    JSON& operator|(T &&rhs) {
        return *this || std::move(rhs);
    }

    JSON& operator||(std::initializer_list<JSON>);
    JSON& operator|(std::initializer_list<JSON>);

    // strings
    JSON(char c) : type(JSONType::STRING), content(new std::string(1, c)) { }

    JSON(const char *c) : type(JSONType::STRING), content(new std::string(c)) { }

    JSON(const std::string &c) : type(JSONType::STRING), content(new std::string(c)) { }

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
    bool isHex128() const;

    bool isString() const;
    bool isArray() const;

    bool isInteger(const std::string& key) const;
    bool isReal(const std::string& key) const;
    bool isBool(const std::string& key) const;
    bool isHex(const std::string& key) const;
    bool isHex16(const std::string& key) const;
    bool isHex32(const std::string& key) const;
    bool isHex64(const std::string& key) const;
    bool isHex128(const std::string& key) const;

    bool isString(const std::string& key) const;
    bool isArray(const std::string& key) const;

    bool exists(const std::string& key) const;
    void set(const std::string& key, const JSON& value);
    const JSON& getOrSet(const std::string& key, const JSON& defaultValue = JSON());
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
    operator std::vector<Type>() {
        this->checkType(JSONType::VECTOR);
        std::vector<Type> v;
        for (JSON& itr : (*this)) {
            v.emplace_back(itr.get<Type>());
        }
        return v;
    }

    template<typename Type>
    operator std::vector<Type>() const {
        this->checkType(JSONType::VECTOR);
        std::vector<Type> v;
        for (const JSON& itr : (*this)) {
            v.emplace_back(itr.get<Type>());
        }
        return v;
    }

    // for for-based loops
    std::vector<JSON>::iterator begin();
    std::vector<JSON>::const_iterator begin() const;

    std::vector<JSON>::iterator end();
    std::vector<JSON>::const_iterator end() const;

    template<typename Type>
    JSON& push_back(const Type &rhs) {
        this->checkType(JSONType::VECTOR);
        auto& v = *(std::vector<JSON>*)(this->content);
        v.push_back(rhs);
        return v.back();
    }

    template<typename Type>
    JSON& emplace_back(Type &&rhs) {
        this->checkType(JSONType::VECTOR);
        auto& v = *(std::vector<JSON>*)(this->content);
        v.emplace_back(rhs);
        return v.back();
    }

    void pop_back();

    int size() const;

    // object
    static JSON parseObject(const char *&content);

    void stringifyObject(StringifyPart part) const;

    JSON& operator[](const std::string &key);

    JSON& operator[](const char* key);

    template<typename Type>
    operator std::map<std::string, Type>() {
        std::map<std::string, Type> m;
        auto& mp = *(std::map<std::string, JSON>*)(content);
        for (auto& itr : mp) {
            m[itr.first] = itr.second.get<Type>();
        }
        return m;
    }

    template<typename Type>
    operator std::map<std::string, Type>() const {
        std::map<std::string, Type> m;
        auto& mp = *(std::map<std::string, JSON>*)(content);
        for (const auto& itr : mp) {
            m[itr.first] = itr.second.get<Type>();
        }
        return m;
    }

    bool valid() { return type; }

    template<typename T>
    T get() {
        return T(*this);
    }

    template<typename T>
    T get() const {
        return T(*this);
    }
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

#endif // AUTOJSON_JSON
