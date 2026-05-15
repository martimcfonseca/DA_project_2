
#ifndef LIVERANGE_H
#define LIVERANGE_H

#include <vector>
#include <string>
#include <set>

/**
 * @struct Ponto
 * @brief Represents a point inside a live range.
 *
 * A point may correspond to either:
 * - a definition ('+')
 * - a use ('-')
 */
struct Ponto {
    int linha;
    bool is_def;   // true se é definição (+), false se é uso (-)

    Ponto(int l, bool def) : linha(l), is_def(def) {}
};

/**
 * @class LiveRange
 * @brief Represents the live range of a variable.
 *
 * A live range stores all lines where a variable is alive,
 * together with information about its definition and usage.
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


    /**
    * @brief Adds a line to the live range.
    *
    * @param linha Line number to add.
    * @param marcador Marker character:
    * - '+' for definition
    * - '-' for use
    */
    void addLinha(int linha, char marcador);

    /**
     * @brief Checks whether the live range contains a given line.
     *
     * @param linha Line number to search for.
     * @return true if the line exists in the live range.
     */
    bool contem(int linha) const;

    /**
     * @brief Checks whether this live range overlaps another.
     * @param other Live range to compare against.
     * @return true if the ranges overlap.
     */
    bool sobrepoe(const LiveRange& other) const;
    
    void print() const;
};

#endif
