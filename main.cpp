#include "Image.h"
#include "MaxFlow.h"
#include <iostream>
#include <vector>

int main() {
    Image img;

    std::cout << "Entrez le nom du fichier PGM (ASCII P2) : ";
    std::string nomFichier;
    std::cin >> nomFichier;

    if (!img.chargerPGM(nomFichier.c_str())) {
        std::cout << "Echec du chargement de l'image.\n";
        return 1;
    }

    const int L = img.getLignes();
    const int C = img.getColonnes();

    const double H = 100.0;
    const double sigma = 5.0;
    const double alpha = 400.0;

    img.calculerCapacites(H, sigma);
    img.calculerCapacitesSourcePuits(alpha);

    MaxFlow mf(img);
    const int flotMax = mf.calculerFlotMaximum();
    std::cout << "Flot maximum : " << flotMax << "\n";

    std::vector<bool> dansSource;
    mf.calculerEnsembleSource(dansSource);

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            const int k = i * C + j;
            img.at(i, j).intensite = dansSource[k] ? 255 : 0;
        }
    }

    const char* nomSortie = "resultat.pgm";
    if (!img.sauvegarderPGM(nomSortie)) {
        std::cout << "Erreur lors de la sauvegarde.\n";
        return 1;
    }

    std::cout << "Image segmentee sauvegardee dans " << nomSortie << "\n";
    return 0;
}
