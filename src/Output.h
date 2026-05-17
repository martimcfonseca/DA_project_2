
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


    static void generateOutput(
        const std::string& filename,
        const std::vector<Web*>& webs_all,
        Graph<Web>* graph_final,
        int num_register,
        int parameters,
        const std::vector<Vertex<Web>*>& spilled = {},
        const std::vector<SplitInfo>& splits = {}
    );

    static void printResume(
        const std::vector<Web*>& webs_all,
        Graph<Web>* graph_final,
        int num_registers,
        const std::vector<Vertex<Web>*>& spilled = {},
        const std::vector<SplitInfo>& splits = {}
    );
};

#endif
