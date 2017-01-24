#include <json>
#include <iostream>
using namespace std;

int main() {
    Json j;
    
    j["id"] = 132;

    vector<int> v = {1, 2, 3, 4, 5};
    j["values"] = v;
       
    int num_values = j["values"].size(); /// num_values = 5
    j["num_values"] = num_values;

    // working even with pointers
    map<string, int*> m = {
        {"one", new int(1)},
        {"two", new int(2)}
    };
    
    j["map pointers"] = m;
    
    for (int i = 0; i < 10; i += 2) {
        j["even numbers"].push_back(i);
    }

    cout << j.Stringify(false) << '\n';
}
