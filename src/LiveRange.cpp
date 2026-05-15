
#include "LiveRange.h"
#include <iostream>
#include <algorithm>

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

bool LiveRange::contains(int line) const {
    return std::find(lines.begin(), lines.end(), line) != lines.end();
}

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