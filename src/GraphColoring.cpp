//
// Created by marti on 11/05/2026.
//

#include "GraphColoring.h"
#include <stack>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <set>

std::vector<Vertex<Web> *> GraphColoring::colorGraphSpilling(Graph<Web> *&graph, int num){
        for (auto v : graph->getVertexSet()) {
            v->setColor(-1);
            v->setVisited(false);
        }

        std::unordered_map<Vertex<Web>*, int> liveDeg;
        for (auto v : graph->getVertexSet())
            liveDeg[v] = (int)v->getAdj().size();   // undirected: adj holds all edges

        std::stack<Vertex<Web>*>  colorStack;
        std::vector<Vertex<Web>*> spilled;

        // ── Phase 1: Simplification ────────────────────────────────────────────
        bool changed = true;
        while (changed) {
            changed = false;

            // Push every live node whose live degree < N
            for (auto v : graph->getVertexSet()) {
                if (v->isVisited()) continue;           // already removed
                if (liveDeg[v] < num) {
                    colorStack.push(v);
                    v->setVisited(true);                // mark as removed

                    // Decrement live degree of live neighbours
                    for (auto edge : v->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;
                }
            }

            // If no progress: all remaining nodes have degree >= N → spill one
            if (!changed) {
                Vertex<Web>* best = nullptr;
                for (auto v : graph->getVertexSet()) {
                    if (v->isVisited()) continue;
                    if (!best || liveDeg[v] > liveDeg[best])
                        best = v;
                }

                if (best != nullptr) {
                    spilled.push_back(best);
                    best->setVisited(true);             // remove from live graph

                    for (auto edge : best->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;                     // restart simplification
                }
            }
        }

        // ── Phase 2: Coloring ──────────────────────────────────────────────────
        while (!colorStack.empty()) {
            auto v = colorStack.top();
            colorStack.pop();

            // Collect colors already used by neighbours (colored or not)
            std::set<int> usedColors;
            for (auto edge : v->getAdj()) {
                int c = edge->getDest()->getColor();
                if (c >= 0)
                    usedColors.insert(c);
            }

            // Assign the lowest non-negative integer not in usedColors
            int color = 0;
            while (usedColors.count(color))
                color++;

            v->setColor(color);     // guaranteed to be < N (degree was < N when pushed)
        }

        return spilled;             // caller inspects v->getColor() == -1 for spills
}

bool GraphColoring::colorGraphNormal(Graph<Web> *&graph, int num) {

    for (auto v : graph->getVertexSet()) {
            v->setColor(-1);
            v->setVisited(false);   // visited == "removed from live graph"
        }

        std::unordered_map<Vertex<Web>*, int> liveDeg;
        for (auto v : graph->getVertexSet())
            liveDeg[v] = (int)v->getAdj().size();

        std::stack<Vertex<Web>*> colorStack;

        bool changed = true;
        while (changed) {
            changed = false;

            for (auto v : graph->getVertexSet()) {
                if (v->isVisited()) continue;
                if (liveDeg[v] < num) {
                    colorStack.push(v);
                    v->setVisited(true);

                    for (auto edge : v->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;
                }
            }

            // No progress but live nodes remain → graph is not N-colorable.
            // Leave all colors at -1 so the caller loads everything from memory.
            if (!changed) {
                for (auto v : graph->getVertexSet())
                    if (!v->isVisited())
                        return false;
            }
        }

        while (!colorStack.empty()) {
            auto v = colorStack.top();
            colorStack.pop();

            std::set<int> usedColors;
            for (auto edge : v->getAdj()) {
                int c = edge->getDest()->getColor();
                if (c >= 0)
                    usedColors.insert(c);
            }

            // Lowest non-negative integer not used by any neighbour.
            // Guaranteed to be < N because degree was < N when pushed.
            int color = 0;
            while (usedColors.count(color)>0) {
                color++;
            }
            v->setColor(color);
        }

        return true;
    }

std::vector<Vertex<Web>*> GraphColoring::escolherWebsParaSplit(
    Graph<Web>* graph,
    int k) {

    std::vector<Vertex<Web>*> todos = graph->getVertexSet();

    // Ordenar por grau (maior primeiro)
    std::sort(todos.begin(), todos.end(),
        [](Vertex<Web>* a, Vertex<Web>* b) {
            return a->getAdj().size() > b->getAdj().size();
        }
    );

    // Pegar os k primeiros
    std::vector<Vertex<Web>*> escolhidos;
    for (int i = 0; i < k && i < (int)todos.size(); i++) {
        escolhidos.push_back(todos[i]);
    }

    return escolhidos;
}


std::vector<Web*> GraphColoring::dividirWeb(Web* original, int& proximo_id) {
    std::vector<Web*> partes;

    // Converter set para vector ordenado para poder aceder por index
    std::vector<int> linhas_vec(original->linhas.begin(),
                                 original->linhas.end());
    std::sort(linhas_vec.begin(), linhas_vec.end());

    if (linhas_vec.empty()) {
        std::cerr << "ERRO: Web vazio não pode ser dividido!" << std::endl;
        return partes;
    }

    // Dividir ao meio
    int meio = linhas_vec.size() / 2;

    if (meio == 0) {
        std::cerr << "AVISO: Web muito pequeno para dividir!" << std::endl;
        return partes;
    }

    // Criar parte 1 (primeira metade)
    Web* parte1 = new Web(proximo_id++);
    parte1->variavel = original->variavel + "_split1";

    for (int i = 0; i < meio; i++) {
        parte1->linhas.insert(linhas_vec[i]);
    }

    // Criar parte 2 (segunda metade)
    Web* parte2 = new Web(proximo_id++);
    parte2->variavel = original->variavel + "_split2";

    for (int i = meio; i < (int)linhas_vec.size(); i++) {
        parte2->linhas.insert(linhas_vec[i]);
    }

    partes.push_back(parte1);
    partes.push_back(parte2);

    std::cout << "  Dividido web" << original->id
              << " (" << original->variavel << ") em:\n";
    std::cout << "    → " << parte1->variavel << " [";
    for (int l : parte1->linhas) std::cout << l << ",";
    std::cout << "]\n";
    std::cout << "    → " << parte2->variavel << " [";
    for (int l : parte2->linhas) std::cout << l << ",";
    std::cout << "]\n";
    std::cout << "    Ponto de divisão: linha "
              << linhas_vec[meio-1] << "/" << linhas_vec[meio] << "\n";

    return partes;
}

Graph<Web>* GraphColoring::reconstruirGrafo(
    Graph<Web>* grafo_original,
    const std::vector<SplitInfo>& splits,
    const std::vector<Web*>& todos_webs) {

    // Criar novo grafo
    Graph<Web>* novo_grafo = new Graph<Web>();

    // Conjunto de IDs dos webs originais que foram divididos
    std::set<int> ids_divididos;
    for (const auto& split : splits) {
        ids_divididos.insert(split.original->id);
    }

    // Adicionar todos os vértices (webs originais não divididos + partes)
    for (Web* web : todos_webs) {
        // Verificar se este web foi dividido
        bool foi_dividido = false;
        for (const auto& split : splits) {
            if (split.original->id == web->id) {
                foi_dividido = true;
                break;
            }
        }

        if (!foi_dividido) {
            // Web não foi dividido, adicionar normalmente
            novo_grafo->addVertex(*web);
        }
    }

    // Adicionar as partes dos webs divididos
    for (const auto& split : splits) {
        for (Web* parte : split.partes) {
            novo_grafo->addVertex(*parte);
        }
    }

    // Reconstruir arestas verificando interferências
    std::vector<Vertex<Web>*> vertices = novo_grafo->getVertexSet();

    for (size_t i = 0; i < vertices.size(); i++) {
        for (size_t j = i + 1; j < vertices.size(); j++) {
            Web web1 = vertices[i]->getInfo();
            Web web2 = vertices[j]->getInfo();

            // Criar objetos Web temporários para verificar interferência
            Web* w1 = nullptr;
            Web* w2 = nullptr;

            // Encontrar os webs reais na lista
            for (Web* w : todos_webs) {
                if (w->id == web1.id) w1 = w;
                if (w->id == web2.id) w2 = w;
            }

            // Verificar também nas partes
            for (const auto& split : splits) {
                for (Web* parte : split.partes) {
                    if (parte->id == web1.id) w1 = parte;
                    if (parte->id == web2.id) w2 = parte;
                }
            }

            if (w1 && w2 && w1->interfereWith(*w2)) {
                novo_grafo->addBidirectionalEdge(*w1, *w2, 1.0);
            }
        }
    }

    return novo_grafo;
}

Graph<Web>* GraphColoring::colorGraphSplitting(
    Graph<Web>*& graph,
    int num,
    int max_splits,
    std::vector<SplitInfo>& splits_info,
    std::vector<Web*>& webs_todos) {

    std::cout << "\n=== WEB SPLITTING ===\n";
    std::cout << "Máximo de splits permitidos: " << max_splits << "\n";
    std::cout << "Registos disponíveis: " << num << "\n\n";

    // Tentar com 1, 2, ..., max_splits divisões
    for (int num_splits = 1; num_splits <= max_splits; num_splits++) {
        std::cout << "Tentativa " << num_splits << ": dividir "
                  << num_splits << " web(s)...\n";

        splits_info.clear();

        // 1. Escolher webs para dividir (maior grau)
        std::vector<Vertex<Web>*> vertices_para_dividir =
            escolherWebsParaSplit(graph, num_splits);

        if (vertices_para_dividir.empty()) {
            std::cout << "  Nenhum web para dividir!\n";
            continue;
        }

        std::cout << "  Webs escolhidos (maior grau):\n";
        for (auto v : vertices_para_dividir) {
            Web web = v->getInfo();
            std::cout << "    web" << web.id << " (" << web.variavel
                     << ") - grau: " << v->getAdj().size() << "\n";
        }
        std::cout << "\n";

        // 2. Dividir cada web
        int proximo_id = webs_todos.size();  // próximo ID disponível

        for (auto v : vertices_para_dividir) {
            Web web_original = v->getInfo();

            // Encontrar o web real na lista
            Web* web_ptr = nullptr;
            for (Web* w : webs_todos) {
                if (w->id == web_original.id) {
                    web_ptr = w;
                    break;
                }
            }

            if (!web_ptr) {
                std::cerr << "ERRO: Web não encontrado!\n";
                continue;
            }

            // Dividir
            std::vector<Web*> partes = dividirWeb(web_ptr, proximo_id);

            if (partes.size() == 2) {
                SplitInfo info;
                info.original = web_ptr;
                info.partes = partes;

                // Calcular ponto de divisão
                std::vector<int> linhas_orig(web_ptr->linhas.begin(),
                                            web_ptr->linhas.end());
                std::sort(linhas_orig.begin(), linhas_orig.end());
                info.ponto_divisao = linhas_orig[linhas_orig.size()/2 - 1];

                splits_info.push_back(info);

                // Adicionar novas partes à lista de webs
                webs_todos.push_back(partes[0]);
                webs_todos.push_back(partes[1]);
            }
        }

        std::cout << "\n";

        // 3. Reconstruir grafo
        Graph<Web>* novo_grafo = reconstruirGrafo(
            graph,
            splits_info,
            webs_todos
        );

        std::cout << "  Novo grafo construído:\n";
        std::cout << "    Vértices: " << novo_grafo->getNumVertex() << "\n";

        // Contar arestas
        int num_arestas = 0;
        for (auto v : novo_grafo->getVertexSet()) {
            num_arestas += v->getAdj().size();
        }
        std::cout << "    Arestas: " << num_arestas/2 << "\n\n";

        // 4. Tentar colorir
        std::cout << "  Tentando colorir com " << num << " registos...\n";
        bool sucesso = colorGraphNormal(novo_grafo, num);

        if (sucesso) {
            std::cout << "  ✓ SUCESSO! Coloração encontrada!\n";
            std::cout << "=====================\n\n";
            return novo_grafo;
        } else {
            std::cout << "  ✗ Falhou. Tentando com mais splits...\n\n";
            delete novo_grafo;
        }
    }

    std::cout << "✗ Falhou mesmo com " << max_splits << " splits.\n";
    std::cout << "=====================\n\n";

    return nullptr;
}
