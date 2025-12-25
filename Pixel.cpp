#include "Pixel.h"

// Constructeur par défaut.
// On met intensité, capacités et flots à 0.
Pixel::Pixel() {
    intensite = 0;
    for (int i = 0; i < 6; ++i) {
        cap[i]  = 0;
        flot[i] = 0;
    }
}
