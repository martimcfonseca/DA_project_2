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
 */
class GraphColoring {
    public:
    /**
    * @brief Colors a graph using spilling strategy.
    * @param graph Interference graph.
    * @param num Number of available registers.
    * @return List of spilled vertices.
    */
    static std::vector<Vertex<Web>*> colorGraphSpilling(Graph<Web>* &graph, int num);
    /**
     * @brief Colors a graph using a standard greedy coloring algorithm.
     * @param graph Interference graph.
     * @param num Number of available registers.
     * @return true if coloring succeeded without conflicts.
     */
    static bool colorGraphNormal(Graph<Web>* &graph, int num);
    /**
     * @brief Colors a graph using splitting strategy.
     * @param graph Interference graph.
     * @param num Number of registers.
     * @param max_splits Maximum number of allowed splits.
     * @param splits_info Stores metadata about performed splits.
     * @param webs_todo List of all webs involved in processing.
     * @return New graph after splitting operations.
     */
    static Graph<Web>* colorGraphSplitting(Graph<Web>*& graph,int num,int max_splits,std::vector<SplitInfo>& splits_info,std::vector<Web*>& all_webs);

    private:
    /**
    * @brief Selects candidate webs to be split
    * @param graph Interference graph.
    * @param k Number of webs to select.
    * @return List of selected vertices.
    */
    static std::vector<Vertex<Web>*> chooseWebsForSplit(
        Graph<Web>* graph,
        int k
    );


    /**
     * @brief Splits a web into smaller parts.
     * @param original Web to split.
     * @param proximo_id Next available web identifier.
     * @return Vector of resulting webs.
     */
    static std::vector<Web*> divideWeb(Web* original, int& next_id);

    /**
     * @brief Rebuilds the graph after splitting operations.
     * @param grafo_original Original graph.
     * @param splits Split metadata.
     * @param todos_webs All webs after splitting.
     * @return Reconstructed graph.
     */
    static Graph<Web>* rebuildGraph(Graph<Web>* original_graph,const std::vector<SplitInfo>& splits,const std::vector<Web*>& all_webs);

};

#endif //REGISTERALLOCATION_GRAPHCOLORING_H