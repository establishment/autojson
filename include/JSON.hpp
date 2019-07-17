#ifndef AUTOJSON_JSON
#define AUTOJSON_JSON

#include <initializer_list>
#include <ostream>
#include <map>
#include <string>
#include <vector>
#include <atomic>

namespace autojson {

enum JSONType {
    INVALID,
    PRIMITIVE,
    STRING,
    VECTOR,
    OBJECT
};

inline std::string JSONTypeToString(JSONType t) {
    switch (t) {
        case INVALID:       return "invalid";
        case PRIMITIVE:     return "primitive";
        case STRING:        return "string";
        case VECTOR:        return "vector";
        case OBJECT:        return "object";
        default:            return "unknown";
    }
}

class StringifyPart;

class JSON {
public:
    JSONType type;
    void *content;

    JSON() : type(JSONType::INVALID), content(nullptr) { }

    JSON(JSONType type);

    // takes ownership of pointer
    JSON(JSONType type, void *content) : type(type), content(content) { }

    // STL constructors
    JSON(const std::string &c) : type(JSONType::STRING), content(new std::string(c)) { }
    JSON(const char *c) : type(JSONType::STRING), content(new std::string(c)) { }


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

    // std initializer constructor - does good things
    JSON(std::initializer_list<JSON> list);

    ~JSON();

    JSON(const JSON&);

    JSON& operator=(const JSON&);

    JSON(JSON&&);

    JSON& operator=(JSON&&);

    // If the JSON is invalid (just declared or it's not in a map)
    // The | operator will give it a valid for easier use when writing deserialize specialised methods
    // Example of usages:
    //  int seed = jsonEntry["seed"] | 1337;

    // Do not use like below if you're *NOT SURE* that jsonEntry["config"] is a valid map
    //  int seed = jsonEntry["config"]["seed"] | 1337;
    template<typename T>
    JSON& operator|(const T &rhs) {
        if (this->type == JSONType::INVALID) {
            *this = JSON(rhs);
        }
        return *this;
    }

    template<typename T>
    JSON& operator|(T &&rhs) {
        if (this->type == JSONType::INVALID) {
            *this = JSON(std::move(rhs));
        }
        return *this;
    }

    // Deserialize API
    static JSON parse(const char *&content);

    static JSON parse(const std::string &content);

    static JSON readFromFile(const std::string &file_name);

    // Serialize API
    std::string stringify(int shrink=true) const;

    void stringify(StringifyPart part) const;

    /// primitive

    void stringifyPrimitive(StringifyPart part) const;

    // if the JSON is invalid and not const, it will change type
    // to the converted value and assign it with a default value
    // * 0 for numbers and empty for the other
    // better than throwing an exception
    operator bool() const;
    JSON(bool b) : type(PRIMITIVE), content(new std::string(b ? "true" : "false")) { }

    operator int() const;
    JSON(int i) : type(PRIMITIVE), content(new std::string(std::to_string(i))) { }

    operator long() const;
    JSON(long l) : type(PRIMITIVE), content(new std::string(std::to_string(l))) { }

    operator unsigned long() const;
    JSON(unsigned long ul) : type(PRIMITIVE), content(new std::string(std::to_string(ul))) { }

    operator long long() const;
    JSON(long long ll) : type(PRIMITIVE), content(new std::string(std::to_string(ll))) { }

    operator unsigned long long() const;
    JSON(unsigned long long ull) : type(PRIMITIVE), content(new std::string(std::to_string(ull))) { }

    operator float() const;
    JSON(float f) : type(PRIMITIVE), content(new std::string(std::to_string(f))) { }

    operator double() const;
    JSON(double d) : type(PRIMITIVE), content(new std::string(std::to_string(d))) { }

    operator long double() const;
    JSON(long double ld) : type(PRIMITIVE), content(new std::string(std::to_string(ld))) { }

    template<class T>
    JSON(const std::atomic<T> &o) : JSON((T)o) {}

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
    bool isObject() const;

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
    bool isObject(const std::string& key) const;

    bool exists(const std::string& key) const;
    void set(const std::string& key, const JSON& value);
    JSON& getOrSet(const std::string& key, const JSON& defaultValue = JSON());
    JSON get(const std::string& key, const JSON& defaultValue = JSON()) const;

    // string

    void stringifyString(StringifyPart part) const;

    operator std::string() const;

    // vector

    static JSON parseVector(const char *&content);

    void stringifyVector(StringifyPart part) const;

    JSON& operator[](int key);

    template<typename Type>
    operator std::vector<Type>() const {
        this->checkType(JSONType::VECTOR);
        std::vector<Type> v;
        for (const JSON& itr : (*this)) {
            v.emplace_back((Type)itr);
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
        this->checkTypeAndSetIfInvalid(JSONType::VECTOR);
        auto& v = *(std::vector<JSON>*)(this->content);
        v.push_back(rhs);
        return v.back();
    }

    template<typename Type>
    JSON& emplace_back(Type &&rhs) {
        this->checkTypeAndSetIfInvalid(JSONType::VECTOR);
        auto& v = *(std::vector<JSON>*)(this->content);
        v.emplace_back(rhs);
        return v.back();
    }

    void pop_back() {
        this->checkTypeAndSetIfInvalid(JSONType::VECTOR);
        auto& v = *(std::vector<JSON>*)(this->content);
        v.pop_back();
    }

    int size() const;

    // object
    static JSON parseObject(const char *&content);

    void stringifyObject(StringifyPart part) const;

    JSON& operator[](const std::string &key);
    JSON& operator[](const char *key);


    template<typename Type>
    operator std::map<std::string, Type>() const {
        std::map<std::string, Type> m;
        auto& mp = *(std::map<std::string, JSON>*)(content);
        for (const auto& itr : mp) {
            m[itr.first] = (Type)(itr.second);
        }
        return m;
    }

    // Helper functions.
    void checkType(JSONType type) const;
    void checkTypeAndSetIfInvalid(JSONType type);

    bool valid() const {
        return type != INVALID;
    }
};

inline std::ostream& operator<<(std::ostream &os, const JSON &JSON) {
    os << JSON.stringify();
    return os;
}

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

typedef void(*FatalErrorCallback)(const std::string&);

FatalErrorCallback& GetFatalErrorCallback();

void SetFatalErrorCallback(FatalErrorCallback callback);

}  // namespace autojson

#endif // AUTOJSON_JSON
