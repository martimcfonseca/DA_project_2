
#include "Parser.h"
#include "GraphBuilder.h"
#include "GraphColoring.h"
#include "output.h"
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
