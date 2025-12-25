#ifndef PIXEL_H
#define PIXEL_H

// Représente un pixel de l'image.
// On stocke :
//  - son intensité (niveau de gris)
//  - les capacités et les flots pour le graphe de flot.
struct Pixel {
    int intensite; // niveau de gris du pixel (0..255)

    int cap[6];    // capacités pour chaque direction (N, S, O, E, SOURCE, PUITS)
    int flot[6];   // flots sur ces arcs (pas indispensable pour la segmentation)

    // Constructeur par défaut :
    // on initialise tout à 0 pour partir d'un état propre.
    Pixel();
};

#endif
