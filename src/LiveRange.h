
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
    int line;
    bool is_def;   // true se é definição (+), false se é uso (-)

    Ponto(int l, bool def) : line(l), is_def(def) {}
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
    std::string variable;
    std::vector<int> lines;  // linhas onde a variável está viva
    int start;               // primeira linha (com +)
    int end;                  // última linha (com -)
    bool has_def;             // tem marcador +?
    bool has_use;             // tem marcador -?

    LiveRange(const std::string& var): variable(var), start(-1), end(-1),has_def(false), has_use(false) {}


    void addLine(int line, char marker);


    bool contains(int line) const;


    bool overlaps(const LiveRange& other) const;
    void print() const;
};

#endif
