//
// Created by marti on 11/05/2026.
//

#ifndef REGISTERALLOCATION_GRAPHCOLORING_H
#define REGISTERALLOCATION_GRAPHCOLORING_H
#include "Graph.h"
#include "Web.h"


struct SplitInfo {
    Web* original;              // Web original
    std::vector<Web*> partes;   // Partes resultantes do split
    int ponto_divisao;          // Linha onde foi dividido
};


class GraphColoring {
    public:
    static std::vector<Vertex<Web>*> colorGraphSpilling(Graph<Web>* &graph, int num);
    static bool colorGraphNormal(Graph<Web>* &graph, int num);
    static Graph<Web>* colorGraphSplitting(Graph<Web>*& graph,int num,int max_splits,std::vector<SplitInfo>& splits_info,std::vector<Web*>& webs_todo);

    private:
    static std::vector<Vertex<Web>*> escolherWebsParaSplit(
        Graph<Web>* graph,
        int k
    );

    static std::vector<Web*> dividirWeb(Web* original, int& proximo_id);

    static Graph<Web>* reconstruirGrafo(Graph<Web>* grafo_original,const std::vector<SplitInfo>& splits,const std::vector<Web*>& todos_webs);

};

#endif //REGISTERALLOCATION_GRAPHCOLORING_H