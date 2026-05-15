
#ifndef OUTPUT_H
#define OUTPUT_H

#include "Graph.h"
#include "Web.h"
#include "GraphColoring.h"
#include <string>
#include <vector>

class Output {
public:

    static void gerarOutput(
        const std::string& filename,
        const std::vector<Web*>& webs_todos,
        Graph<Web>* grafo_final,
        int num_registos,
        const std::vector<Vertex<Web>*>& spillados = {},
        const std::vector<SplitInfo>& splits = {}
    );

    static void printResumo(
        const std::vector<Web*>& webs_todos,
        Graph<Web>* grafo_final,
        int num_registos,
        const std::vector<Vertex<Web>*>& spillados = {},
        const std::vector<SplitInfo>& splits = {}
    );
};

#endif
