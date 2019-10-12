#ifndef __HALO_H
#define __HALO_H

#include "gfxlib.h"
#include "vec.h"
#include "quaternion.h"

#define TranslucentWhite ( GFXColor( 1, 1, 1, .5 ) )
#define ZeroQvector ( Vector( 0, 0, 0 ) )

class Halo
{
    Vector position;
    float   sizex;
    float   sizey;
    int     decal;
    int     quadnum;
public: ~Halo();
    Halo( const char *texture,
          const GFXColor &col = TranslucentWhite,
          const Vector &pos = ZeroQvector,
          float sizx = 1,
          float sizy = 1 );
    void Draw( const Transformation &quat = identity_transformation, const Matrix &m = identity_matrix, float alpha = -1 );
    static void ProcessDrawQueue();
    void SetDimensions( float wid, float hei )
    {
        sizex = wid;
        sizey = hei;
    }
    void SetPosition( const Vector &k )
    {
        position = k;
    }
    Vector& Position()
    {
        return position;
    }
    void SetColor( const GFXColor &col );
    void GetDimensions( float &wid, float &hei )
    {
        wid = sizex;
        hei = sizey;
    }
};

#endif

