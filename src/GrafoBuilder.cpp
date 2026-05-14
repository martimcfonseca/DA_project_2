
#include "GrafoBuilder.h"
#include <iostream>
#include <iomanip>
#include "Graph.h"

bool GrafoBuilder::sobrepoe(const LiveRange& lr1, const LiveRange& lr2) {
    for (int linha : lr1.linhas) {
        if (lr2.contem(linha)) {
            return true;
        }
    }
    return false;
}

std::vector<Web*> GrafoBuilder::criarWebs(std::map<std::string, std::vector<LiveRange>>& ranges) {

    std::vector<Web*> webs;
    int webId = 0;

    std::cout << "\n=== CRIANDO WEBS ===\n";

    for (auto& pair : ranges) { //percorrer todas as variaveis 
        std::string variavel = pair.first;
        std::vector<LiveRange>& lrs = pair.second;

        std::cout << "Variável: " << variavel
                  << " (" << lrs.size() << " live range(s))\n";

        // Marcar quais live ranges já foram processados
        std::vector<bool> processado(lrs.size(), false);

        for (size_t i = 0; i < lrs.size(); i++) {
            if (processado[i]) continue;

            // Criar novo web
            Web* web = new Web(webId++);
            web->addLiveRange(&lrs[i]);
            processado[i] = true;

            std::cout << "  Criando web" << web->id
                     << " com LR inicial: [";
            for (size_t j = 0; j < lrs[i].linhas.size(); j++) {
                std::cout << lrs[i].linhas[j];
                if (j < lrs[i].linhas.size()-1) std::cout << ",";
            }
            std::cout << "]\n";

            // Tentar juntar outros live ranges que se sobrepõem
            bool adicionou = true;
            while (adicionou) {
                adicionou = false;

                for (size_t j = 0; j < lrs.size(); j++) {
                    if (processado[j]) continue;

                    // Verificar se sobrepõe com algum LR já no web
                    bool sobrepoe_com_web = false;
                    for (LiveRange* lr : web->liveRanges) {
                        if (sobrepoe(*lr, lrs[j])) {
                            sobrepoe_com_web = true;
                            std::cout << "    → Fundindo com LR [";
                            for (size_t k = 0; k < lrs[j].linhas.size(); k++) {
                                std::cout << lrs[j].linhas[k];
                                if (k < lrs[j].linhas.size()-1) std::cout << ",";
                            }
                            std::cout << "] (sobreposição detectada)\n";
                            break;
                        }
                    }

                    if (sobrepoe_com_web) {
                        web->addLiveRange(&lrs[j]);
                        processado[j] = true;
                        adicionou = true;
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

Graph<Web>* GrafoBuilder::construirGrafo(std::vector<Web*>& webs) {
    Graph<Web>* grafo = new Graph<Web>();

    std::cout << "\n=== CONSTRUINDO GRAFO DE INTERFERÊNCIA ===\n";

    // Adicionar todos os webs como vértices
    for (Web* web : webs) {
        grafo->addVertex(*web);
        std::cout << "  Adicionado vértice: web" << web->id << "\n";
    }

    // Verificar interferências e adicionar arestas
    int num_arestas = 0;
    for (size_t i = 0; i < webs.size(); i++) {
        for (size_t j = i + 1; j < webs.size(); j++) {
            if (webs[i]->interfereWith(*webs[j])) {
                // Adicionar aresta bidirecional (grafo não-dirigido)
                grafo->addBidirectionalEdge(*webs[i], *webs[j], 1.0);
                num_arestas++;

                std::cout << "  web" << webs[i]->id << " ↔ web"
                         << webs[j]->id << " (interferem)\n";
            }
        }
    }

    std::cout << "\nTotal de arestas: " << num_arestas << std::endl;
    std::cout << "==========================================\n";

    return grafo;
}

void GrafoBuilder::printGrafo(Graph<Web>* grafo, const std::vector<Web*>& webs) {
    std::cout << "\n=== GRAFO DE INTERFERÊNCIA ===\n";
    std::cout << "Nós (webs): " << grafo->getNumVertex() << std::endl;

    for (Web* web : webs) {
        Vertex<Web>* v = grafo->findVertex(*web);
        if (v) {
            int grau = v->getAdj().size();
            std::cout << "  web" << web->id << " (" << web->variavel << ")"
                     << " - grau: " << grau
                        << "-color: " << v->getColor() << std::endl;
        }
    }

    std::cout << "\nArestas (interferências):\n";
    for (Web* web : webs) {
        Vertex<Web>* v = grafo->findVertex(*web);
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

void GrafoBuilder::printMatriz(Graph<Web>* grafo, const std::vector<Web*>& webs) {
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
                Vertex<Web>* v1 = grafo->findVertex(*web1);
                bool adjacente = false;

                if (v1) {
                    for (Edge<Web>* e : v1->getAdj()) {
                        if (e->getDest()->getInfo().id == web2->id) {
                            adjacente = true;
                            break;
                        }
                    }
                }

                std::cout << std::setw(5) << (adjacente ? "1" : "0");
            }
        }
        std::cout << std::endl;
    }
    std::cout << "============================\n\n";
}