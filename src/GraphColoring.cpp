//
// Created by marti on 11/05/2026.
//
/**
 * @file GraphColoring.cpp
 * @brief Graph coloring algorithms for register allocation.
 */
#include "GraphColoring.h"
#include <stack>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
/**
 * @brief Performs graph coloring using a spilling-based strategy.
 *
 * This algorithm removes low-degree nodes first and assigns colors
 * in reverse order. When no node with degree < K exists, a spill
 * (node removal to memory) is performed.
 *
 * Time Complexity:
 * - O(v² + e)
 *
 * Where:
 * - v = number of vertices
 * - e = number of edges
 *
 * Space Complexity:
 * - O(v)
 *
 * @param graph Interference graph.
 * @param num Number of available registers.
 * @return List of spilled vertices.
 */
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
/**
 * @brief Standard greedy graph coloring algorithm.
 *
 * If a valid coloring within K colors is not possible,
 * the function returns false.
 *
 * Time Complexity:
 * - O(v² + e)
 *
 * Space Complexity:
 * - O(v)
 */
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
/**
 * @brief Selects vertices with the highest degree for splitting.
 *
 * Time Complexity:
 * - O(v log v)
 */
std::vector<Vertex<Web>*> GraphColoring::chooseWebsForSplit(
    Graph<Web>* graph,
    int k) {

    std::vector<Vertex<Web>*> all = graph->getVertexSet();

    // Ordenar por grau (maior primeiro)
    std::sort(all.begin(), all.end(),
        [](Vertex<Web>* a, Vertex<Web>* b) {
            return a->getAdj().size() > b->getAdj().size();
        }
    );

    // Pegar os k primeiros
    std::vector<Vertex<Web>*> chosen;
    for (int i = 0; i < k && i < (int)all.size(); i++) {
        chosen.push_back(all[i]);
    }

    return chosen;
}

/**
 * @brief Splits a web into two smaller webs.
 *
 * Time Complexity:
 * - O(n log n)
 *
 * Space Complexity:
 * - O(n)
 */
std::vector<Web*> GraphColoring::divideWeb(Web* original, int& next_id) {
    std::vector<Web*> parts;

    // Converter set para vector ordenado para poder aceder por index
    std::vector<int> linhas_vec(original->lines.begin(),
                                 original->lines.end());
    std::sort(linhas_vec.begin(), linhas_vec.end());

    if (linhas_vec.empty()) {
        std::cerr << "ERRO: Web vazio não pode ser dividido!" << std::endl;
        return parts;
    }

    // Dividir ao meio
    int meio = linhas_vec.size() / 2;

    if (meio == 0) {
        std::cerr << "AVISO: Web muito pequeno para dividir!" << std::endl;
        return parts;
    }

    // Criar parte 1 (primeira parte)
    Web* parte1 = new Web(next_id++);
    parte1->variable = original->variable + "_split1";

    for (int i = 0; i < meio-1; i++) {
        parte1->lines.insert(linhas_vec[i]);
    }

    // Criar parte 2 (meio para ser spilled)
    Web* parte2 = new Web(next_id++);
    parte2->variable = original->variable + "_split2";

    for (int i = meio-1; i < meio+1; i++) {
        parte2->lines.insert(linhas_vec[i]);
    }

    // Criar parte 3 (segunda metade)
    Web* parte3 = new Web(next_id++);
    parte3->variable = original->variable + "_split3";

    for (int i = meio+1; i < (int)linhas_vec.size(); i++) {
        parte3->lines.insert(linhas_vec[i]);
    }

    parts.push_back(parte1);
    parts.push_back(parte2);
    parts.push_back(parte3);

    std::cout << "  Dividido web" << original->id
              << " (" << original->variable << ") em:\n";
    std::cout << "    → " << parte1->variable << " [";
    for (int l : parte1->lines) std::cout << l << ",";
    std::cout << "]\n";
    std::cout << "    → " << parte2->variable << " [";
    for (int l : parte2->lines) std::cout << l << ",";
    std::cout << "]\n";
    std::cout << "    → " << parte3->variable << " [";
    for (int l : parte3->lines) std::cout << l << ",";
    std::cout << "]\n";
    std::cout << "    Ponto de divisão: linha "
              << linhas_vec[meio-1] << "/" << linhas_vec[meio] << "\n";

    return parts;
}

/**
 * @brief Rebuilds interference graph after splitting.
 *
 * Time Complexity:
 * - O(v² + e)
 */
Graph<Web>* GraphColoring::rebuildGraph(
    Graph<Web>* original_graph,
    const std::vector<SplitInfo>& splits,
    const std::vector<Web*>& all_webs) {

    // Criar novo grafo
    Graph<Web>* new_graph = new Graph<Web>();

    // Conjunto de IDs dos webs originais que foram divididos
    std::set<int> ids_divididos;
    for (const auto& split : splits) {
        ids_divididos.insert(split.original->id);
    }

    // Adicionar todos os vértices (webs originais não divididos + partes)
    for (Web* web : all_webs) {
        bool was_divided = false;
        for (const auto& split : splits) {
            if (split.original->id == web->id) {
                was_divided = true;
                break;
            }
        }

        if (!was_divided) {
            new_graph->addVertex(*web);
        }
    }

    // Adicionar as partes dos webs divididos
    for (const auto& split : splits) {
        new_graph->addVertex(*split.parts[0]);
        if (split.parts.size() == 3)
            new_graph->addVertex(*split.parts[2]);  // skip parts[1]
        else
            new_graph->addVertex(*split.parts[1]);
    }

    // Reconstruir arestas verificando interferências
    std::vector<Vertex<Web>*> vertices = new_graph->getVertexSet();

    for (size_t i = 0; i < vertices.size(); i++) {
        for (size_t j = i + 1; j < vertices.size(); j++) {
            Web web1 = vertices[i]->getInfo();
            Web web2 = vertices[j]->getInfo();

            // Criar objetos Web temporários para verificar interferência
            Web* w1 = nullptr;
            Web* w2 = nullptr;

            // Encontrar os webs reais na lista
            for (Web* w : all_webs) {
                if (w->id == web1.id) w1 = w;
                if (w->id == web2.id) w2 = w;
            }

            // Verificar também nas partes
            for (const auto& split : splits) {
                for (Web* part : split.parts) {
                    if (part->id == web1.id) w1 = part;
                    if (part->id == web2.id) w2 = part;
                }
            }

            if (w1 && w2 && w1->interfereWith(*w2)) {
                new_graph->addBidirectionalEdge(*w1, *w2, 1.0);
            }
        }
    }

    return new_graph;
}
/**
 * @brief Graph coloring with splitting strategy.
 *
 * Tries progressively splitting high-degree webs until
 * coloring becomes possible.
 *
 * Time Complexity:
 * - Exponential in worst case due to splitting attempts
 */
Graph<Web>* GraphColoring::colorGraphSplitting(
    Graph<Web>*& graph,
    int num,
    int max_splits,
    std::vector<Vertex<Web>*>& spilled,
    std::vector<SplitInfo>& splits_info,
    std::vector<Web*>& all_webs) {

    std::cout << "\n=== WEB SPLITTING ===\n";
    std::cout << "Máximo de splits permitidos: " << max_splits << "\n";
    std::cout << "Registos disponíveis: " << num << "\n\n";

    // GUARDAR webs originais
    std::vector<Web*> original_webs = all_webs;

    // Tentar 1..max_splits
    for (int num_splits = 1; num_splits <= max_splits; num_splits++) {

        std::cout << "Tentativa " << num_splits
                  << ": dividir "
                  << num_splits
                  << " web(s)...\n";

        splits_info.clear();

        // IMPORTANTE:
        // recriar lista limpa em cada tentativa
        std::vector<Web*> current_webs = original_webs;

        // Escolher webs para dividir
        std::vector<Vertex<Web>*> vertices_para_dividir =
            chooseWebsForSplit(graph, num_splits);

        if (vertices_para_dividir.empty()) {
            std::cout << "  Nenhum web para dividir!\n";
            continue;
        }

        std::cout << "  Webs escolhidos (maior grau):\n";

        for (auto v : vertices_para_dividir) {
            Web web = v->getInfo();

            std::cout << "    web" << web.id
                      << " (" << web.variable
                      << ") - grau: "
                      << v->getAdj().size()
                      << "\n";
        }

        std::cout << "\n";

        // Próximo ID disponível
        int proximo_id = 1000;

        // Dividir webs
        for (auto v : vertices_para_dividir) {

            Web web_original = v->getInfo();

            Web* web_ptr = nullptr;

            // procurar apenas nos originais
            for (Web* w : original_webs) {
                if (w->id == web_original.id) {
                    web_ptr = w;
                    break;
                }
            }

            if (!web_ptr) {
                std::cerr << "ERRO: Web não encontrado!\n";
                continue;
            }

            // dividir
            std::vector<Web*> partes =
                divideWeb(web_ptr, proximo_id);

            if (partes.size() != 3) continue;

            // guardar split info
            SplitInfo info;
            info.original = web_ptr;
            info.parts = partes;

            std::vector<int> linhas(
                web_ptr->lines.begin(),
                web_ptr->lines.end()
            );

            std::sort(linhas.begin(), linhas.end());

            info.point_division =
                linhas[linhas.size()/2 - 1];

            splits_info.push_back(info);

            // adicionar partes ao vetor temporário
            current_webs.push_back(partes[0]);
            current_webs.push_back(partes[1]);
            current_webs.push_back(partes[2]);

            Web temp = *partes[1];
            Vertex<Web>* vertex = new Vertex<Web>(temp);
            spilled.push_back(vertex);
        }

        std::cout << "\n";

        // reconstruir grafo
        Graph<Web>* novo_grafo =
            rebuildGraph(
                graph,
                splits_info,
                current_webs
            );

        std::cout << "  Novo grafo construído:\n";
        std::cout << "    Vértices: "
                  << novo_grafo->getNumVertex()
                  << "\n";

        int num_arestas = 0;

        for (auto v : novo_grafo->getVertexSet()) {
            num_arestas += v->getAdj().size();
        }

        std::cout << "    Arestas: "
                  << num_arestas / 2
                  << "\n\n";

        // tentar colorir
        std::cout << "  Tentando colorir com "
                  << num
                  << " registos...\n";

        bool sucesso =
            colorGraphNormal(novo_grafo, num);

        for (auto w : spilled) {
            novo_grafo->findVertex(w->getInfo())->setColor(-1);
        }

        if (sucesso) {

            std::cout << "  ✓ SUCESSO! Coloração encontrada!\n";
            std::cout << "=====================\n\n";

            // atualizar all_webs final
            all_webs = current_webs;

            return novo_grafo;
        }

        std::cout << "  ✗ Falhou. Tentando com mais splits...\n\n";

        delete novo_grafo;
    }

    std::cout << "✗ Falhou mesmo com "
              << max_splits
              << " splits.\n";

    std::cout << "=====================\n\n";

    return graph;
}

/**
 * @brief Performs register allocation using a free-register assignment strategy.
 *
 * This algorithm scans program lines sequentially and dynamically assigns
 * available registers to active webs.
 *
 * A register can be reused if:
 * - it is currently free, or
 * - the assigned web does not interfere with the current web.
 *
 * If no register is available, the web is spilled to memory.
 *
 * Time Complexity:
 * - O(l * v * r)
 *
 * Where:
 * - l = number of program lines
 * - v = number of webs (vertices)
 * - r = number of available registers
 *
 * Worst case:
 * - every line checks every web and every register.
 *
 * Space Complexity:
 * - O(v + r)
 *
 * @param graph Interference graph.
 * @param num Number of available registers.
 * @return Vector containing spilled webs.
 */
std::vector<Vertex<Web>*> GraphColoring::freeRegisterAssign(Graph<Web> *graph, int num) {

    auto vertexWebs = graph->getVertexSet();


    std::vector<bool> allocated(vertexWebs.size(), false);
    std::map<int,Vertex<Web>*> webMap;
    std::vector<Vertex<Web>*> spilled_webs;

    int lastLine = 0;
    for (auto &v : vertexWebs) {
        Web w = v->getInfo();
        lastLine = std::max(lastLine,*w.lines.rbegin());
    }

    for ( int i=1; i<lastLine; i++) {
        for (auto& v : vertexWebs) {
            Web web = v->getInfo();
            if (!web.contains(i)) continue;
            if (allocated[web.id] ) continue;

                bool found = false;
                for (int j=0; j<num; j++) {
                    if (webMap.find(j) == webMap.end() || !webMap[j]->getInfo().interfereWith(web)) {
                        if (v->getColor() != -1 && v->getColor() != webMap[j]->getColor()) continue;
                        if (webMap.count(j) && webMap[j]->getInfo().id != -1) {
                            allocated[webMap[j]->getInfo().id] = false;
                        }
                        webMap[j]= v;
                        v->setColor(j);
                        allocated[web.id] = true;
                        found=true;
                        break;
                    }
                }
            if (!found) {
                spilled_webs.push_back(v);
                v->setColor(-1);
            }
        }
    }
    return spilled_webs;
}