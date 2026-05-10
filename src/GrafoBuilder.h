
#ifndef GRAFOBUILDER_H
#define GRAFOBUILDER_H

#include "Graph.h"
#include "Web.h"
#include "LiveRange.h"
#include <map>
#include <vector>


class GrafoBuilder {
public:

    static std::vector<Web*> criarWebs(
        std::map<std::string, std::vector<LiveRange>>& ranges);


    static Graph<Web>* construirGrafo(std::vector<Web*>& webs);


    static void printGrafo(Graph<Web>* grafo, const std::vector<Web*>& webs);


    static void printMatriz(Graph<Web>* grafo, const std::vector<Web*>& webs);

private:
    
    static bool sobrepoe(const LiveRange& lr1, const LiveRange& lr2);
};

#endif
