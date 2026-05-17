//
// Created by marti on 11/05/2026.
//

#ifndef REGISTERALLOCATION_GRAPHCOLORING_H
#define REGISTERALLOCATION_GRAPHCOLORING_H
#include "Graph.h"
#include "Web.h"

/**
 * @struct SplitInfo
 * @brief Stores information about a web splitting operation.
 *
 * Used in graph coloring with splitting strategies,
 * where a single web can be divided into multiple smaller webs
 * to reduce register pressure.
 */
struct SplitInfo {
    Web* original;              // Web original
    std::vector<Web*> parts;   // Partes resultantes do split
    int point_division;          // Linha onde foi dividido
};

/**
 * @class GraphColoring
 * @brief Provides graph coloring algorithms for register allocation.
 *
 * Implements different strategies:
 * - normal coloring
 * - spilling
 * - splitting-based coloring
 * - free register
 */
class GraphColoring {
    public:

    static std::vector<Vertex<Web>*> colorGraphSpilling(Graph<Web>* &graph, int num);

    static bool colorGraphNormal(Graph<Web>* &graph, int num);

    static Graph<Web>* colorGraphSplitting(Graph<Web>*& graph,int num,int max_splits, std::vector<Vertex<Web>*>& spilled, std::vector<SplitInfo>& splits_info,std::vector<Web*>& all_webs);

    static std::vector<Vertex<Web>*> freeRegisterAssign(Graph<Web>* graph, int num);
    private:

    static std::vector<Vertex<Web>*> chooseWebsForSplit(
        Graph<Web>* graph,
        int k
    );



    static std::vector<Web*> divideWeb(Web* original, int& next_id);


    static Graph<Web>* rebuildGraph(Graph<Web>* original_graph,const std::vector<SplitInfo>& splits,const std::vector<Web*>& all_webs);


};

#endif //REGISTERALLOCATION_GRAPHCOLORING_H