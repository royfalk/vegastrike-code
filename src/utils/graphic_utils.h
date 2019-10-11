#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include "gfxlib_struct.h"

class graphic_utils {
public:
    graphic_utils();
    static void ScaleByAlpha(GFXColorVertex &vert, float alpha);

    static void floatArrayToDoubleArray(const float floatArray[], double doubleArray[], int size);
    static void doubleArrayToFloatArray(const double doubleArray[], float floatArray[], int size);

};

#endif // GRAPHIC_UTILS_H
