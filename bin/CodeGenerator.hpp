#ifndef AUTOJSON_BIN_CODE_GENERATOR_HPP
#define AUTOJSON_BIN_CODE_GENERATOR_HPP

#include "ASTElements.hpp"

#include <vector>
#include <string>

std::string JSONFieldName(const std::string &name) {
    bool nextUppercase = false;
    std::string result = "";
    for (auto itr : name) {
        if (itr == '_') {
            nextUppercase = true;
        } else {
            char c = itr;
            if (nextUppercase) {
                if ('a' <= c and c <= 'z') {
                    c += 'A' - 'a';
                }
            }

            result += c;
            nextUppercase = false;
        }
    }
    return result;
}

std::string JSONifyClass(Class c) {
    std::string result = "";

    result +=
        "namespace autojson {\n"
        "template<>\n"
        "autojson::JSON::JSON(";
    result += ((c.fields.size()) ? "" : "__attribute__((unused)) ");
    result +=
        "const " + c.scope + c.name + "& rhs) : type(JSONType::OBJECT), content(new std::map<std::string, JSON>()) {\n";

    for (auto itr : c.fields) {
        result += "\t(*this)[\"" + JSONFieldName(itr.name) + "\"] = rhs." + itr.name + ";\n";
    }

    if (c.isEnum) {
        result +=
            "\t(*this) = int(rhs);\n";
    }

    result += "}\n\n";

    result +=
        "template<>\n"
        "autojson::JSON::operator " + c.scope + c.name + "() {\n"
        "\t" + c.scope + c.name + " obj;\n";

    for (auto itr : c.fields) {
        result +=
            "\tobj." + itr.name + " = (*this)[\"" + JSONFieldName(itr.name) + "\"].get<" + itr.type+ ">();\n";
    }

    if (c.isEnum) {
        result +=
            "\tobj = " + c.scope + c.name + "((*this).get<int>());\n";
    }

    result += "\treturn obj;\n";
    result += "}\n";
    result += "}  //namespace autojson\n\n";

    return result;
}

#endif // AUTOJSON_BIN_CODE_GENERATOR_HPP
