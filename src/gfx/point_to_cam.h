#ifndef _POINT_TO_CAM_H_
#define _POINT_TO_CAM_H_

/**
 * Calculates the way a point-like sphere should be oriented, with a given
 * input position.  Pos will be tweaked, and p,q,r to be used will be returned.
 * true will be returned if it is close enough to be rendered
 * Uses current camera orientation
 * If local_transformation is used, the object will attempt to use that to keep
 * Its own up alignment, otherwise it will match the camera up
 * If offsetbyradius is used the animation will appear closer than it is
 * If moveiftoofar is true and also offset by radius is true then the item will
 * be moved closer to the cam if it is too far and shrunken appropriately
 * Fixed 052402 to have floating -> double for important values
 *
 */

#include "gfx/camera.h"

inline bool CalculateOrientation( Vector &Pos,
                                  Vector &p,
                                  Vector &q,
                                  Vector &r,
                                  float &wid,
                                  float &hei,
                                  float OffsetByThisPercent,
                                  bool moveiftoofar,
                                  Matrix *local_transformation = NULL )
{
    const float kkkk = 3;     //this seems to work for no good reason
    _Universe->AccessCamera()->GetPQR( p, q, r );
    Vector     OLDR( r.i, r.j, r.k );
    Vector     offset( _Universe->AccessCamera()->GetPosition()-Pos );
    double offz = -OLDR.Dot( offset );
    if (OffsetByThisPercent != 0) {
        double offmag = offset.Magnitude();
        double rad    = wid > hei ? wid : hei;
        offset *= 1./offmag;
        if ( (!moveiftoofar) || offz < rad+.4*g_game.zfar ) {
            if (offz-OffsetByThisPercent*rad < 2*g_game.znear)
                rad = (offz-2*g_game.znear)/OffsetByThisPercent;
            offset *= OffsetByThisPercent*rad;
        } else {
            offset *= (offmag/offz)*(offz-2*g_game.znear);                 //-rad-.4*g_game.zfar);
            wid    /= ( (offz)/(kkkk*g_game.znear) );                 //it's 1 time away from znear
            hei    /= ( (offz)/(kkkk*g_game.znear) );
        }
        Pos  += offset;
        offz += OLDR.Dot( offset );         //coming closer so this means that offz is less
    }
    if (local_transformation) {
        Vector  q1( local_transformation->r[1], local_transformation->r[4], local_transformation->r[7] );
        Vector  p1( ( q1.Dot( q ) )*q );
        q     = ( q1.Dot( p ) )*p+p1;
        Vector posit = _Universe->AccessCamera()->GetPosition();

        posit = posit-local_transformation->p;

        Normalize( posit );
        r.i   = posit.i;
        r.j   = posit.j;
        r.k   = posit.k;
        ScaledCrossProduct( q, r, p );
        ScaledCrossProduct( r, p, q );
        //if the vectors are linearly dependant we're phucked :) fun fun fun
    }
    return offz < .4*g_game.zfar;
}

#endif

