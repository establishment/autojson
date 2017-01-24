AutoJSON - An easy way to make your C++ backends JSON compatible
================================================================

Overview
--------
AutoJSON is composed of two main parts. A very powerful JSON library that brings python accessibility to C++ and a tool to auto-generate serialization and deserialization methods for specified C++ classes with minimal effort.
  
  
It's no news that there are a lot of JSON libraries for C++ out there. This one was born in the need to write better backends in C++ which comunicate to other servicies writen in python in a decent way without using something heavy like [protocol buffers](https://github.com/google/protobuf).

Design goals
------------
There are a lot of JSON libraries out there, no news here, but this one was create so you can use it and forget about it. There are X main design goals that stand at the base of the library:

- **Easy to use**. Create inline JSONs and modify them like any stl container.

- **Good auto-generated code**. Create JSON compatible C++ classes with 0 code. Just run the JSON creator binary on target classes and the serialization and deserialization methods will be created in a $name_json_impl.h file that can be included anywhere and can be shipped anywhere if the JSON library is present. No binary needed anymore.   

FAQ
---

#### What flags do I need to compile this?

```
-std=c++11
```

#### Do I need to change my compile command to include the auto-generated files?

No.

Just #include them

Installation
------------

```sh
git clone https://github.com/establishment/autojson.git
cd autojson
make install
```

Examples
--------
### Declare a JSON anywhere using initializer lists

```cpp
Json j = {
    {"id", 1},
    {"name", "autojson"},
    {"tags", {
            "github", "json", "c++"
        }
    }
};
```

And the JSON will look like:

```json
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

or it can be minified

```json
{"id":1,"name":"autojson","tags":["github","json","c++"]}
```

### Parsing JSONs

```cpp
/// read JSON from file
Json file_json = Json::ReadFromFile("config.json");

/// create a string containing a JSON
string stringified_json = file_json.Stringify();

/// create a JSON object from a string
Json text_json = Json::Parse(stringified_json);
```

### Retrieving information from JSONs
Same format as with python or javascript

```cpp
Json j = Json::ReadFromFile("config.json");
int id = j["id"];
string name = j["name"];
vector<string> tags = j["tags"]
```

### Give a default-value to a field that may be missing

```cpp
Json j;
...
// if j has the field "id", that value will be returned
// else, it will be 100
int id = j["id"] | 100;

vector<string> tags = j["tags"] | {"no_tags_found", "why there are no tags?};

int score = j["very"]["nested"]["json"] | 100;

// both | and || work
int priority = j["priority"] || -1;
```

### Putting information in JSONs

```cpp
#include <json>
#include <iostream>
using namespace std;

int main() {
    Json j;
    
    j["id"] = 132;

    vector<int> v = {1, 2, 3, 4, 5};
    j["values"] = v;
       
    int num_values = j["values"].size(); /// num_values = 5

    // working even with pointers
    map<string, int*> m = {
        {"one", new int(1)},
        {"two", new int(2)}
    };
    
    j["map pointers"] = m;
    
    for (int i = 0; i < 10; i += 2) {
        j["even numbers"].push_back(i);
    }

    string result = j.Stringify();

    cout << result << '\n';
}
```

#### Output

```json
{
    "even numbers":[
        0,
        2,
        4,
        6,
        8
    ],
    "id":132,
    "map pointers":{
        "one":1,
        "two":2
    },
    "num_values":5,
    "values":[
        1,
        2,
        3,
        4,
        5
    ]
}
```

### For-based loops inside JSONs
If your JSON/field is an array for-based loops can be used to iterate over it. 

```cpp
Json j;
// j["numbers"] is an array of numbers 
for (int number : j["numbers"]) {
    cout << number << '\t';
}

// j["tags"] is an array of strings
for (string tag : j["tags"]) {
    cout << "Got tag:" << tag << '\n';
}

// j["objects"] is an array of jsons
for (Json itr : j["objects"]) {
    int id = itr["id"];
    string name = itr["name"];
}

// or use auto inside the for-loop
for (auto itr : j["objects"]) { }

// even references to change the information
for (auto& itr : j["objects"] { }

// or const references so nothings gets copied or changed
for (const auto& itr : j["objects"]) { }
```
