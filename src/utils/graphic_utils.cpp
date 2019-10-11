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

void graphic_utils::floatArrayToDoubleArray(const float floatArray[], double doubleArray[], int size) {
  for(int i = 0; i<size;i++)
    doubleArray[i] = floatArray[i];
}

void graphic_utils::doubleArrayToFloatArray(const double doubleArray[], float floatArray[], int size) {
  for(int i = 0; i<size;i++)
    floatArray[i] = doubleArray[i];
}
