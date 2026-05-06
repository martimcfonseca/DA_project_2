
#ifndef LIVERANGE_H
#define LIVERANGE_H

#include <vector>
#include <string>
#include <set>

/**
 * Representa um ponto num live range
 */
struct Ponto {
    int linha;
    bool is_def;   // true se é definição (+), false se é uso (-)

    Ponto(int l, bool def) : linha(l), is_def(def) {}
};

/**
 * Representa um live range de uma variável
 */
class LiveRange {
public:
    std::string variavel;
    std::vector<int> linhas;  // linhas onde a variável está viva
    int inicio;               // primeira linha (com +)
    int fim;                  // última linha (com -)
    bool tem_def;             // tem marcador +?
    bool tem_uso;             // tem marcador -?

    LiveRange(const std::string& var): variavel(var), inicio(-1), fim(-1),tem_def(false), tem_uso(false) {}

    void addLinha(int linha, char marcador);
    bool contem(int linha) const;
    bool sobrepoe(const LiveRange& other) const;
    void print() const;
};

#endif
