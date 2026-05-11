// main.cpp
#include "Parser.h"
#include "GrafoBuilder.h"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "Uso: " << argv[0]
                 << " <ranges.txt> <registers.txt>" << std::endl;
        return 1;
    }

    std::string rangesFile = argv[1];
    std::string registersFile = argv[2];

    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   ALOCAÇÃO DE REGISTOS - COMPILADOR   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    // 1. PARSING
    auto ranges = Parser::parseRanges(rangesFile);

    int numRegistos = 0;
    std::string algoritmo;
    int parametro = 0;
    Parser::parseConfig(registersFile, numRegistos, algoritmo, parametro);

    // 2. CRIAR WEBS
    std::vector<Web*> webs = GrafoBuilder::criarWebs(ranges);

    // 3. CONSTRUIR GRAFO
    Graph<Web>* grafo = GrafoBuilder::construirGrafo(webs);

    // 4. VISUALIZAR
    GrafoBuilder::printGrafo(grafo, webs);
    GrafoBuilder::printMatriz(grafo, webs);

    // Resumo
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║              RESUMO                    ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Webs criados:     " << std::setw(3) << webs.size()
              << "                  ║\n";
    std::cout << "║ Registos disponíveis: " << std::setw(3) << numRegistos
              << "              ║\n";
    std::cout << "║ Algoritmo:        " << std::setw(17) << std::left
              << algoritmo << "║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    // Cleanup
    delete grafo;
    for (Web* web : webs) {
        delete web;
    }

    return 0;
}