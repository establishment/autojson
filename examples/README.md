Your first build
------------------

**simple_example.cpp**

*The file can be found in this folder 
```
#include <autojson>
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
```

**Compile and run**
```
g++ -std=c++11 simple_example.cpp -o exec
./exec
```

**output**
```
{"id":1,"name":"autojson","tags":["github","json","c++"]}

or the formated version with Stringify(false)

{
    "id":1,
    "name":"autojson",
    "tags":[
        "github",
        "json",
        "c++"
    ]
}
```