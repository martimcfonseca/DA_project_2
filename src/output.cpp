
#include "output.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>

void Output::gerarOutput(
    const std::string& filename,
    const std::vector<Web*>& webs_todos,
    Graph<Web>* grafo_final,
    int num_registos,
    const std::vector<Vertex<Web>*>& spillados,
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
                 << " (" << split.original->variavel << ")\n";

            file << "parts: ";
            for (size_t i = 0; i < split.partes.size(); i++) {
                file << "web" << split.partes[i]->id;
                if (i < split.partes.size() - 1)
                    file << ", ";
            }

            file << "\n";
            file << "split_point: " << split.ponto_divisao << "\n";
            file << "\n";
        }
    }

    // =====================================================
    // WEBS
    // =====================================================

    file << "# WEBS\n";

    std::vector<Vertex<Web>*> vertices = grafo_final->getVertexSet();

    file << "webs: " << vertices.size() << "\n\n";

    for (auto v : vertices) {

        Web web = v->getInfo();

        Web* real = nullptr;

        for (Web* w : webs_todos) {
            if (w->id == web.id) {
                real = w;
                break;
            }
        }

        if (!real) continue;

        file << "web" << real->id << ": "
             << real->formatLinhas() << "\n";
    }

    file << "\n";

    // =====================================================
    // REGISTOS
    // =====================================================

    file << "# REGISTER ASSIGNMENT\n";

    std::set<int> cores_usadas;
    std::map<int, std::vector<int>> cor_para_webs;

    for (auto v : vertices) {

        int cor = v->getColor();
        int web_id = v->getInfo().id;

        if (cor >= 0) {
            cores_usadas.insert(cor);
            cor_para_webs[cor].push_back(web_id);
        }
    }

    file << "registers: " << cores_usadas.size() << "\n\n";

    if (cores_usadas.size()==0) {

        for (auto v : vertices) {

            Web web = v->getInfo();

            Web* real = nullptr;

            for (Web* w : webs_todos) {
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
    for (const auto& pair : cor_para_webs) {

        int cor = pair.first;

        for (int web_id : pair.second) {
            file << "r" << cor << ": web" << web_id << "\n";
        }
    }

    // =====================================================
    // SPILLING
    // =====================================================

    if (!spillados.empty()) {

        file << "\n";
        file << "# SPILLED WEBS\n";
        file << "spills: " << spillados.size() << "\n\n";

        for (auto v : spillados) {
            file << "M: web" << v->getInfo().id << "\n";
        }
    }

    file.close();

    std::cout << "✓ Output gerado: " << filename << std::endl;
}

void Output::printResumo(
    const std::vector<Web*>& webs_todos,
    Graph<Web>* grafo_final,
    int num_registos,
    const std::vector<Vertex<Web>*>& spillados,
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

            for (size_t i = 0; i < split.partes.size(); i++) {
                std::cout << "web" << split.partes[i]->id;

                if (i < split.partes.size() - 1)
                    std::cout << ",";
            }

            std::cout << " ║\n";
        }
    }

    // =====================================================
    // Contar registos usados
    // =====================================================

    std::set<int> cores;

    for (auto v : grafo_final->getVertexSet()) {
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

    if (!spillados.empty()) {

        std::cout << "║ Webs em memória: "
                  << spillados.size();

        if (spillados.size() < 10)
            std::cout << "                 ║\n";
        else
            std::cout << "                ║\n";

        for (auto v : spillados) {
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
