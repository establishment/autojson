#pragma once

#include "ast_elements.hpp"

#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

string EraseComments(const string& content) {
    string result;
    result += content[0];

    bool quoted = false;
    bool commented = false;

    int index = 1;
    while (index < (int)content.size()) {
        if (commented) {
            if (content[index] == '/' and content[index - 1] == '*') {
                commented = false;
            }
            index++;
            continue;
        }

        if (quoted) {
            result += content[index];
            if (content[index - 1] != '\\' and content[index] == '"') {
                ;
            } else {
                quoted = false;
            }
            index++;
            continue;
        }

        if (content[index - 1] == '/' and content[index] == '*') {
            result.pop_back();
            commented = true;
            index++;
            continue;
        }

        if (content[index - 1] == '/' and content[index] == '/') {
            result.pop_back();
            while (content[index] != '\n') {
                index++;
            }
            continue;
        }

        if (content[index - 1] != '\\' and content[index] == '\"') {
            quoted = true;
            result += content[index];
            index++;
            continue;
        }

        result += content[index];
        index++;
    }
    return result;
}

vector<string> GetWords(const string& content) {
    string standalone = "{}:;()[],<>";
    auto IsStandalone = [&](char c) {
        for (char itr : standalone) {
            if (itr == c) {
                return true;
            }
        }
        return false;
    };


    string delimiters = "\t\n ";
    auto IsDelimiter = [&](char c) {
        for (char itr : delimiters) {
            if (itr == c) {
                return true;
            }
        }
        return false;
    };

    vector<string> words;
    string current_word = "";
    auto Add = [&]() {
        if (current_word.size() == 0) {
            return;
        }
        words.push_back(current_word);
        current_word = "";
    };


    for (char itr : content) {
        if (IsStandalone(itr)) {
            Add();
            current_word = itr;
            Add();
        } else if (IsDelimiter(itr)) {
            Add();
        } else {
            current_word += itr;
        }
    }

    Add();

    return words;
}

string ReadFromFile(const string& file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string file_information;

    if (fin) {
        fin.seekg(0, fin.end);
        file_information.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(& file_information[0], file_information.size());
        fin.close();
    } else {
        cerr << "Something went wrong while reading\n";
    }

    return file_information;
}

ClassBundle ParseClasses(const vector<string>& words) {
    map<string, Class> classes;
    
    string scope = "::";

    int index = 0;

    auto PopLastScope = [](string& txt) {
        txt.pop_back();
        txt.pop_back();
        while (txt.size() and txt.back() != ':') {
            txt.pop_back();
        }
    };

    function<void(bool)> Parse;

    Parse = [&](bool add) -> void {
        bool had_namespace = false;
        bool had_class = false;
        bool is_enum = false;
        string last_scope_name = "";      

        while (1) {
            if (index >= (int)words.size()) {
                return;
            }

            auto& current = words[index];

            if (current == "namespace" and words[index - 1] != "using") {
                had_namespace = true;
                last_scope_name = words[index + 1];
                index += 2;
                continue;
            }

            if (current == "class" || current == "struct" || current == "enum") {
                had_class = true;
                last_scope_name = words[index + 1];
                if (current == "enum") {
                    is_enum = true;
                }
                index += 2;
                continue;
            }

            if (current == "{") {
                index += 1;
                if (had_namespace || had_class) {
                    auto last_scope = scope;
                    scope += last_scope_name + "::";

                    if (had_class) {
                        if (classes.find(scope) == classes.end()) {
                            classes[scope] = Class({"", last_scope, last_scope_name, {}, is_enum});
                        } else {
                            had_class = false;
                        }
                    }

                    Parse(had_class);

                    had_namespace = had_class = is_enum = false;
                    PopLastScope(scope);
                } else {
                    Parse(false);
                }

                if (words[index] == ";") {
                    index += 1;
                }

                continue;
            }

            if (current == "}") {
                index += 1;
                return;
            }

            if (current == ";" and add) {
                // check if the object is good
                if (words[index - 1] == ")" or words[index - 1] == "const" or words[index - 1] == "0" or words[index - 1] == "default") {
                    index += 1;
                    continue;
                }

                int start_index = index - 1;
                while (words[start_index] != ")" and 
                        words[start_index] != ";" and 
                        words[start_index] != "{" and 
                        words[start_index] != "}" and
                        !(words[start_index] == ":" and words[start_index - 1] == "public") and
                        !(words[start_index] == ":" and words[start_index - 1] == "private") and
                        !(words[start_index] == ":" and words[start_index - 1] == "protected")) {
                    start_index -= 1;
                }
                
                start_index += 1;

                if (words[start_index] == "static") {
                    index += 1;
                    continue;
                }

                int first_val = start_index;
                int num_templates = 0;
                while ((words[first_val] != "," and words[first_val] != ";") or num_templates) {
                    if (words[first_val] == ">") {
                        num_templates -= 1;
                    } 

                    if (words[first_val] == "<") {
                        num_templates += 1;
                    }
                    first_val += 1;
                }
                first_val -= 1;

                std::string type = words[start_index] + " ";
                if (words[start_index] == ":") {
                    type.pop_back();
                }

                start_index += 1;
                while (start_index < first_val) {
                    if (words[start_index] == ":") {
                        if (words[start_index - 1] != ":") {
                            type.pop_back();
                        }
                        type += ":";
                    } else {
                        type += words[start_index] + " ";
                    }
                    start_index += 1;
                }

                type.pop_back();

                while (first_val < index) {
                    classes[scope].fields.push_back({type, words[first_val]});
                    first_val += 2;
                }

/*
                if (add and words[index - 1] != ")" and words[index - 1] != "const") {
                    auto name = words[index - 1];
                    auto type = words[index - 2];
                    auto _index = index - 3;
                    while (_index > 0) {
                        if (words[_index] == ":" and words[_index - 1] == ":") {
                            type = "::" + type;
                            _index -= 2;
                        } else {
                            break;
                        }
                        
                        if (words[_index].size() == 1) { // insane life-hack
                            break;
                        }

                        type = words[_index] + type;
                        _index -= 1;
                    }

                    classes[scope].fields.push_back({type, name});
                }
*/                
                index += 1;
                continue;
            }

            index += 1;
        }
    };

    Parse(false);

    ClassBundle cb;
    for (auto& itr : classes) {
        cb.all_classes.push_back(itr.second);
    }
    return cb;
}
