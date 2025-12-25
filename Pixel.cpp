#include "Pixel.h"

Pixel::Pixel() : intensite(0) {
    for (int i = 0; i < 6; ++i) {
        cap[i] = 0;
        flot[i] = 0;
    }
}
