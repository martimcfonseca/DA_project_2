//
// Created by marti on 11/05/2026.
//

#ifndef REGISTERALLOCATION_GRAPHCOLORING_H
#define REGISTERALLOCATION_GRAPHCOLORING_H
#include "Graph.h"
#include "Web.h"


struct SplitInfo {
    Web* original;              // Web original
    std::vector<Web*> parts;   // Partes resultantes do split
    int point_division;          // Linha onde foi dividido
};


class GraphColoring {
    public:
    static std::vector<Vertex<Web>*> colorGraphSpilling(Graph<Web>* &graph, int num);
    static bool colorGraphNormal(Graph<Web>* &graph, int num);
    static Graph<Web>* colorGraphSplitting(Graph<Web>*& graph,int num,int max_splits,std::vector<SplitInfo>& splits_info,std::vector<Web*>& all_webs);

    private:
    static std::vector<Vertex<Web>*> chooseWebsForSplit(
        Graph<Web>* graph,
        int k
    );

    static std::vector<Web*> divideWeb(Web* original, int& next_id);

    static Graph<Web>* rebuildGraph(Graph<Web>* original_graph,const std::vector<SplitInfo>& splits,const std::vector<Web*>& all_webs);

};

#endif //REGISTERALLOCATION_GRAPHCOLORING_H