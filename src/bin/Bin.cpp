#include <algorithm>

#include "CodeGenerator.hpp"
#include "Parse.hpp"

using namespace std;

void SolveFile(const string& file) {
    auto content = ReadFromFile(file);
/*
    cerr << "~~~~~~~~~~~~~~~~~~~~~~\n";
    cerr << EraseComments(content) << '\n';
    cerr << "~~~~~~~~~~~~~~~~~~~~~~\n";
    cerr << "\n\n\n\n\n\n\n";
*/
    content = EraseComments(content);
    auto words = GetWords(content);
    ClassBundle cb = ParseClasses(words);

    cb.upgradeScopes();

    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "Found the following classes\n";
    for (auto itr : cb.allClasses) {
        auto scope = itr.scope;
        auto cls = itr;
        cout << "Name:\t" << cls.name << '\n';
        cout << "Scope:\t" << cls.scope << '\n';
        cout << "~~~~~~~\n";
        for (auto itr : cls.fields) {
            cout << "\t" << itr.type << '\t' << itr.name << '\n';
        }
        cout << "<--------->\n\n\n";
    }

    sort(cb.allClasses.begin(), cb.allClasses.end(), [](const auto& a, const auto& b) { return a.scope.size() > b.scope.size(); });
    string fileContent = "";

    {
        std::string shortFile = file;
        int start = 0;

        for (int i = 0; i < (int)file.size(); i += 1) {
            if (file[i] == '/') {
                start = i;
            }
        }

        fileContent += "#include \"" + file.substr(start, file.size()) + "\"\n"
                        "#include <JSON>\n\n";
    }

    for (auto itr : cb.allClasses) {
        fileContent += JSONifyClass(itr);
    }

    string finalFile = file;
    string remainingFile = "";

    while (finalFile.size()) {
        remainingFile += finalFile.back();
        if (finalFile.back() == '.') {
            finalFile.pop_back();
            break;
        }

        finalFile.pop_back();
    }

    finalFile += "JSONImpl";
    reverse(remainingFile.begin(), remainingFile.end());
    finalFile += remainingFile;

    ofstream fout(finalFile);
    fout << fileContent << '\n';
    fout.close();
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i += 1) {
        SolveFile(argv[i]);
    }
}
