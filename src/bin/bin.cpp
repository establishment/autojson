#include <algorithm>
#include "code_generator.hpp"
#include "parse.hpp"
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

    cb.UpgradeScopes();

    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "Found the following classes\n";
    for (auto itr : cb.all_classes) {
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

    sort(cb.all_classes.begin(), cb.all_classes.end(), [](const auto& a, const auto& b) { return a.scope.size() > b.scope.size(); });
    string file_content = "";

    {
        std::string short_file = file;
        int start = 0;
        
        for (int i = 0; i < (int)file.size(); i += 1) {
            if (file[i] == '/') {
                start = i;
            }
        }

        file_content += "#include \"" + file.substr(start, file.size()) + "\"\n"
                        "#include <json>\n\n";
    }

    for (auto itr : cb.all_classes) {
        file_content += JsonifyClass(itr);
    }

    string final_file = file;
    string remaining_file = "";

    while (final_file.size()) {
        remaining_file += final_file.back();
        if (final_file.back() == '.') {
            final_file.pop_back();
            break;
        }

        final_file.pop_back();
    }

    final_file += "_json_impl";
    reverse(remaining_file.begin(), remaining_file.end());
    final_file += remaining_file;

    ofstream fout(final_file);
    fout << file_content << '\n';
    fout.close();
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i += 1) {
        SolveFile(argv[i]);        
    }
}

