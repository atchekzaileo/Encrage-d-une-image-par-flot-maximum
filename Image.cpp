#include "Image.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

Image::Image() : L(0), C(0) {}

int Image::getLignes() const { return L; }
int Image::getColonnes() const { return C; }

int Image::index(int i, int j) const { return i * C + j; }

Pixel& Image::at(int i, int j) { return tab[index(i, j)]; }
const Pixel& Image::at(int i, int j) const { return tab[index(i, j)]; }

bool Image::dansImage(int i, int j) const { return i >= 0 && i < L && j >= 0 && j < C; }

bool Image::chargerPGM(const char* nomFichier) {
    std::ifstream ifs(nomFichier);
    if (!ifs) {
        std::cerr << "Impossible d'ouvrir le fichier " << nomFichier << " en lecture.\n";
        return false;
    }

    std::string magic;
    ifs >> magic;
    if (!ifs || magic != "P2") {
        std::cerr << "Erreur : format PGM ASCII (P2) attendu.\n";
        return false;
    }

    while (true) {
        ifs >> std::ws;
        if (ifs.peek() != '#') break;
        std::string ligne;
        std::getline(ifs, ligne);
    }

    int largeur = 0, hauteur = 0;
    ifs >> largeur >> hauteur;
    if (!ifs || largeur <= 0 || hauteur <= 0) {
        std::cerr << "Erreur de lecture de la largeur / hauteur.\n";
        return false;
    }

    int maxval = 0;
    ifs >> maxval;
    if (!ifs || maxval <= 0) {
        std::cerr << "Valeur maximum invalide dans le PGM : " << maxval << "\n";
        return false;
    }

    C = largeur;
    L = hauteur;
    tab.assign(L * C, Pixel{});

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int g = 0;
            ifs >> g;
            if (!ifs) {
                std::cerr << "Erreur de lecture des niveaux de gris.\n";
                return false;
            }
            if (g < 0) g = 0;
            if (g > maxval) g = maxval;
            at(i, j).intensite = g;
        }
    }

    return true;
}

bool Image::sauvegarderPGM(const char* nomFichier) const {
    std::ofstream ofs(nomFichier);
    if (!ofs) {
        std::cerr << "Impossible d'ouvrir le fichier " << nomFichier << " en écriture.\n";
        return false;
    }

    ofs << "P2\n" << C << " " << L << "\n255\n";

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            int g = at(i, j).intensite;
            if (g < 0) g = 0;
            if (g > 255) g = 255;
            ofs << g << (j + 1 == C ? '\n' : ' ');
        }
    }

    return true;
}

void Image::calculerCapacites(double H, double sigma) {
    if (sigma <= 0.0) {
        for (int i = 0; i < L; ++i) {
            for (int j = 0; j < C; ++j) {
                Pixel& p = at(i, j);
                p.cap[NORD] = p.cap[SUD] = p.cap[OUEST] = p.cap[EST] = 0;
                p.cap[SOURCE] = 0;
                p.cap[PUITS] = 0;
            }
        }
        return;
    }

    const double denom = 2.0 * sigma * sigma;

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            Pixel& p = at(i, j);
            const double Ip = static_cast<double>(p.intensite);

            p.cap[NORD] = 0;
            p.cap[SUD] = 0;
            p.cap[OUEST] = 0;
            p.cap[EST] = 0;

            if (dansImage(i - 1, j)) {
                const double diff = Ip - static_cast<double>(at(i - 1, j).intensite);
                p.cap[NORD] = static_cast<int>(H * std::exp(-(diff * diff) / denom));
            }
            if (dansImage(i + 1, j)) {
                const double diff = Ip - static_cast<double>(at(i + 1, j).intensite);
                p.cap[SUD] = static_cast<int>(H * std::exp(-(diff * diff) / denom));
            }
            if (dansImage(i, j - 1)) {
                const double diff = Ip - static_cast<double>(at(i, j - 1).intensite);
                p.cap[OUEST] = static_cast<int>(H * std::exp(-(diff * diff) / denom));
            }
            if (dansImage(i, j + 1)) {
                const double diff = Ip - static_cast<double>(at(i, j + 1).intensite);
                p.cap[EST] = static_cast<int>(H * std::exp(-(diff * diff) / denom));
            }

            p.cap[SOURCE] = 0;
            p.cap[PUITS] = 0;
        }
    }
}

void Image::calculerCapacitesSourcePuits(double alpha) {
    const double eps = 1e-6;

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < C; ++j) {
            Pixel& p = at(i, j);
            const double I = static_cast<double>(p.intensite);

            double p_objet = (255.0 - I) / 255.0;
            double p_fond = I / 255.0;

            if (p_objet < eps) p_objet = eps;
            if (p_objet > 1.0 - eps) p_objet = 1.0 - eps;
            if (p_fond < eps) p_fond = eps;
            if (p_fond > 1.0 - eps) p_fond = 1.0 - eps;

            const double capS_double = -alpha * std::log(p_objet);
            const double capP_double = -alpha * std::log(p_fond);

            int capS = static_cast<int>(std::round(capS_double));
            int capP = static_cast<int>(std::round(capP_double));

            if (capS_double > 0.0 && capS <= 0) capS = 1;
            if (capP_double > 0.0 && capP <= 0) capP = 1;

            p.cap[SOURCE] = capS;
            p.cap[PUITS] = capP;
        }
    }
}
