
#ifndef WEB_H
#define WEB_H

#include "LiveRange.h"
#include <vector>
#include <set>
#include <string>

/**
 * @class Web
 * @brief Represents a web formed by one or more live ranges.
 *
 * A web groups together compatible live ranges belonging to the same
 * variable. It stores all lines where the variable is alive and provides
 * utility methods for interference analysis.
 */
class Web {
public:
    int id;
    std::string variable;                // variável original
    std::set<int> lines;                // todas as linhas onde está vivo
    std::vector<LiveRange*> liveRanges;  // live ranges que formam este web

    Web() : id(-1) {}

    Web(int webId) : id(webId) {}


    void addLiveRange(LiveRange* lr);


    bool interfereWith(const Web& other) const;


    bool contains(int line) const;


    void print() const;

    std::string formatLines() const;

    bool operator==(const Web& other) const {
        return id == other.id;
    }
};

#endif
