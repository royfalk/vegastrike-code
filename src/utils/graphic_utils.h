#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include "gfxlib_struct.h"

class graphic_utils {
public:
    graphic_utils();
    static void ScaleByAlpha(GFXColorVertex &vert, float alpha);
};

#endif // GRAPHIC_UTILS_H
