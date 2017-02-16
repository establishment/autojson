#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::cerr;

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
    std::vector<FieldType> template_types;
    FieldType(const std::string& type) {
        const char* start = type.c_str();
        Parse(start);
    }

    FieldType(const char*& type) {
        Parse(type);
    }

    void Parse(const char*& type) {
        while (*type != '\0' and *type != '>' and *type != '<' and *type != ',') {
            name += *type;
            type++;
        }

        TrimSpaces(name);

        if (*type == '>' or *type == ',') {
            return;
        }

        if (*type == '<') {
            type++;
            while (*type != '>') {
                template_types.push_back(FieldType(type));
                while (*type == ',' or *type == ' ' or *type == '\n') {
                    type++;
                }
            }
        }
    }

    std::string GetName() {
        std::string result = name;
        if (template_types.size()) {
            result += "<";
            for (auto& itr : template_types) {
                result += itr.GetName() + ", ";
            }
            result.pop_back();
            result.pop_back();
            result += ">";
        }
        return result;
    }

    void Debug() {
        std::cerr << "Type\t" << name << '\n';
        if (template_types.size()) {
            std::cerr << "*Templates\t[\n";
            for (auto itr : template_types) {
                itr.Debug();
            }
            std::cerr << "]\n";
        }
    }
};

struct Field {
    std::string type;
    std::string name;

    Field() : type(""), name("") { }

    Field(const std::string& type, const std::string& name) : type(type), name(name) {
        TrimSpaces(this->type);
        TrimSpaces(this->name);
    }

    std::string OnlyType() {
        std::string clean_type = "";
        for (char c : type) {
            if (c == '<' or c == '*') {
                break;
            }
            clean_type += c;
        }
        return clean_type;
    }
};

struct Class {
    std::string file_path;
    std::string scope;
    std::string name;
    std::vector<Field> fields;
    
    bool is_enum;
};

struct ClassBundle {
    std::vector<Class> all_classes;
    static std::vector<std::string> builtin_classes;

    void Merge(const ClassBundle& rhs) {
        for (auto itr : rhs.all_classes) {
            all_classes.push_back(itr);
        }
    }


    std::map<std::string, int> unknown_classes;

    void UpdateFieldType(FieldType& type) {
        // solve childrens
        for (auto& itr : type.template_types) {
            UpdateFieldType(itr);
        }

        Field only_base_type;
        only_base_type.type = type.name;

        std::string clean_type = only_base_type.OnlyType();
        std::string remaining_type = only_base_type.type.substr(clean_type.size(), only_base_type.type.size());

        for (auto builtin_class : builtin_classes) {
            if (clean_type == builtin_class) {
                return;
            }
        }

        for (auto& target_class : all_classes) {
            // check if class_type ends with my type
            std::string class_type = target_class.scope + target_class.name;

            if (class_type.size() < clean_type.size()) {
                continue;
            }

            if (clean_type == class_type.substr(class_type.size() - clean_type.size(), clean_type.size())) {
                type.name = class_type + remaining_type;
                return;
            }
        }

        if (unknown_classes[clean_type] == 1) {
            return;
        }

        unknown_classes[clean_type] = 1;
        std::cerr << "[Warning]\t" << "Cannot determine full scope for class \"" << clean_type << "\"\n";

    }

    void UpdateField(Field& field) {
        FieldType type(field.type);
//        type.Debug();

        UpdateFieldType(type);

        field.type = type.GetName();
    }

    void UpgradeScopes() {
        unknown_classes.clear();
        for (auto& cls : all_classes) {
            for (auto& field : cls.fields) {
                UpdateField(field);
            }
        }

    }
};

std::vector<std::string> ClassBundle::builtin_classes = {
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
