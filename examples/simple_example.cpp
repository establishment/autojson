#include <json>
#include <iostream>

int main() {
    Json j = {
        {"id", 1},
        {"name", "autojson"},
        {"tags", {
                "github", "json", "c++"
            }
        }
    };
 
    /// do not shrink the JSON - Stringify(false) - default = true
    std::cout << j.Stringify(false) << '\n';
}
