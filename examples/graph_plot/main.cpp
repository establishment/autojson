#include "graph_plot.h"
#include "graph_plot_json_impl.h"

#include <json>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Provide 1 file\n";
        exit(0);
    }

    Json j = (Json::ReadFromFile(argv[1]));

    GraphPlot p(j);

    cout << Json(p).Stringify(false) << '\n';
}
