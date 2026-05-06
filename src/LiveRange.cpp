
#include "LiveRange.h"
#include <iostream>
#include <algorithm>

void LiveRange::addLinha(int linha, char marcador) {
    linhas.push_back(linha);

    if (marcador == '+') {
        inicio = linha;
        tem_def = true;
    } else if (marcador == '-') {
        fim = linha;
        tem_uso = true;
    }
}

bool LiveRange::contem(int linha) const {
    return std::find(linhas.begin(), linhas.end(), linha) != linhas.end();
}

bool LiveRange::sobrepoe(const LiveRange& other) const {
    // Verifica se há interseção entre os conjuntos de linhas para formar as web depois
    for (int linha : linhas) {
        if (other.contem(linha)) {
            return true;
        }
    }
    return false;
}

void LiveRange::print() const {
    std::cout << variavel << ": ";
    for (std::size_t i = 0; i < linhas.size(); i++) {
        std::cout << linhas[i];
        if (i == 0 && tem_def) std::cout << "+";
        if (i == linhas.size()-1 && tem_uso) std::cout << "-";
        if (i < linhas.size()-1) std::cout << ",";
    }
    std::cout << std::endl;
}