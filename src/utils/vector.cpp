#include "vector.h"
#include <math.h>


RFVector::RFVector() {}

//https://stackoverflow.com/questions/3021333/can-i-use-memcpy-in-c-to-copy-classes-that-have-no-pointers-or-virtual-functio
/*template < class vec >Vector::Vector( const vec &in) {
    memcpy( this, &in, sizeof (*this) );
}*/

/*Vector::Vector(const Vector &in) {
    memcpy( this, &in, sizeof (*this) );
}*/

/*Vector::Vector(float x, float y, float z) {
    i = x;
    j = y;
    k = z;
}*/

RFVector::RFVector(float x, float y, float z) {
    i = x;
    j = y;
    k = z;
}

RFVector::RFVector(float x, float y, float z, float s, float t) {
    i = x;
    j = y;
    k = z;
    this->s = s;
    this->t = t;
}

float RFVector::Mag() {
    return sqrtf(i*i + j*j + k*k);
}

RFVector RFVector::Cross( const RFVector &v ) const {
    return RFVector(j*v.k-k*v.j, k*v.i-i*v.k, i*v.j-j*v.i);
}
void RFVector::Yaw( float rad ) { //only works with unit vector
    float theta = 0;
    float m = Mag();
    if (i > 0)
        theta = static_cast<float>(atan( k/i));
    else if (i < 0)
        theta = M_PI + static_cast<float>(atan( k/i));
    else if (k <= 0 && i == 0)
        theta = -M_PI/2;
    else if (k > 0 && i == 0)
        theta = M_PI/2;
    theta += rad;
    i      = m*cosf( theta );
    k      = m*sinf( theta );
}

void RFVector::Roll( float rad )
{
    float theta = 0;
    float m = Mag();
    if (i > 0)
        theta = (float) atan( j/i );
    else if (i < 0)
        theta = M_PI+(float) atan( j/i );
    else if (j <= 0 && i == 0)
        theta = -M_PI/2;
    else if (j > 0 && i == 0)
        theta = M_PI/2;
    theta += rad;
    i      = m*cosf( theta );
    j      = m*sinf( theta );
}

void RFVector::Pitch( float rad )
{
    float theta = 0;
    float m = Mag();
    if (k > 0)
        theta = (float) atan( j/k );
    else if (k < 0)
        theta = M_PI+(float) atan( j/k );
    else if (j <= 0 && k == 0)
        theta = -M_PI/2;
    else if (j > 0 && k == 0)
        theta = M_PI/2;
    theta += rad;
    k      = m*cosf( theta );
    j      = m*sinf( theta );
}
