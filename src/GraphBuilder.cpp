
#include "GraphBuilder.h"
#include <iostream>
#include <iomanip>
#include "Graph.h"
/**
 * @brief Checks whether two live ranges overlap.
 *
 * Two live ranges overlap if they share at least one common line.
 *
 * Time Complexity:
 * - O(n * m)
 *
 * Where:
 * - n is the number of lines in lr1
 * - m is the cost of searching in lr2 (linear in its size)
 *
 * Space Complexity:
 * - O(1)
 *
 * @param lr1 First live range.
 * @param lr2 Second live range.
 * @return true if they overlap, false otherwise.
 */
bool GraphBuilder::overlaps(const LiveRange& lr1, const LiveRange& lr2) {
    for (int line : lr1.lines) {
        if (lr2.contains(line)) {
            return true;
        }
    }
    return false;
}
/**
 * @brief Creates webs from a set of live ranges grouped by variable.
 *
 * Live ranges belonging to the same variable are merged into webs
 * when they overlap. A greedy merging strategy is used.
 *
 * Time Complexity:
 * - O(v * n² * m)
 *
 * Where:
 * - v is the number of variables
 * - n is the number of live ranges per variable
 * - m is the average size of a live range
 *
 * Worst case:
 * - Each live range is compared and possibly merged with all others.
 *
 * Space Complexity:
 * - O(w + n)
 *
 * Where:
 * - w is the number of created webs
 * - n is the number of input live ranges
 *
 * @param ranges Map of variables to their live ranges.
 * @return Vector of created Web pointers.
 */
std::vector<Web*> GraphBuilder::createWebs(std::map<std::string, std::vector<LiveRange>>& ranges) {

    std::vector<Web*> webs;
    int webId = 0;

    std::cout << "\n=== CRIANDO WEBS ===\n";

    for (auto& pair : ranges) { //percorrer todas as variaveis 
        std::string variable = pair.first;
        std::vector<LiveRange>& lrs = pair.second;

        std::cout << "Variável: " << variable
                  << " (" << lrs.size() << " live range(s))\n";

        // Marcar quais live ranges já foram processados
        std::vector<bool> processed(lrs.size(), false);

        for (size_t i = 0; i < lrs.size(); i++) {
            if (processed[i]) continue;

            // Criar novo web
            Web* web = new Web(webId++);
            web->addLiveRange(&lrs[i]);
            processed[i] = true;

            std::cout << "  Criando web" << web->id
                     << " com LR inicial: [";
            for (size_t j = 0; j < lrs[i].lines.size(); j++) {
                std::cout << lrs[i].lines[j];
                if (j < lrs[i].lines.size()-1) std::cout << ",";
            }
            std::cout << "]\n";

            // Tentar juntar outros live ranges que se sobrepõem
            bool added = true;
            while (added) {
                added = false;

                for (size_t j = 0; j < lrs.size(); j++) {
                    if (processed[j]) continue;

                    // Verificar se sobrepõe com algum LR já no web
                    bool overlapsWithWeb = false;
                    for (LiveRange* lr : web->liveRanges) {
                        if (overlaps(*lr, lrs[j])) {
                            overlapsWithWeb = true;
                            std::cout << "    → Fundindo com LR [";
                            for (size_t k = 0; k < lrs[j].lines.size(); k++) {
                                std::cout << lrs[j].lines[k];
                                if (k < lrs[j].lines.size()-1) std::cout << ",";
                            }
                            std::cout << "] (sobreposição detectada)\n";
                            break;
                        }
                    }

                    if (overlapsWithWeb) {
                        web->addLiveRange(&lrs[j]);
                        processed[j] = true;
                        added = true;
                    }
                }
            }

            webs.push_back(web);
            web->print();
        }
    }

    std::cout << "\nTotal de webs criados: " << webs.size() << std::endl;
    std::cout << "====================\n";

    return webs;
}
/**
 * @brief Builds an interference graph from a set of webs.
 *
 * Each web becomes a vertex. An edge is added between two webs
 * if they interfere.
 *
 * Time Complexity:
 * - O(w² * i)
 *
 * Where:
 * - w is the number of webs
 * - i is the cost of interference checking
 *
 * Space Complexity:
 * - O(w + e)
 *
 * Where:
 * - e is the number of edges
 *
 * @param webs Vector of Web pointers.
 * @return Pointer to constructed graph.
 */
Graph<Web>* GraphBuilder::buildGraph(std::vector<Web*>& webs) {
    Graph<Web>* graph = new Graph<Web>();

    std::cout << "\n=== CONSTRUINDO GRAFO DE INTERFERÊNCIA ===\n";

    // Adicionar todos os webs como vértices
    for (Web* web : webs) {
        graph->addVertex(*web);
        std::cout << "  Adicionado vértice: web" << web->id << "\n";
    }

    // Verificar interferências e adicionar arestas
    int num_edges = 0;
    for (size_t i = 0; i < webs.size(); i++) {
        for (size_t j = i + 1; j < webs.size(); j++) {
            if (webs[i]->interfereWith(*webs[j])) {
                // Adicionar aresta bidirecional (grafo não-dirigido)
                graph->addBidirectionalEdge(*webs[i], *webs[j], 1.0);
                num_edges++;

                std::cout << "  web" << webs[i]->id << " ↔ web"
                         << webs[j]->id << " (interferem)\n";
            }
        }
    }

    std::cout << "\nTotal de arestas: " << num_edges << std::endl;
    std::cout << "==========================================\n";

    return graph;
}

void GraphBuilder::printGraph(Graph<Web>* graph, const std::vector<Web*>& webs) {
    std::cout << "\n=== GRAFO DE INTERFERÊNCIA ===\n";
    std::cout << "Nós (webs): " << graph->getNumVertex() << std::endl;

    for (Web* web : webs) {
        Vertex<Web>* v = graph->findVertex(*web);
        if (v) {
            int grau = v->getAdj().size();
            std::cout << "  web" << web->id << " (" << web->variable << ")"
                     << " - grau: " << grau
                        << "-color: " << v->getColor() << std::endl;
        }
    }

    std::cout << "\nArestas (interferências):\n";
    for (Web* web : webs) {
        Vertex<Web>* v = graph->findVertex(*web);
        if (v) {
            for (Edge<Web>* e : v->getAdj()) {
                Web dest = e->getDest()->getInfo();
                if (web->id < dest.id) {  // evitar duplicados
                    std::cout << "  web" << web->id << " ↔ web"
                             << dest.id << std::endl;
                }
            }
        }
    }
    std::cout << "==============================\n\n";
}

void GraphBuilder::printMatrix(Graph<Web>* graph, const std::vector<Web*>& webs) {
    std::cout << "\n=== MATRIZ DE ADJACÊNCIA ===\n";

    // Header
    std::cout << "      ";
    for (Web* web : webs) {
        std::cout << std::setw(5) << "web" + std::to_string(web->id);
    }
    std::cout << std::endl;

    // Linhas
    for (Web* web1 : webs) {
        std::cout << "web" << web1->id << "  ";

        for (Web* web2 : webs) {
            if (web1->id == web2->id) {
                std::cout << std::setw(5) << "0";
            } else {
                Vertex<Web>* v1 = graph->findVertex(*web1);
                bool adjacent = false;

                if (v1) {
                    for (Edge<Web>* e : v1->getAdj()) {
                        if (e->getDest()->getInfo().id == web2->id) {
                            adjacent = true;
                            break;
                        }
                    }
                }

                std::cout << std::setw(5) << (adjacent ? "1" : "0");
            }
        }
        std::cout << std::endl;
    }
    std::cout << "============================\n\n";
}