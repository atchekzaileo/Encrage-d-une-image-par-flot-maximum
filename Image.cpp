#include "Image.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath> // exp, log, round

using namespace std;

Image::Image() {
    L = 0;
    C = 0;
}

int Image::getLignes() const {
    return L;
}

int Image::getColonnes() const {
    return C;
}

// Transforme des coordonnées (i,j) en indice dans le tableau 1D.
int Image::index(int i, int j) const {
    return i * C + j;
}

Pixel& Image::at(int i, int j) {
    return tab[index(i, j)];
}

const Pixel& Image::at(int i, int j) const {
    return tab[index(i, j)];
}

// Vérifie que (i,j) est bien à l'intérieur de l'image.
bool Image::dansImage(int i, int j) const {
    return (i >= 0 && i < L && j >= 0 && j < C);
}

// -----------------------------------------------------------------------------
// CHARGEMENT D'UNE IMAGE PGM ASCII (P2)
// -----------------------------------------------------------------------------
bool Image::chargerPGM(const char* nomFichier) {
    ifstream ifs;
    ifs.open(nomFichier);
    if (!ifs) {
        cerr << "Impossible d'ouvrir le fichier " << nomFichier << " en lecture." << endl;
        return false;
    }

    // Le fichier doit commencer par "P2".
    string magic;
    ifs >> magic;
    if (!ifs || magic != "P2") {
        cerr << "Erreur : format PGM ASCII (P2) attendu." << endl;
        return false;
    }

    // On ignore les lignes de commentaires qui commencent par '#'.
    while (true) {
        ifs >> ws;           // enlève espaces, retours à la ligne, etc.
        int c = ifs.peek();  // regarde le prochain caractère sans le lire

        if (c == '#') {
            string ligne;
            getline(ifs, ligne); // lit et ignore la ligne entière
        } else {
            break; // ce n'est plus un commentaire
        }
    }

    // Lecture largeur / hauteur.
    int largeur, hauteur;
    ifs >> largeur >> hauteur;
    if (!ifs) {
        cerr << "Erreur de lecture de la largeur / hauteur." << endl;
        return false;
    }

    // Lecture de la valeur max (souvent 255).
    int maxval;
    ifs >> maxval;
    if (!ifs) {
        cerr << "Erreur de lecture de la valeur maximum." << endl;
        return false;
    }
    if (maxval <= 0) {
        cerr << "Valeur maximum invalide dans le PGM : " << maxval << endl;
        return false;
    }

    // On met à jour les dimensions et on alloue le tableau de pixels.
    C = largeur;
    L = hauteur;
    tab.clear();
    tab.resize(L * C);

    // Lecture des L*C niveaux de gris.
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int g;
            ifs >> g;
            if (!ifs) {
                cerr << "Erreur de lecture des niveaux de gris." << endl;
                return false;
            }

            // On s'assure que g reste dans [0, maxval].
            if (g < 0) g = 0;
            if (g > maxval) g = maxval;

            tab[index(i, j)].intensite = g;
        }
    }

    ifs.close();
    return true;
}

// -----------------------------------------------------------------------------
// SAUVEGARDE D'UNE IMAGE PGM ASCII (P2)
// -----------------------------------------------------------------------------
bool Image::sauvegarderPGM(const char* nomFichier) const {
    ofstream ofs;
    ofs.open(nomFichier);
    if (!ofs) {
        cerr << "Impossible d'ouvrir le fichier " << nomFichier << " en écriture." << endl;
        return false;
    }

    ofs << "P2\n";
    ofs << C << " " << L << "\n";
    ofs << 255 << "\n";

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int g = at(i, j).intensite;
            if (g < 0) g = 0;
            if (g > 255) g = 255;
            ofs << g << " ";
        }
        ofs << "\n";
    }

    ofs.close();
    return true;
}

// -----------------------------------------------------------------------------
// CALCUL DES CAPACITES ENTRE PIXELS VOISINS (N, S, O, E)
// -----------------------------------------------------------------------------
//
// Cap(p,q) = H * exp( - (I(p) - I(q))^2 / (2*sigma^2) )
//
void Image::calculerCapacites(double H, double sigma) {
    double denom = 2.0 * sigma * sigma;

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            Pixel& p = at(i, j);
            double Ip = (double)p.intensite;

            // Nord
            if (dansImage(i - 1, j)) {
                double Iq = (double)at(i - 1, j).intensite;
                double diff = Ip - Iq;
                p.cap[NORD] = (int)(H * exp(-(diff * diff) / denom));
            } else {
                p.cap[NORD] = 0;
            }

            // Sud
            if (dansImage(i + 1, j)) {
                double Iq = (double)at(i + 1, j).intensite;
                double diff = Ip - Iq;
                p.cap[SUD] = (int)(H * exp(-(diff * diff) / denom));
            } else {
                p.cap[SUD] = 0;
            }

            // Ouest
            if (dansImage(i, j - 1)) {
                double Iq = (double)at(i, j - 1).intensite;
                double diff = Ip - Iq;
                p.cap[OUEST] = (int)(H * exp(-(diff * diff) / denom));
            } else {
                p.cap[OUEST] = 0;
            }

            // Est
            if (dansImage(i, j + 1)) {
                double Iq = (double)at(i, j + 1).intensite;
                double diff = Ip - Iq;
                p.cap[EST] = (int)(H * exp(-(diff * diff) / denom));
            } else {
                p.cap[EST] = 0;
            }

            // Les capacités SOURCE et PUITS seront remplies après.
            p.cap[SOURCE] = 0;
            p.cap[PUITS]  = 0;
        }
    }
}

// -----------------------------------------------------------------------------
// CALCUL DES CAPACITES SOURCE / PUITS
// -----------------------------------------------------------------------------
void Image::calculerCapacitesSourcePuits(double alpha) {
    const double eps = 1e-6; // pour éviter log(0)

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            Pixel& p = at(i, j);
            double I = (double)p.intensite;

            // p_objet élevé = pixel sombre = plutôt objet
            double p_objet = (255.0 - I) / 255.0;
            // p_fond élevé  = pixel clair  = plutôt fond
            double p_fond  = I / 255.0;

            if (p_objet < eps)       p_objet = eps;
            if (p_objet > 1.0 - eps) p_objet = 1.0 - eps;
            if (p_fond  < eps)       p_fond  = eps;
            if (p_fond  > 1.0 - eps) p_fond  = 1.0 - eps;

            double capS_double = -alpha * std::log(p_objet);
            double capP_double = -alpha * std::log(p_fond);

            int capS = (int)std::round(capS_double);
            int capP = (int)std::round(capP_double);

            // Si la valeur est >0 mais arrondie à 0, on force à 1.
            if (capS_double > 0.0 && capS <= 0) capS = 1;
            if (capP_double > 0.0 && capP <= 0) capP = 1;

            p.cap[SOURCE] = capS;
            p.cap[PUITS]  = capP;
        }
    }
}
