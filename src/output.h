
#ifndef OUTPUT_H
#define OUTPUT_H

#include "Graph.h"
#include "Web.h"
#include "GraphColoring.h"
#include <string>
#include <vector>

/**
 * @class Output
 * @brief Responsible for generating final reports and summaries.
 *
 * This class formats and outputs the results of the register allocation
 * process, including graph coloring, spills, and splits.
 */

class Output {
public:


    /**
    * @brief Generates a complete output file with allocation results.
    * @param filename Output file name.
    * @param webs_todos All webs processed in the system.
    * @param grafo_final Final interference graph.
    * @param num_registos Number of available registers.
    * @param spillados List of spilled vertices.
    * @param splits Metadata about split operations.
    */
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
