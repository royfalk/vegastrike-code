#ifndef CUBICSPLINES_H
#define CUBICSPLINES_H

//Spline
/* Cubic spline interpolation functions<p>
 *
 *       <b>Historique : </b><font size=-1><ul>
 *     <li>16/07/02 - Egg - Added methods to access slope per axis
 *          <li>28/05/00 - Egg - Javadocisation, minor changes & optimizations,
 *                          Renamed TSpline to TCubicSpline, added W component
 *                          and a bunch of helper methods
 *          <li>20/05/00 - RoC - Created, based on the C source code from Eric
 *       </ul></font>
 */

//#define SPLINE_METHOD1
#define SPLINE_METHOD2
//#define SPLINE_METHOD3

#include "gfx/vec.h"

class CubicSpline
{
private:
//Private Declarations
#ifdef SPLINE_METHOD1
    double **MatX;
    double **MatY;
    double **MatZ;
    double **MatW;
    int FNb;
#endif
#ifdef SPLINE_METHOD2
    double A, B, C, D, E, F, G, H, I, J, K, L;
#endif
#ifdef SPLINE_METHOD3
#endif

public:
//Public Declarations
/* Creates the spline and declares interpolation points.<p>
 *  Time references go from 0 (first point) to nb-1 (last point), the
 *  first and last reference matrices respectively are used when T is
 *  used beyond this range.<p>
 *  Note : "nil" single arrays are accepted, in this case the axis is
 *  disabled and calculus will return 0 (zero) for this component. */
    CubicSpline();
    ~CubicSpline();

    void createSpline( Vector P0, Vector P1, Vector P2, Vector P3 );
//Calculates affine vector at time t.<p>
    Vector computePoint( double t ) const;
};

#endif /* CUBICSPLINES_H */

