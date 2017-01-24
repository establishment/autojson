#pragma once
#include <json>

#include <map>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::map;

struct GraphPlot {
    struct Node {
        /// coordinates of the node
        int x_pos;
        int y_pos;

        Node() { }
        Node(Json& j);
    };

    struct Edge {
        string source;
        string target;
        int is_directed;

        Edge() { }
        Edge(Json& j);
    };

    /// size of the canvas
    int x_size;
    int y_size;

    vector<Edge> all_edges;
    map<string, Node> node_info;

    GraphPlot() : x_size(500), y_size(500), all_edges(), node_info() { }

    GraphPlot(Json& j);
};
