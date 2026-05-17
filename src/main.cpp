/**
 * @mainpage Register Allocation Tool
 *
 * @section intro_sec Introduction
 *
 * This project implements a Register Allocation system based on
 * graph coloring techniques.
 *
 * It reads live range information from input files, constructs
 * interference graphs, groups live ranges into webs, and assigns
 * registers using different strategies:
 *
 * - Basic graph coloring
 * - Spilling-based allocation
 * - Splitting-based allocation
 * - Free-register dynamic allocation
 *
 * The goal is to assign a limited number of CPU registers while
 * minimizing memory spills and handling register pressure efficiently.
 *
 * @section usage_sec Usage
 *
 * - Run the program in a terminal.
 * - Load a live ranges file (variable definitions and usages).
 * - Load a configuration file (number of registers + algorithm).
 * - Choose the allocation option from the menu.
 * - Generate output with final register assignment.
 *
 * Supported algorithms:
 * - "basic" → standard graph coloring
 * - "spilling" → removes high-degree nodes (spill to memory)
 * - "splitting" → splits webs to reduce interference
 * - "free-register" → dynamically assigns free registers during execution
 *
 * @section structure_sec Code Structure
 *
 * The project is organized as follows:
 *
 * - \ref Parser "Parser.h/cpp"
 *   - Reads input files (live ranges + configuration)
 *
 * - \ref LiveRange "LiveRange.h/cpp"
 *   - Represents variable live ranges in the program
 *
 * - \ref Web "Web.h/cpp"
 *   - Groups overlapping live ranges into interference units
 *
 * - \ref GrafoBuilder "GrafoBuilder.h/cpp"
 *   - Builds the interference graph from webs
 *
 * - \ref Graph "Graph.h/cpp"
 *   - Generic graph implementation used for interference graph
 *
 * - \ref GraphColoring "GraphColoring.h/cpp"
 *   - Implements register allocation algorithms:
 *     - greedy coloring
 *     - spilling
 *     - splitting
 *
 * - \ref Output "Output.h/cpp"
 *   - Generates final report and formatted output file
 *
 * - \ref Menu "Menu.h/cpp"
 *   - Provides CLI menu interface for user interaction
 *
 * - main.cpp
 *   - Program entry point
 *
 * @section complexity_sec Complexity Overview
 *
 * Main algorithmic complexities:
 *
 * - Live range parsing: O(n)
 * - Web construction: O(n²)
 * - Graph construction: O(w²)
 * - Greedy coloring: O(v + e)
 * - Spilling strategy: O(v² + e)
 * - Splitting strategy: potentially O(v² * k)
 *
 * Where:
 * - n = number of input live ranges
 * - w = number of webs
 * - v = graph vertices
 * - e = graph edges
 * - k = number of split iterations
 *
 * @section notes_sec Notes
 *
 * - All core algorithms are documented with time complexity.
 * - The system is designed for compiler register allocation simulation.
 * - Web construction and splitting are heuristics to improve coloring results.
 */
#include "Parser.h"
#include "GraphBuilder.h"
#include "GraphColoring.h"
#include "Output.h"
#include <iostream>

#include "Menu.h"


int main(int argc, char* argv[]) {


     if (argc == 5 && std::string(argv[1]) == "-b") {   //Modo batch "-b"
         std::string rangesFile = argv[1];
         std::string registersFile = argv[2];
         std::string outputFile = argv[3];
         try {
             auto ranges = Parser::parseRanges(rangesFile);
             int numRegistos = 0;
             std::string algoritmo;
             int parametro = 0;
             Parser::parseConfig(registersFile, numRegistos, algoritmo, parametro);
             std::vector<Web*> webs = GraphBuilder::createWebs(ranges); //criar as webs
             Graph<Web>* grafo = GraphBuilder::buildGraph(webs); //criação do grafo atraves das ranges

             std::vector<Vertex<Web>*> spillados;
             std::vector<SplitInfo> splits;
             Graph<Web>* grafo_final = grafo;
             bool sucesso = false;

             if (algoritmo == "basic") {
                 sucesso = GraphColoring::colorGraphNormal(grafo, numRegistos);



             } else if (algoritmo == "spilling") {
                 spillados = GraphColoring::colorGraphSpilling(grafo, numRegistos);


             } else if (algoritmo == "splitting") {
                 grafo_final = GraphColoring::colorGraphSplitting(
                     grafo,
                     numRegistos,
                     parametro,
                     splits,
                     webs
                 );

                 sucesso = (grafo_final != nullptr);

             } else {
                 std::cerr << "Algoritmo desconhecido: " << algoritmo << std::endl;
                 return 1;
             }


             Output::generateOutput(outputFile,
                webs,
                grafo_final,
                numRegistos,
                spillados,
                splits);

         } catch (const std::exception& e) {
             std::cerr << "Error: " << e.what() << std::endl;
             return 1;
         }

         return 0;
     }

     if (argc == 1) { //Menu para o utlizador
         bool rangesF = false;
         bool registersF = false;
         std::map<std::string, std::vector<LiveRange>> ranges;
         int numRegistos = 0;
         std::string algoritmo;
         int parametro = 0;
         runMenu(ranges,numRegistos,algoritmo,parametro, rangesF, registersF);
         return 0;
     }
     return 0;
 }
