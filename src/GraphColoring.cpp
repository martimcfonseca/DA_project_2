//
// Created by marti on 11/05/2026.
//

#include "GraphColoring.h"
#include <stack>
#include <unordered_map>

std::vector<Vertex<Web> *> GraphColoring::colorGraphSpilling(Graph<Web> *&graph, int num){
        for (auto v : graph->getVertexSet()) {
            v->setColor(-1);
            v->setVisited(false);
        }

        std::unordered_map<Vertex<Web>*, int> liveDeg;
        for (auto v : graph->getVertexSet())
            liveDeg[v] = (int)v->getAdj().size();   // undirected: adj holds all edges

        std::stack<Vertex<Web>*>  colorStack;
        std::vector<Vertex<Web>*> spilled;

        // ── Phase 1: Simplification ────────────────────────────────────────────
        bool changed = true;
        while (changed) {
            changed = false;

            // Push every live node whose live degree < N
            for (auto v : graph->getVertexSet()) {
                if (v->isVisited()) continue;           // already removed
                if (liveDeg[v] < num) {
                    colorStack.push(v);
                    v->setVisited(true);                // mark as removed

                    // Decrement live degree of live neighbours
                    for (auto edge : v->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;
                }
            }

            // If no progress: all remaining nodes have degree >= N → spill one
            if (!changed) {
                Vertex<Web>* best = nullptr;
                for (auto v : graph->getVertexSet()) {
                    if (v->isVisited()) continue;
                    if (!best || liveDeg[v] > liveDeg[best])
                        best = v;
                }

                if (best != nullptr) {
                    spilled.push_back(best);
                    best->setVisited(true);             // remove from live graph

                    for (auto edge : best->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;                     // restart simplification
                }
            }
        }

        // ── Phase 2: Coloring ──────────────────────────────────────────────────
        while (!colorStack.empty()) {
            auto v = colorStack.top();
            colorStack.pop();

            // Collect colors already used by neighbours (colored or not)
            std::set<int> usedColors;
            for (auto edge : v->getAdj()) {
                int c = edge->getDest()->getColor();
                if (c >= 0)
                    usedColors.insert(c);
            }

            // Assign the lowest non-negative integer not in usedColors
            int color = 0;
            while (usedColors.count(color))
                color++;

            v->setColor(color);     // guaranteed to be < N (degree was < N when pushed)
        }

        return spilled;             // caller inspects v->getColor() == -1 for spills
}

bool GraphColoring::colorGraphNormal(Graph<Web> *&graph, int num) {

    for (auto v : graph->getVertexSet()) {
            v->setColor(-1);
            v->setVisited(false);   // visited == "removed from live graph"
        }

        std::unordered_map<Vertex<Web>*, int> liveDeg;
        for (auto v : graph->getVertexSet())
            liveDeg[v] = (int)v->getAdj().size();

        std::stack<Vertex<Web>*> colorStack;

        bool changed = true;
        while (changed) {
            changed = false;

            for (auto v : graph->getVertexSet()) {
                if (v->isVisited()) continue;
                if (liveDeg[v] < num) {
                    colorStack.push(v);
                    v->setVisited(true);

                    for (auto edge : v->getAdj()) {
                        auto nb = edge->getDest();
                        if (!nb->isVisited())
                            liveDeg[nb]--;
                    }

                    changed = true;
                }
            }

            // No progress but live nodes remain → graph is not N-colorable.
            // Leave all colors at -1 so the caller loads everything from memory.
            if (!changed) {
                for (auto v : graph->getVertexSet())
                    if (!v->isVisited())
                        return false;
            }
        }

        while (!colorStack.empty()) {
            auto v = colorStack.top();
            colorStack.pop();

            std::set<int> usedColors;
            for (auto edge : v->getAdj()) {
                int c = edge->getDest()->getColor();
                if (c >= 0)
                    usedColors.insert(c);
            }

            // Lowest non-negative integer not used by any neighbour.
            // Guaranteed to be < N because degree was < N when pushed.
            int color = 0;
            while (usedColors.count(color)>0) {
                color++;
            }
            v->setColor(color);
        }

        return true;
    }