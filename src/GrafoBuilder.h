
#ifndef GRAFOBUILDER_H
#define GRAFOBUILDER_H

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
class GrafoBuilder {
public:

    /**
    * @brief Creates webs from live ranges.
    * @param ranges Map containing live ranges grouped by variable.
    * @return Vector containing pointers to the generated webs.
    */
    static std::vector<Web*> criarWebs(
        std::map<std::string, std::vector<LiveRange>>& ranges);


    /**
    * @brief Builds the interference graph from a set of webs.
    * @param webs Vector containing all generated webs.
    * @return Pointer to the generated interference graph.
    */
    static Graph<Web>* construirGrafo(std::vector<Web*>& webs);


    static void printGrafo(Graph<Web>* grafo, const std::vector<Web*>& webs);


    static void printMatriz(Graph<Web>* grafo, const std::vector<Web*>& webs);

private:

    /**
    * @brief Checks whether two live ranges overlap.
    * @param lr1 First live range.
    * @param lr2 Second live range.
    * @return true if the live ranges overlap.
    */
    static bool sobrepoe(const LiveRange& lr1, const LiveRange& lr2);
};

#endif
