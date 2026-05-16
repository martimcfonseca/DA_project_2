

#include <iostream>
#include <limits>
#include "Menu.h"
#include "GraphColoring.h"
#include "GraphBuilder.h"
#include "output.h"
#include "Graph.h"




void printMenu() {
    std::cout << "\n==========================" << endl;
    std::cout << "  Alocated registers Tool  "   << endl;
    std::cout << "==========================" << endl;
    std::cout << "1. Carregar ranges file"      << endl;
    std::cout << "2. carregar registers file"          << endl;
    std::cout << "3. Alocated register"           << endl;
    std::cout << "0. Sair"                       << endl;
    std::cout << "==========================" << endl;
    std::cout << "Opcao: ";
}


/**
 * @brief Runs the interactive menu loop.
 *
 * This function controls the full workflow:
 * - parsing input files
 * - building interference graph
 * - executing selected coloring algorithm
 * - generating output
 *
 * Time Complexity:
 * - Depends on selected algorithm:
 *   - Basic coloring: O(v + e)
 *   - Spilling: O(v² + e)
 *   - Splitting: potentially exponential (worst case)
 *
 * Where:
 * - v = number of webs
 * - e = number of interference edges
 *
 * Space Complexity:
 * - O(v + e)
 *
 * @param ranges Live ranges grouped by variable.
 * @param numRegisters Number of available registers.
 * @param algorithm Selected algorithm ("basic", "spilling", "splitting").
 * @param parameters Extra parameter (used for splitting depth).
 * @param rangesF Flag indicating if ranges file was loaded.
 * @param registersF Flag indicating if config file was loaded.
 */
void runMenu( std::map<std::string, std::vector<LiveRange>> ranges,int numRegisters,std::string algorithm,int parameters,bool rangesF, bool registersF ) {
    int choice;
    do {
        printMenu();
        cin >> choice;


        switch (choice) {
            case 1: {
                string filename;
                cout << "Caminho do ficheiro: ";
                cin >> filename;
                try {
                    ranges  = Parser::parseRanges(filename);
                    rangesF = true;
                    cout << "Ficheiro carregado com sucesso!" << endl;
                } catch (const std::exception& e) {
                    std::cerr << "Erro: " << e.what() << std::endl;
                }
                break;
            }
            case 2: {
                string filename;
                cout << "Caminho do ficheiro: ";
                cin >> filename;
                try {
                    Parser::parseConfig(filename, numRegisters, algorithm, parameters);
                    registersF = true;
                    cout << "Ficheiro carregado com sucesso!" << endl;
                } catch (const std::exception& e) {
                    std::cerr << "Erro: " << e.what() << std::endl;
                }
                break;
            }
            case 3: {
                if (!(rangesF && registersF)) {
                    if (!(rangesF)) {
                        cout << "carregar primeiro ficheiro ranges" << endl;
                        break;
                    }
                    if (!(registersF)) {
                        cout << "carregar primeiro ficheiro registers" << endl;
                        break;;
                    }
                }
                else {
                    std::vector<Web*> webs = GraphBuilder::createWebs(ranges);
                    Graph<Web>* graph = GraphBuilder::buildGraph(webs);
                    std::vector<Vertex<Web>*> spilled;
                    std::vector<SplitInfo> splits;
                    Graph<Web>* graph_final = graph;
                    bool sucess = false;

                    if (algorithm == "basic") {
                        sucess = GraphColoring::colorGraphNormal(graph, numRegisters);



                    } else if (algorithm == "spilling") {
                        spilled = GraphColoring::colorGraphSpilling(graph, numRegisters);


                    } else if (algorithm == "splitting") {
                        graph_final = GraphColoring::colorGraphSplitting(
                            graph,
                            numRegisters,
                            parameters,
                            splits,
                            webs
                        );

                        sucess = (graph_final != nullptr);

                    } else if (algorithm == "free") {
                        spilled = GraphColoring::linearScan(graph,numRegisters);
                    }

                    else {
                        std::cerr << "Algoritmo desconhecido: " << algorithm << std::endl;
                    }

                    Output::generateOutput("output.txt",
                        webs,
                        graph_final,
                        numRegisters,
                        spilled,
                        splits);

                }
                break;
            }
            case 0:
                cout << "A sair..." << endl;
                break;
            default:
                cout << "Opcao invalida!" << endl;
        }

    } while (choice != 0);
}
