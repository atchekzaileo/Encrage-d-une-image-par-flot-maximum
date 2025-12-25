#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <vector>
#include "Image.h"

// Classe qui construit un graphe de flot à partir d'une Image,
// puis calcule le flot maximum entre une source et un puits.
//
// Ici on utilise l'algorithme d'Edmonds-Karp :
//  - à chaque étape, on cherche un chemin augmentant par BFS,
//  - on augmente le flot le long de ce chemin,
//  - on recommence jusqu'à ce qu'il n'y ait plus de chemin.
//
class MaxFlow {
public:
    // Construit le graphe de flot à partir des capacités stockées dans l'image.
    MaxFlow(const Image& image);

    // Calcule le flot maximum (algorithme Edmonds-Karp).
    int calculerFlotMaximum();

    // Après le flot max, marque dans "dansSource" quels pixels
    // sont encore atteignables depuis la source dans le graphe résiduel.
    void calculerEnsembleSource(std::vector<bool>& dansSource) const;

private:
    // Représentation d'une arête dans le graphe.
    struct Edge {
        int to;   // sommet d'arrivée
        int cap;  // capacité résiduelle
        int rev;  // indice de l'arête inverse dans adj[to]
    };

    const Image& img;
    int L;
    int C;
    int nbPixels;
    int source;
    int puits;
    int N; // nombre total de sommets

    // Listes d'adjacence : adj[u] = liste des arêtes sortantes de u.
    std::vector< std::vector<Edge> > adj;

    // Ajoute une arête u->v de capacité "cap" plus l'arête inverse v->u de cap 0.
    void ajouterArete(int u, int v, int cap);

    // BFS dans le graphe résiduel pour chercher un chemin augmentant.
    // Remplit parentSommet[v] et parentArete[v] pour pouvoir reconstruire le chemin.
    bool bfs(std::vector<int>& parentSommet, std::vector<int>& parentArete);

    // Convertit un indice de pixel k en coordonnées (i,j).
    void indiceVersCoord(int k, int& i, int& j) const;
};

#endif
