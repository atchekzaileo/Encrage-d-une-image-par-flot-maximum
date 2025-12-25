#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <vector>
#include "Image.h"

class MaxFlow {
public:
    explicit MaxFlow(const Image& image);

    int calculerFlotMaximum();
    void calculerEnsembleSource(std::vector<bool>& dansSource) const;

private:
    struct Edge {
        int to;
        int cap;
        int rev;
    };

    const Image& img;
    int L;
    int C;
    int nbPixels;
    int source;
    int puits;
    int N;

    std::vector<std::vector<Edge>> adj;

    void ajouterArete(int u, int v, int cap);
    bool bfs(std::vector<int>& parentSommet, std::vector<int>& parentArete);

    void indiceVersCoord(int k, int& i, int& j) const;
};

#endif
