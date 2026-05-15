
#include "Web.h"
#include <iostream>
#include <algorithm>
/**
 * @brief Adds a live range to the web.
 *
 * The live range pointer is stored internally and all its
 * live lines are inserted into the web line set.
 *
 * If the web variable name has not yet been defined,
 * it is initialized using the variable associated with
 * the first inserted live range.
 *
 * Time Complexity:
 * - O(n log m)
 *
 * Where:
 * - n is the number of lines in the live range.
 * - m is the number of lines already stored in the set.
 *
 * Space Complexity:
 * - O(n)
 *
 * @param lr Pointer to the live range to add.
 */
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
/**
 * @brief Checks whether a given line belongs to the web.
 *
 * Uses set lookup for efficient searching.
 *
 * Time Complexity:
 * - O(log n)
 *
 * Where:
 * - n is the number of stored lines.
 *
 * Space Complexity:
 * - O(1)
 *
 * @param linha Line number to search for.
 * @return true if the line exists in the web.
 */
bool Web::contains(int line) const {
    return lines.find(line) != lines.end();
}
/**
 * @brief Determines whether this web interferes with another web.
 *
 * Two webs interfere if they share at least one active line,
 * except for the special case where one web ends exactly
 * when the other begins.
 *
 * The method computes the intersection between both line sets
 * and analyzes overlapping live ranges.
 *
 * Time Complexity:
 * - O(n + m + k * (a + b))
 *
 * Where:
 * - n is the number of lines in this web.
 * - m is the number of lines in the other web.
 * - k is the size of the intersection.
 * - a and b are the number of live ranges in each web.
 *
 * In practice, the dominant operation is the set intersection.
 *
 * Space Complexity:
 * - O(k)
 *
 * Where:
 * - k is the size of the intersection.
 *
 * @param other Web to compare against.
 * @return true if the webs interfere, false otherwise.
 */
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
