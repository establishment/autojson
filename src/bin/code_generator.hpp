#pragma once
#include "ast_elements.hpp"
#include <vector>
#include <string>
using namespace std;

string JsonFieldName(const string& name) {
    bool next_uppercase = false;
    string result = "";
    for (auto itr : name) {
        if (itr == '_') {
            next_uppercase = true;
        } else {
            char c = itr;
            if (next_uppercase) {
                if ('a' <= c and c <= 'z') {
                    c += 'A' - 'a';
                }
            }

            result += c;
            next_uppercase = false;
        }
    }
    return result;
}

string JsonifyClass(Class c) {
    string result = "";
    
    result += 
        "namespace AutoJson {\n"
        "template<>\n"
        "AutoJson::Json::Json(";
    result += ((c.fields.size()) ? "" : "__attribute__((unused)) ");
    result +=
        "const " + c.scope + c.name + "& rhs) : type(JsonType::OBJECT), content(new std::map<std::string, Json>()) {\n";

    for (auto itr : c.fields) {    
        result += "\t(*this)[\"" + JsonFieldName(itr.name) + "\"] = rhs." + itr.name + ";\n";
    }

    if (c.is_enum) {
        result += 
            "\t(*this) = int(rhs);\n";
    }

    result += "}\n\n";

    result += 
        "template<>\n"
        "AutoJson::Json::operator " + c.scope + c.name + "() {\n"
        "\t" + c.scope + c.name + " obj;\n";

    for (auto itr : c.fields) {
        result += 
            "\tobj." + itr.name + " = (*this)[\"" + JsonFieldName(itr.name) + "\"].Get<" + itr.type+ ">();\n";
    }

    if (c.is_enum) {
        result += 
            "\tobj = " + c.scope + c.name + "((*this).Get<int>());\n";
    }

    result += "\treturn obj;\n";
    result += "}\n";
    result += "}  //namespace AutoJson\n\n";

    return result;
}

