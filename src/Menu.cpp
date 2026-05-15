/**
* @file Menu.cpp
 * @brief User interface for register allocation tool.
 *
 * Provides an interactive menu to:
 * - Load live ranges
 * - Load configuration (registers + algorithm)
 * - Execute register allocation
 */


#include <iostream>
#include <limits>
#include "Menu.h"
#include "GraphColoring.h"
#include "GrafoBuilder.h"
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
 * @param algoritmo Selected algorithm ("basic", "spilling", "splitting").
 * @param parametros Extra parameter (used for splitting depth).
 * @param rangesF Flag indicating if ranges file was loaded.
 * @param registersF Flag indicating if config file was loaded.
 */
void runMenu( std::map<std::string, std::vector<LiveRange>> ranges,int numRegisters,std::string algoritmo,int parametros,bool rangesF, bool registersF ) {
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
                    Parser::parseConfig(filename, numRegisters, algoritmo, parametros);
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
                    std::vector<Web*> webs = GrafoBuilder::criarWebs(ranges);
                    Graph<Web>* grafo = GrafoBuilder::construirGrafo(webs);
                    std::vector<Vertex<Web>*> spillados;
                    std::vector<SplitInfo> splits;
                    Graph<Web>* grafo_final = grafo;
                    bool sucesso = false;

                    if (algoritmo == "basic") {
                        sucesso = GraphColoring::colorGraphNormal(grafo, numRegisters);



                    } else if (algoritmo == "spilling") {
                        spillados = GraphColoring::colorGraphSpilling(grafo, numRegisters);


                    } else if (algoritmo == "splitting") {
                        grafo_final = GraphColoring::colorGraphSplitting(
                            grafo,
                            numRegisters,
                            parametros,
                            splits,
                            webs
                        );

                        sucesso = (grafo_final != nullptr);

                    } else {
                        std::cerr << "Algoritmo desconhecido: " << algoritmo << std::endl;
                    }

                    Output::gerarOutput("output.txt",
                        webs,
                        grafo_final,
                        numRegisters,
                        spillados,
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
