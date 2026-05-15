
#include "Web.h"
#include <iostream>
#include <algorithm>

void Web::addLiveRange(LiveRange* lr) {
    liveRanges.push_back(lr);

    // Adicionar todas as linhas ao conjunto
    for (int linha : lr->lines) {
        lines.insert(linha);
    }

    // Usar a variável do primeiro live range
    if (variable.empty()) {
        variable = lr->variable;
    }
}

bool Web::contains(int line) const {
    return lines.find(line) != lines.end();
}

bool Web::interfereWith(const Web& other) const {


    std::vector<int> intersection;
    std::set_intersection(lines.begin(), lines.end(),other.lines.begin(), other.lines.end(),std::back_inserter(intersection));

    if (intersection.empty()) {
        return false;
    }

    // Verificar se é o caso especial:
    // um web TERMINA exatamente onde outro COMEÇA
    for (int linha : intersection) {
        bool this_ends = false;
        bool another_starts = false;

        // Verificar se este web TERMINA nesta linha
        for (LiveRange* lr : liveRanges) {
            if (lr->end == linha && lr->has_use) {
                this_ends = true;
                break;
            }
        }

        // Verificar se o outro web COMEÇA nesta linha
        for (LiveRange* lr : other.liveRanges) {
            if (lr->start == linha && lr->has_def) {
                another_starts = true;
                break;
            }
        }

        // Caso especial: este morre, outro nasce → NÃO interferem
        if (this_ends && another_starts) {
            // continuar para as proximas linhas
            continue;
        }

        return true;
    }

    return false;
}

void Web::print() const {
    std::cout << "web" << id << " (" << variable << "): ";
    std::cout << formatLines() << std::endl;
}

std::string Web::formatLines() const {
    if (lines.empty()) return "";

    std::string result;
    std::vector<int> lines_vec(lines.begin(), lines.end());

    for (size_t i = 0; i < lines_vec.size(); i++) {
        int line = lines_vec[i];
        result += std::to_string(line);

        // Verificar se tem marcador + ou -
        bool has_plus = false;
        bool has_minus = false;

        for (LiveRange* lr : liveRanges) {
            if (lr->start == line && lr->has_def) {
                has_plus = true;
            }
            if (lr->end == line && lr->has_use) {
                has_minus = true;
            }
        }

        if (!(has_plus && has_minus)) {
            if (has_plus) result += "+";
            if (has_minus) result += "-";
        }


        if (i < lines_vec.size() - 1) {
            result += ",";
        }
    }

    return result;
}
