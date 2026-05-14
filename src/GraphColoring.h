//
// Created by marti on 11/05/2026.
//

#ifndef REGISTERALLOCATION_GRAPHCOLORING_H
#define REGISTERALLOCATION_GRAPHCOLORING_H
#include "Graph.h"
#include "Web.h"

class GraphColoring {
    public:
    static std::vector<Vertex<Web>*> colorGraphSpilling(Graph<Web>* &graph, int num);
    static bool colorGraphNormal(Graph<Web>* &graph, int num);
};

#endif //REGISTERALLOCATION_GRAPHCOLORING_H