#ifndef AUTOJSON_BIN_ASTELEMENTS_HPP
#define AUTOJSON_BIN_ASTELEMENTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

void TrimSpaces(std::string& txt) {
    while (txt.size() and txt.back() == ' ') {
        txt.pop_back();
    }
    for (int i = 0; 2 * i < (int)txt.size(); i += 1) {
        std::swap(txt[i], txt[txt.size() - i - 1]);
    }

    while (txt.size() and txt.back() == ' ') {
        txt.pop_back();
    }
    for (int i = 0; 2 * i < (int)txt.size(); i += 1) {
        std::swap(txt[i], txt[txt.size() - i - 1]);
    }
}

struct FieldType {
    std::string name;
    std::vector<FieldType> templateTypes;
    FieldType(const std::string &type) {
        const char *start = type.c_str();
        this->parse(start);
    }

    FieldType(const char *&type) {
        this->parse(type);
    }

    void parse(const char *&type) {
        while (*type != '\0' and *type != '>' and *type != '<' and *type != ',') {
            this->name += *type;
            type++;
        }

        TrimSpaces(this->name);

        if (*type == '>' or *type == ',') {
            return;
        }

        if (*type == '<') {
            type++;
            while (*type != '>') {
                this->templateTypes.push_back(FieldType(type));
                while (*type == ',' or *type == ' ' or *type == '\n') {
                    type++;
                }
            }
        }
    }

    std::string getName() {
        std::string result = name;
        if (this->templateTypes.size()) {
            result += "<";
            for (auto& itr : this->templateTypes) {
                result += itr.getName() + ", ";
            }
            result.pop_back();
            result.pop_back();
            result += ">";
        }
        return result;
    }

    void debug() {
        std::cerr << "Type\t" << name << '\n';
        if (this->templateTypes.size()) {
            std::cerr << "*Templates\t[\n";
            for (auto itr : this->templateTypes) {
                itr.debug();
            }
            std::cerr << "]\n";
        }
    }
};

struct Field {
    std::string type;
    std::string name;

    Field() : type(""), name("") { }

    Field(const std::string &type, const std::string &name) : type(type), name(name) {
        TrimSpaces(this->type);
        TrimSpaces(this->name);
    }

    std::string onlyType() {
        std::string cleanType = "";
        for (char c : type) {
            if (c == '<' or c == '*') {
                break;
            }
            cleanType += c;
        }
        return cleanType;
    }
};

struct Class {
    std::string filePath;
    std::string scope;
    std::string name;
    std::vector<Field> fields;

    bool isEnum;
};

struct ClassBundle {
    std::vector<Class> allClasses;
    static std::vector<std::string> builtinClasses;

    void merge(const ClassBundle &rhs) {
        for (auto itr : rhs.allClasses) {
            allClasses.push_back(itr);
        }
    }

    std::map<std::string, int> unknownClasses;

    void updateFieldType(FieldType &type) {
        // solve childrens
        for (auto& itr : type.templateTypes) {
            updateFieldType(itr);
        }

        Field onlyBaseType;
        onlyBaseType.type = type.name;

        std::string cleanType = onlyBaseType.onlyType();
        std::string remainingType = onlyBaseType.type.substr(cleanType.size(), onlyBaseType.type.size());

        for (auto builtinClass : this->builtinClasses) {
            if (cleanType == builtinClass) {
                return;
            }
        }

        for (auto& targetClass : this->allClasses) {
            // check if classType ends with my type
            std::string classType = targetClass.scope + targetClass.name;

            if (classType.size() < cleanType.size()) {
                continue;
            }

            if (cleanType == classType.substr(classType.size() - cleanType.size(), cleanType.size())) {
                type.name = classType + remainingType;
                return;
            }
        }

        if (unknownClasses[cleanType] == 1) {
            return;
        }

        unknownClasses[cleanType] = 1;
        std::cerr << "[Warning]\t" << "Cannot determine full scope for class \"" << cleanType << "\"\n";

    }

    void updateField(Field &field) {
        FieldType type(field.type);
//        type.Debug();

        this->updateFieldType(type);

        field.type = type.getName();
    }

    void upgradeScopes() {
        this->unknownClasses.clear();
        for (auto& cls : this->allClasses) {
            for (auto& field : cls.fields) {
                this->updateField(field);
            }
        }

    }
};

std::vector<std::string> ClassBundle::builtinClasses = {
    "char",
    "bool",
    "int",
    "long",
    "unsigned long",
    "long long",
    "unsided long long",
    "float",
    "double",
    "vector",
    "std::vector",
    "string",
    "std::string",
    "map",
    "std::map"
};

#endif // AUTOJSON_BIN_ASTELEMENTS_HPP
