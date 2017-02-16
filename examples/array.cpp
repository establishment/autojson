#include <json>
#include <iostream>
using namespace std;

int main() {
    Json j = {1, 2, 3, 4, {{"1", 2}, {"2", "3"}}};

    string s = j;

    cout << s << '\n';
    return 0;
}
