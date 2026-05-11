//
// Created by Martim on 10/05/2026.
//

#include <iostream>
#include <string>

#include "GrafoBuilder.h"
#include "GraphColoring.h"
#include "Parser.h"
using namespace std;

int main(int argc, char *argv[]) {
    //cout << "path of the ranges file: ";
    string ranges_file,config;
    //cin >> ranges_file;
    ranges_file = "C:/Users/marti/Documents/DA/DA_project_2/basic/ranges/ranges6.txt";
    //cout << "path of the config file: ";
    //cin >> config;
    config = "C:/Users/marti/Documents/DA/DA_project_2/basic/registers/registers1.txt";

    map<string, vector<LiveRange>> ranges = Parser::parseRanges(ranges_file);
    int num,parameter;
    string algorithm;
    Parser::parseConfig(config, num, algorithm, parameter);
    auto webs = GrafoBuilder::criarWebs(ranges);
    auto graph = GrafoBuilder::construirGrafo(webs);
    auto spills = GraphColoring::colorGraphNormal(graph,3);
    GrafoBuilder::printGrafo(graph,webs);

    for (auto v : graph->getVertexSet())
        if (v->getColor() >= 0)
            cout << v->getInfo().id << " → r" << v->getColor() << "\n";

    if (spills.empty()) cout << "Nothing to Spill\n";
    for (auto v : spills)
        cout << v->getInfo().id << " → SPILL\n";
}
