#include "MaxFlow.h"
#include <queue>
#include <iostream>

using namespace std;

MaxFlow::MaxFlow(const Image& image)
    : img(image)
{
    // Récupération des dimensions de l'image.
    L = img.getLignes();
    C = img.getColonnes();
    nbPixels = L * C;

    // Indices pour la source et le puits dans le graphe.
    source = nbPixels;      // sommet source
    puits  = nbPixels + 1;  // sommet puits
    N      = nbPixels + 2;  // nb de sommets total

    // Préparation des listes d'adjacence.
    adj.clear();
    adj.resize(N);

    // -----------------------------------------------------------------
    // 1) Arcs source -> pixels (capacités cap[SOURCE]).
    // -----------------------------------------------------------------
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int k = i * C + j;
            const Pixel& p = img.at(i, j);
            int capSource = p.cap[SOURCE];
            if (capSource > 0) {
                ajouterArete(source, k, capSource);
            }
        }
    }

    // -----------------------------------------------------------------
    // 2) Arcs pixels -> puits (capacités cap[PUITS]).
    // -----------------------------------------------------------------
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int k = i * C + j;
            const Pixel& p = img.at(i, j);
            int capPuits = p.cap[PUITS];
            if (capPuits > 0) {
                ajouterArete(k, puits, capPuits);
            }
        }
    }

    // -----------------------------------------------------------------
    // 3) Arcs entre pixels voisins (voisinage 4-connexe).
    //
    // Optimisation simple : pour éviter de créer deux fois les mêmes
    // arêtes, on ne regarde que les voisins "vers le bas" (SUD)
    // et "vers la droite" (EST), et à chaque fois on ajoute les
    // deux sens (symétrique).
    // -----------------------------------------------------------------
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int k = i * C + j;
            const Pixel& p = img.at(i, j);

            // Voisin au sud (i+1, j)
            if (i < L - 1) {
                int kSud   = (i + 1) * C + j;
                int capSud = p.cap[SUD];
                if (capSud > 0) {
                    ajouterArete(k,    kSud, capSud); // k -> kSud
                    ajouterArete(kSud, k,    capSud); // kSud -> k
                }
            }

            // Voisin à l'est (i, j+1)
            if (j < C - 1) {
                int kEst   = i * C + (j + 1);
                int capEst = p.cap[EST];
                if (capEst > 0) {
                    ajouterArete(k,    kEst, capEst); // k -> kEst
                    ajouterArete(kEst, k,    capEst); // kEst -> k
                }
            }
        }
    }
}

// Ajoute une arête u->v de capacité "cap" et l'arête inverse v->u de cap 0.
void MaxFlow::ajouterArete(int u, int v, int cap) {
    Edge a;
    a.to  = v;
    a.cap = cap;
    a.rev = (int)adj[v].size(); // position de l'arête inverse dans adj[v]

    Edge b;
    b.to  = u;
    b.cap = 0;                  // capacité résiduelle de l'arête inverse
    b.rev = (int)adj[u].size(); // position de l'arête directe dans adj[u]

    adj[u].push_back(a);
    adj[v].push_back(b);
}

// BFS dans le graphe résiduel pour trouver un chemin augmentant.
// On remplit parentSommet[v] et parentArete[v] pour reconstruire le chemin
// si on atteint le puits.
bool MaxFlow::bfs(std::vector<int>& parentSommet, std::vector<int>& parentArete) {
    parentSommet.assign(N, -1);
    parentArete.assign(N, -1);

    std::queue<int> q;
    q.push(source);
    parentSommet[source] = source; // marque la source comme visitée

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        if (u == puits) {
            // On a trouvé un chemin jusqu'au puits.
            return true;
        }

        // Parcours de toutes les arêtes sortantes de u.
        for (int i = 0; i < (int)adj[u].size(); ++i) {
            const Edge& e = adj[u][i];

            // On suit seulement les arêtes avec capacité résiduelle > 0
            // et vers des sommets non encore visités.
            if (e.cap > 0 && parentSommet[e.to] == -1) {
                parentSommet[e.to] = u;
                parentArete[e.to]  = i;
                q.push(e.to);
            }
        }
    }

    // Aucun chemin augmentant trouvé.
    return false;
}

// Calcul du flot maximum avec Edmonds-Karp.
int MaxFlow::calculerFlotMaximum() {
    int flotMax = 0;

    std::vector<int> parentSommet(N);
    std::vector<int> parentArete(N);

    // Tant qu'il existe un chemin augmentant...
    while (bfs(parentSommet, parentArete)) {
        // On cherche la capacité résiduelle minimale le long du chemin.
        int delta = 1000000000; // valeur initiale très grande

        int v = puits;
        while (v != source) {
            int u   = parentSommet[v];
            int idx = parentArete[v];
            const Edge& e = adj[u][idx];
            if (e.cap < delta) {
                delta = e.cap;
            }
            v = u;
        }

        // On met à jour les capacités résiduelles le long du chemin.
        v = puits;
        while (v != source) {
            int u   = parentSommet[v];
            int idx = parentArete[v];

            Edge& e   = adj[u][idx];     // arête directe
            Edge& rev = adj[v][e.rev];   // arête inverse associée

            e.cap   -= delta;
            rev.cap += delta;

            v = u;
        }

        // On ajoute la quantité trouvée au flot total.
        flotMax += delta;
    }

    return flotMax;
}

// Convertit un indice de pixel en coordonnées (i,j).
void MaxFlow::indiceVersCoord(int k, int& i, int& j) const {
    i = k / C;
    j = k % C;
}

// Marque les sommets atteignables depuis la source dans le graphe résiduel final.
// Ces sommets forment l'ensemble "source" de la coupure minimale.
void MaxFlow::calculerEnsembleSource(std::vector<bool>& dansSource) const {
    dansSource.assign(nbPixels, false);

    std::vector<bool> visite(N, false);
    std::queue<int> q;

    q.push(source);
    visite[source] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int i = 0; i < (int)adj[u].size(); ++i) {
            const Edge& e = adj[u][i];
            if (e.cap > 0 && !visite[e.to]) {
                visite[e.to] = true;
                q.push(e.to);
            }
        }
    }

    // Les sommets 0..nbPixels-1 correspondent aux pixels.
    for (int k = 0; k < nbPixels; ++k) {
        if (visite[k]) {
            dansSource[k] = true;
        }
    }
}
