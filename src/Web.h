
#ifndef WEB_H
#define WEB_H

#include "LiveRange.h"
#include <vector>
#include <set>
#include <string>

/**
 * Web = união de LiveRanges que se sobrepõem
 */
class Web {
public:
    int id;
    std::string variavel;                // variável original
    std::set<int> linhas;                // todas as linhas onde está vivo
    std::vector<LiveRange*> liveRanges;  // live ranges que formam este web

    Web(int webId) : id(webId) {}


    void addLiveRange(LiveRange* lr);


    bool interfereWith(const Web& other) const;


    bool contem(int linha) const;


    void print() const;

    std::string formatLinhas() const;
};

#endif
