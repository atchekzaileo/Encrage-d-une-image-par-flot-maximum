#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include "Pixel.h"

enum Directions {
    NORD = 0,
    SUD = 1,
    OUEST = 2,
    EST = 3,
    SOURCE = 4,
    PUITS = 5
};

class Image {
public:
    Image();

    bool chargerPGM(const char* nomFichier);
    bool sauvegarderPGM(const char* nomFichier) const;

    int getLignes() const;
    int getColonnes() const;

    Pixel& at(int i, int j);
    const Pixel& at(int i, int j) const;

    bool dansImage(int i, int j) const;

    void calculerCapacites(double H, double sigma);
    void calculerCapacitesSourcePuits(double alpha);

private:
    int L;
    int C;
    std::vector<Pixel> tab;

    int index(int i, int j) const;
};

#endif
