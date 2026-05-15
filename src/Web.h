
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
    std::string variavel;                // variável original
    std::set<int> linhas;                // todas as linhas onde está vivo
    std::vector<LiveRange*> liveRanges;  // live ranges que formam este web


    /**
     * @brief Constructs a new Web object.
     *
     * @param webId Unique identifier of the web.
     */
    Web(int webId) : id(webId) {}


    /**
     * @brief Adds a live range to the web.
     *
     * @param lr Pointer to the live range to add.
     */
    void addLiveRange(LiveRange* lr);

    /**
    * @brief Checks whether this web interferes with another web.
    * @param other Web to compare against.
    * @return true if the webs interfere, false otherwise.
    */

    bool interfereWith(const Web& other) const;

    /**
    * @brief Checks whether the web contains a specific line.
    * @param linha Line number to search for.
    * @return true if the line exists in the web.
    */

    bool contem(int linha) const;


    void print() const;

    std::string formatLinhas() const;

    bool operator==(const Web& other) const {
        return id == other.id;
    }
};

#endif
