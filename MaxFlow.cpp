#include "MaxFlow.h"
#include <queue>

MaxFlow::MaxFlow(const Image& image) : img(image) {
    L = img.getLignes();
    C = img.getColonnes();
    nbPixels = L * C;

    source = nbPixels;
    puits = nbPixels + 1;
    N = nbPixels + 2;

    adj.assign(N, {});

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            const int k = i * C + j;
            const Pixel& p = img.at(i, j);

            if (p.cap[SOURCE] > 0) ajouterArete(source, k, p.cap[SOURCE]);
            if (p.cap[PUITS] > 0) ajouterArete(k, puits, p.cap[PUITS]);

            if (i + 1 < L) {
                const int kSud = (i + 1) * C + j;
                const int capSud = p.cap[SUD];
                if (capSud > 0) {
                    ajouterArete(k, kSud, capSud);
                    ajouterArete(kSud, k, capSud);
                }
            }

            if (j + 1 < C) {
                const int kEst = i * C + (j + 1);
                const int capEst = p.cap[EST];
                if (capEst > 0) {
                    ajouterArete(k, kEst, capEst);
                    ajouterArete(kEst, k, capEst);
                }
            }
        }
    }
}

void MaxFlow::ajouterArete(int u, int v, int cap) {
    Edge a{v, cap, static_cast<int>(adj[v].size())};
    Edge b{u, 0, static_cast<int>(adj[u].size())};
    adj[u].push_back(a);
    adj[v].push_back(b);
}

bool MaxFlow::bfs(std::vector<int>& parentSommet, std::vector<int>& parentArete) {
    parentSommet.assign(N, -1);
    parentArete.assign(N, -1);

    std::queue<int> q;
    q.push(source);
    parentSommet[source] = source;

    while (!q.empty()) {
        const int u = q.front();
        q.pop();

        if (u == puits) return true;

        for (int i = 0; i < static_cast<int>(adj[u].size()); ++i) {
            const Edge& e = adj[u][i];
            if (e.cap <= 0 || parentSommet[e.to] != -1) continue;
            parentSommet[e.to] = u;
            parentArete[e.to] = i;
            q.push(e.to);
        }
    }

    return false;
}

int MaxFlow::calculerFlotMaximum() {
    int flotMax = 0;

    std::vector<int> parentSommet(N);
    std::vector<int> parentArete(N);

    while (bfs(parentSommet, parentArete)) {
        int delta = 1000000000;

        for (int v = puits; v != source; v = parentSommet[v]) {
            const int u = parentSommet[v];
            const int idx = parentArete[v];
            const int cap = adj[u][idx].cap;
            if (cap < delta) delta = cap;
        }

        for (int v = puits; v != source; v = parentSommet[v]) {
            const int u = parentSommet[v];
            const int idx = parentArete[v];

            Edge& e = adj[u][idx];
            Edge& rev = adj[v][e.rev];

            e.cap -= delta;
            rev.cap += delta;
        }

        flotMax += delta;
    }

    return flotMax;
}

void MaxFlow::indiceVersCoord(int k, int& i, int& j) const {
    i = k / C;
    j = k % C;
}

void MaxFlow::calculerEnsembleSource(std::vector<bool>& dansSource) const {
    dansSource.assign(nbPixels, false);

    std::vector<bool> visite(N, false);
    std::queue<int> q;

    visite[source] = true;
    q.push(source);

    while (!q.empty()) {
        const int u = q.front();
        q.pop();

        for (const auto& e : adj[u]) {
            if (e.cap <= 0 || visite[e.to]) continue;
            visite[e.to] = true;
            q.push(e.to);
        }
    }

    for (int k = 0; k < nbPixels; ++k) {
        if (visite[k]) dansSource[k] = true;
    }
}
