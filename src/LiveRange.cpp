
#include "LiveRange.h"
#include <iostream>
#include <algorithm>
/**
 * @brief Adds a line to the live range.
 *
 * The line is inserted into the internal vector and
 * definition/use markers are updated depending on
 * the provided marker character.
 *
 * Supported markers:
 * - '+' -> definition
 * - '-' -> use
 *
 * Time Complexity:
 * - O(1)
 *
 * Space Complexity:
 * - O(1)
 *
 * @param linha Line number to add.
 * @param marcador Marker character.
 */
void LiveRange::addLine(int line, char marker) {
    lines.push_back(line);

    if (marker == '+') {
        start = line;
        has_def = true;
    } else if (marker == '-') {
        end = line;
        has_use = true;
    }
}
/**
 * @brief Checks whether the live range contains a given line.
 *
 * Performs a linear search over the stored lines.
 *
 * Time Complexity:
 * - O(n)
 *
 * Where:
 * - n is the number of stored lines.
 *
 * Space Complexity:
 * - O(1)
 *
 * @param linha Line number to search for.
 * @return true if the line exists in the live range.
 */
bool LiveRange::contains(int line) const {
    return std::find(lines.begin(), lines.end(), line) != lines.end();
}
/**
 * @brief Checks whether this live range overlaps another.
 *
 * Two live ranges overlap if they share at least one line.
 * This method is mainly used later when constructing webs.
 *
 * Time Complexity:
 * - O(n * m)
 *
 * Where:
 * - n is the number of lines in this live range.
 * - m is the number of lines in the other live range.
 *
 * In practice:
 * - Each line in this live range performs a linear lookup
 *   in the other live range.
 *
 * Space Complexity:
 * - O(1)
 *
 * @param other Live range to compare against.
 * @return true if the live ranges overlap.
 */
bool LiveRange::overlaps(const LiveRange& other) const {
    // Verifica se há interseção entre os conjuntos de linhas para formar as web depois
    for (int line : lines) {
        if (other.contains(line)) {
            return true;
        }
    }
    return false;
}

void LiveRange::print() const {
    std::cout << variable << ": ";
    for (std::size_t i = 0; i < lines.size(); i++) {
        std::cout << lines[i];
        if (i == 0 && has_def) std::cout << "+";
        if (i == lines.size()-1 && has_use) std::cout << "-";
        if (i < lines.size()-1) std::cout << ",";
    }
    std::cout << std::endl;
}