
#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include "Graph.h"
#include "Web.h"
#include "LiveRange.h"
#include <map>
#include <vector>

/**
 * @class GrafoBuilder
 * @brief Responsible for building webs and interference graphs.
 *
 * This class provides utility methods for:
 * - grouping live ranges into webs,
 * - constructing the interference graph,
 * - printing graph representations.
 *
 * The generated graph is later used in register allocation
 * and interference analysis algorithms.
 */
class GraphBuilder {
public:


    static std::vector<Web*> createWebs(
        std::map<std::string, std::vector<LiveRange>>& ranges);

    static Graph<Web>* buildGraph(std::vector<Web*>& webs);


    static void printGraph(Graph<Web>* graph, const std::vector<Web*>& webs);


    static void printMatrix(Graph<Web>* graph, const std::vector<Web*>& webs);

private:

    static bool overlaps(const LiveRange& lr1, const LiveRange& lr2);
};

#endif
