
#include "output.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>

void Output::generateOutput(
    const std::string& filename,
    const std::vector<Web*>& webs_all,
    Graph<Web>* graph_final,
    int num_register,
    const std::vector<Vertex<Web>*>& spilled,
    const std::vector<SplitInfo>& splits) {

    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Erro ao criar ficheiro: " << filename << std::endl;
        return;
    }

    // =====================================================
    // SPLITTING INFO
    // =====================================================

    if (!splits.empty()) {
        file << "# WEB SPLITTING\n";
        file << "splits: " << splits.size() << "\n\n";

        for (const auto& split : splits) {
            file << "original: web" << split.original->id
                 << " (" << split.original->variable << ")\n";

            file << "parts: ";
            for (size_t i = 0; i < split.parts.size(); i++) {
                file << "web" << split.parts[i]->id;
                if (i < split.parts.size() - 1)
                    file << ", ";
            }

            file << "\n";
            file << "split_point: " << split.point_division << "\n";
            file << "\n";
        }
    }

    // =====================================================
    // WEBS
    // =====================================================

    file << "# WEBS\n";

    std::vector<Vertex<Web>*> vertexes = graph_final->getVertexSet();

    file << "webs: " << vertexes.size() << "\n\n";

    for (auto v : vertexes) {

        Web web = v->getInfo();

        Web* real = nullptr;

        for (Web* w : webs_all) {
            if (w->id == web.id) {
                real = w;
                break;
            }
        }

        if (!real) continue;

        file << "web" << real->id << ": "
             << real->formatLines() << "\n";
    }

    file << "\n";

    // =====================================================
    // REGISTOS
    // =====================================================

    file << "# REGISTER ASSIGNMENT\n";

    std::set<int> used_colors;
    std::map<int, std::vector<int>> web_colors;

    for (auto v : vertexes) {

        int color = v->getColor();
        int web_id = v->getInfo().id;

        if (color >= 0) {
            used_colors.insert(color);
            web_colors[color].push_back(web_id);
        }
    }

    file << "registers: " << used_colors.size() << "\n\n";

    if (used_colors.size()==0) {

        for (auto v : vertexes) {

            Web web = v->getInfo();

            Web* real = nullptr;

            for (Web* w : webs_all) {
                if (w->id == web.id) {
                    real = w;
                    break;
                }
            }

            if (!real) continue;

            file  << "M: web" << real->id << std::endl;
        }

    }

    // Registos
    for (const auto& pair : web_colors) {

        int cor = pair.first;

        for (int web_id : pair.second) {
            file << "r" << cor << ": web" << web_id << "\n";
        }
    }

    // =====================================================
    // SPILLING
    // =====================================================

    if (!spilled.empty()) {

        file << "\n";
        file << "# SPILLED WEBS\n";
        file << "spills: " << spilled.size() << "\n\n";

        for (auto v : spilled) {
            file << "M: web" << v->getInfo().id << "\n";
        }
    }

    file.close();

    std::cout << "✓ Output gerado: " << filename << std::endl;
}

void Output::printResume(
    const std::vector<Web*>& webs_all,
    Graph<Web>* graph_final,
    int num_registers,
    const std::vector<Vertex<Web>*>& spilled,
    const std::vector<SplitInfo>& splits) {

    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║          RESULTADO FINAL               ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    // =====================================================
    // Splitting
    // =====================================================

    if (!splits.empty()) {

        std::cout << "║ Splits realizados: "
                  << splits.size();

        if (splits.size() < 10)
            std::cout << "                  ║\n";
        else
            std::cout << "                 ║\n";

        for (const auto& split : splits) {
            std::cout << "║ web" << split.original->id
                      << " dividido em ";

            for (size_t i = 0; i < split.parts.size(); i++) {
                std::cout << "web" << split.parts[i]->id;

                if (i < split.parts.size() - 1)
                    std::cout << ",";
            }

            std::cout << " ║\n";
        }
    }

    // =====================================================
    // Contar registos usados
    // =====================================================

    std::set<int> cores;

    for (auto v : graph_final->getVertexSet()) {
        if (v->getColor() >= 0) {
            cores.insert(v->getColor());
        }
    }

    std::cout << "║ Registos usados: "
              << cores.size();

    if (cores.size() < 10)
        std::cout << "                   ║\n";
    else
        std::cout << "                  ║\n";

    // =====================================================
    // Spilling
    // =====================================================

    if (!spilled.empty()) {

        std::cout << "║ Webs em memória: "
                  << spilled.size();

        if (spilled.size() < 10)
            std::cout << "                 ║\n";
        else
            std::cout << "                ║\n";

        for (auto v : spilled) {
            std::cout << "║   web"
                      << v->getInfo().id
                      << " → MEMÓRIA";

            std::string texto = "web" + std::to_string(v->getInfo().id) + " → MEMÓRIA";

            int padding = 33 - texto.size();

            for (int i = 0; i < padding; i++)
                std::cout << " ";

            std::cout << "║\n";
        }
    }

    // =====================================================
    // Resultado final
    // =====================================================

    std::cout << "║ ✓ ALOCAÇÃO CONCLUÍDA                   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
}
