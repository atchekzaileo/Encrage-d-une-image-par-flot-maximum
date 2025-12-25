#include "Image.h"
#include "MaxFlow.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    Image img;

    char nomFichier[256];
    cout << "Entrez le nom du fichier PGM (format ASCII P2) : ";
    cin >> nomFichier;

    // 1) Chargement de l'image.
    if (!img.chargerPGM(nomFichier)) {
        cout << "Echec du chargement de l'image." << endl;
        return 1;
    }

    cout << "Image chargee avec succes." << endl;
    cout << "Dimensions : " << img.getLignes()
         << " lignes, " << img.getColonnes()
         << " colonnes." << endl;

    // Petit affichage pour vérifier la lecture.
    if (img.getLignes() > 0) {
        cout << "Premiere ligne des intensites :" << endl;
        for (int j = 0; j < img.getColonnes(); ++j) {
            cout << img.at(0, j).intensite << " ";
        }
        cout << endl;
    }

    int L = img.getLignes();
    int C = img.getColonnes();

    // Paramètres du modèle de capacités (à adapter selon le sujet).
    double H     = 100.0;
    double sigma = 5.0;
    double alpha = 400.0;

    // 2) Capacités entre pixels voisins.
    img.calculerCapacites(H, sigma);

    // 3) Capacités entre source / puits et les pixels.
    img.calculerCapacitesSourcePuits(alpha);

    // Exemple d'affichage des capacités pour le pixel (0,0).
    if (L > 0 && C > 0) {
        Pixel& p00 = img.at(0, 0);
        cout << "\nCapacites du pixel (0,0) :" << endl;
        cout << "Nord   = " << p00.cap[NORD]   << endl;
        cout << "Sud    = " << p00.cap[SUD]    << endl;
        cout << "Ouest  = " << p00.cap[OUEST]  << endl;
        cout << "Est    = " << p00.cap[EST]    << endl;
        cout << "Source = " << p00.cap[SOURCE] << endl;
        cout << "Puits  = " << p00.cap[PUITS]  << endl;
    }

    // 4) Construction du graphe et calcul du flot maximum (Edmonds-Karp).
    MaxFlow mf(img);
    int flotMax = mf.calculerFlotMaximum();
    cout << "\nFlot maximum calcule : " << flotMax << endl;

    // 5) Ensemble des pixels côté source dans la coupure minimale.
    std::vector<bool> dansSource;
    mf.calculerEnsembleSource(dansSource);

    // 6) Construction de l'image segmentée binaire :
    //    - 255 (blanc) pour les pixels dans l'ensemble source (objet),
    //    - 0   (noir)  pour les autres (fond).
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int k = i * C + j;
            if (dansSource[k]) {
                img.at(i, j).intensite = 255; // objet en blanc
            } else {
                img.at(i, j).intensite = 0;   // fond en noir
            }
        }
    }

    // 7) Sauvegarde du résultat.
    const char* nomSortie = "resultat.pgm";
    if (img.sauvegarderPGM(nomSortie)) {
        cout << "Image segmentee sauvegardee dans " << nomSortie << endl;
    } else {
        cout << "Erreur lors de la sauvegarde de l'image segmentee." << endl;
    }

    return 0;
}
