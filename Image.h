#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include "Pixel.h"

// Indices utilisés dans les tableaux cap[] et flot[].
// Ça rend le code plus lisible que des 0,1,2...
enum Directions {
    NORD   = 0,
    SUD    = 1,
    OUEST  = 2,
    EST    = 3,
    SOURCE = 4,
    PUITS  = 5
};

// Classe représentant une image en niveaux de gris.
class Image {
public:
    // Constructeur par défaut : image vide.
    Image();

    // Lecture au format PGM ASCII (P2).
    // Retourne true si la lecture est correcte.
    bool chargerPGM(const char* nomFichier);

    // Écriture au format PGM ASCII (P2).
    bool sauvegarderPGM(const char* nomFichier) const;

    // Dimensions.
    int getLignes() const;
    int getColonnes() const;

    // Accès à un pixel (i,j).
    Pixel& at(int i, int j);
    const Pixel& at(int i, int j) const;

    // Test si (i,j) est à l'intérieur de l'image.
    bool dansImage(int i, int j) const;

    // Capacités entre pixels voisins (N, S, O, E).
    void calculerCapacites(double H, double sigma);

    // Capacités entre source / puits et les pixels.
    void calculerCapacitesSourcePuits(double alpha);

private:
    int L; // nombre de lignes
    int C; // nombre de colonnes

    // Tableau 1D de taille L*C stockant les pixels.
    std::vector<Pixel> tab;

    // Convertit des coordonnées (i,j) en indice dans le tableau tab.
    int index(int i, int j) const;
};

#endif
