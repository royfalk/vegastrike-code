#include "graphic_utils.h"

graphic_utils::graphic_utils() {
}

void graphic_utils::ScaleByAlpha(GFXColorVertex &vert, float alpha) {
    if (alpha < 1) {
        vert.r *= alpha;
        vert.g *= alpha;
        vert.b *= alpha;
    }
}
