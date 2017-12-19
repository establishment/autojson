#include "/JSON.hpp"
#include <JSON>

namespace autojson {
template<>
autojson::JSON::JSON(const ::autojson::JSON& rhs) : type(JSONType::OBJECT), content(new std::map<std::string, JSON>()) {
	(*this)["type"] = rhs.type;
	(*this)["*content"] = rhs.*content;
}

template<>
autojson::JSON::operator ::autojson::JSON() {
	::autojson::JSON obj;
	obj.type = (*this)["type"].get<int>();
	obj.*content = (*this)["*content"].get<void>();
	return obj;
}
}  //namespace autojson

namespace autojson {
template<>
autojson::JSON::JSON(__attribute__((unused)) const ::autojson::JSONType& rhs) : type(JSONType::OBJECT), content(new std::map<std::string, JSON>()) {
	(*this) = int(rhs);
}

template<>
autojson::JSON::operator ::autojson::JSONType() {
	::autojson::JSONType obj;
	obj = ::autojson::JSONType((*this).get<int>());
	return obj;
}
}  //namespace autojson

namespace autojson {
template<>
autojson::JSON::JSON(const ::autojson::StringifyPart& rhs) : type(JSONType::OBJECT), content(new std::map<std::string, JSON>()) {
	(*this)["result"] = rhs.result;
	(*this)["indentLevel"] = rhs.indentLevel;
	(*this)["shrink"] = rhs.shrink;
	(*this)["continueLine"] = rhs.continueLine;
}

template<>
autojson::JSON::operator ::autojson::StringifyPart() {
	::autojson::StringifyPart obj;
	obj.result = (*this)["result"].get<std::string&>();
	obj.indentLevel = (*this)["indentLevel"].get<int>();
	obj.shrink = (*this)["shrink"].get<bool>();
	obj.continueLine = (*this)["continueLine"].get<bool>();
	return obj;
}
}  //namespace autojson


