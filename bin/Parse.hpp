#ifndef AUTOJSON_BIN_PARSE_HPP
#define AUTOJSON_BIN_PARSE_HPP

#include "ASTElements.hpp"

#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

std::string EraseComments(const std::string &content) {
    std::string result;
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

std::vector<std::string> GetWords(const std::string &content) {
    std::string standalone = "{}:;()[],<>";
    auto IsStandalone = [&](char c) {
        for (char itr : standalone) {
            if (itr == c) {
                return true;
            }
        }
        return false;
    };


    std::string delimiters = "\t\n ";
    auto IsDelimiter = [&](char c) {
        for (char itr : delimiters) {
            if (itr == c) {
                return true;
            }
        }
        return false;
    };

    std::vector<std::string> words;
    std::string currentWord = "";
    auto Add = [&]() {
        if (currentWord.size() == 0) {
            return;
        }
        words.push_back(currentWord);
        currentWord = "";
    };


    for (char itr : content) {
        if (IsStandalone(itr)) {
            Add();
            currentWord = itr;
            Add();
        } else if (IsDelimiter(itr)) {
            Add();
        } else {
            currentWord += itr;
        }
    }

    Add();

    return words;
}

std::string ReadFromFile(const std::string& file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string fileInformation;

    if (fin) {
        fin.seekg(0, fin.end);
        fileInformation.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(& fileInformation[0], fileInformation.size());
        fin.close();
    } else {
        std::cerr << "Something went wrong while reading\n";
    }

    return fileInformation;
}

ClassBundle ParseClasses(const std::vector<std::string>& words) {
    std::map<std::string, Class> classes;

    std::string scope = "::";

    int index = 0;

    auto PopLastScope = [](std::string& txt) {
        txt.pop_back();
        txt.pop_back();
        while (txt.size() and txt.back() != ':') {
            txt.pop_back();
        }
    };

    std::function<void(bool)> Parse;

    Parse = [&](bool add) -> void {
        bool hadNamespace = false;
        bool hadClass = false;
        bool isEnum = false;
        std::string lastScopeName = "";

        while (1) {
            if (index >= (int)words.size()) {
                return;
            }

            auto& current = words[index];

            if (current == "namespace" and words[index - 1] != "using") {
                hadNamespace = true;
                lastScopeName = words[index + 1];
                index += 2;
                continue;
            }

            if (current == "class" || current == "struct" || current == "enum") {
                hadClass = true;
                lastScopeName = words[index + 1];
                if (current == "enum") {
                    isEnum = true;
                }
                index += 2;
                continue;
            }

            if (current == "{") {
                index += 1;
                if (hadNamespace || hadClass) {
                    auto lastScope = scope;
                    scope += lastScopeName + "::";

                    if (hadClass) {
                        if (classes.find(scope) == classes.end()) {
                            classes[scope] = Class({"", lastScope, lastScopeName, {}, isEnum});
                        } else {
                            hadClass = false;
                        }
                    }

                    Parse(hadClass);

                    hadNamespace = hadClass = isEnum = false;
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

                int startIndex = index - 1;
                while (words[startIndex] != ")" and
                        words[startIndex] != ";" and
                        words[startIndex] != "{" and
                        words[startIndex] != "}" and
                        !(words[startIndex] == ":" and words[startIndex - 1] == "public") and
                        !(words[startIndex] == ":" and words[startIndex - 1] == "private") and
                        !(words[startIndex] == ":" and words[startIndex - 1] == "protected")) {
                    startIndex -= 1;
                }

                startIndex += 1;

                if (words[startIndex] == "static") {
                    index += 1;
                    continue;
                }

                int firstVal = startIndex;
                int numTemplates = 0;
                while ((words[firstVal] != "," and words[firstVal] != ";") or numTemplates) {
                    if (words[firstVal] == ">") {
                        numTemplates -= 1;
                    }

                    if (words[firstVal] == "<") {
                        numTemplates += 1;
                    }
                    firstVal += 1;
                }
                firstVal -= 1;

                std::string type = words[startIndex] + " ";
                if (words[startIndex] == ":") {
                    type.pop_back();
                }

                startIndex += 1;
                while (startIndex < firstVal) {
                    if (words[startIndex] == ":") {
                        if (words[startIndex - 1] != ":") {
                            type.pop_back();
                        }
                        type += ":";
                    } else {
                        type += words[startIndex] + " ";
                    }
                    startIndex += 1;
                }

                type.pop_back();

                while (firstVal < index) {
                    classes[scope].fields.push_back({type, words[firstVal]});
                    firstVal += 2;
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
        cb.allClasses.push_back(itr.second);
    }
    return cb;
}

#endif // AUTOJSON_BIN_PARSE_HPP
