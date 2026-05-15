
#include "GraphBuilder.h"
#include <iostream>
#include <iomanip>
#include "Graph.h"

bool GraphBuilder::overlaps(const LiveRange& lr1, const LiveRange& lr2) {
    for (int line : lr1.lines) {
        if (lr2.contains(line)) {
            return true;
        }
    }
    return false;
}

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