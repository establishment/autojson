#include <type_traits>
#include <cxxabi.h>

#include "error.hpp"

namespace AutoJson {

// operators | and || for giving a default value to invaild JSONs
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

// deep copy pointers
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

// small STL constructors
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

// If type is unknown, treat the object like a primitive
// This constructor can be specialised for custom classes
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
