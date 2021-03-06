Lets take a look at [csacademy](www.csacamy.com)'s [graph editor](https://csacademy.com/app/graph_editor/) structure and build a C++ program that stores that information in `structs` so it can be manipulated easily. The whole reasoning for this is to showcase a way to integrate JSONs in a C++ program without changing/adding code using autojson's feature to auto generate JSON serialization/deserialization code. This may be needed sometimes since the data needs to be serialised or sent to another backend. JSONs are not the best way to do this, but they're by far the easiest way(in other languages).
 
We'll start by declaring our `graph`'s structure.
##### *For this example, the struct's fields will have snake case and the JSON fields will have camel case, since camel case is the standard for JSON field naming.
```
struct GraphPlot {
    struct Node {
        /// coordinates of the node
        int x_pos;
        int y_pos;
    };
    

    struct Edge {
        string source;
        string target;
        int is_directed;
    };
    
    /// size of the canvas
    int x_size;
    int y_size;
    

    vector<Edge> all_edges;
    map<string, Node> node_info;
};
```

We have a GraphPlot which has a list of edges and a map nodes, where each node has it's coordinates in the plot.

The node struct will look like this:
```
struct Node {
    /// coordinates of the node
    int x_pos;
    int y_pos;
    
    /// default constructor
    Node() { }
};
```

Running json-maker on graph_plot.h will create a file graph_plot_json_impl.h which can be included anywhere and will provide serialization/deserialization support for all classes inside graph_plot.h file.

To populate your objects with the information from a JSON it's as easy as
```
Json j = (Json::ReadFromFile(argv[1]));
GraphPlot p(j);
```

And you have your information in the GraphPlot struct with minimal effort and code changed/inserted.
