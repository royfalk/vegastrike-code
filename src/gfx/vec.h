#ifndef _3DMANIP_H_
#define _3DMANIP_H_

#include <math.h>

#include "endianness.h"
#include "vectorizable.h"




class Vector;

inline Vector operator*( const Vector &lval, const double obj );

inline Vector operator*( const double obj, const Vector &rval );

inline Vector& operator+=( Vector &lval, const Vector &obj );
inline Vector& operator-=( Vector &lval, const Vector &obj );

inline double DotProduct( const Vector &a, const Vector &b );
inline void Normalize( Vector &r );

class Vector
{
public:
    union
    {
        double i;
        double x;
    };
    union
    {
        double j;
        double y;
    };
    union
    {
        double k;
        double z;
    };

    Vector() : i(0),j(0),k(0){}
private:
    friend class Quadsquare;
    friend class QuadTree;
    friend class CoordinateSelect;
    friend class AIScript;
    friend class SphericalTransform;
public:
    inline Vector( double i, double j, double k )
    {
        this->i = i;
        this->j = j;
        this->k = k;
    }
    inline void Set( double x, double y, double z )
    {
        i = x;
        j = y;
        k = z;
    }

    void Yaw( double rad );
    void Roll( double rad );
    void Pitch( double rad );
    inline Vector Scale( double s ) const
    {
        return Vector( s*i, s*j, s*k );
    }
    inline Vector Transform( const Vector &p, const Vector &q, const Vector &r )
    {
        Vector tvect = Vector( DotProduct( *this, p ), DotProduct( *this, q ), DotProduct( *this, r ) );
        *this = tvect;
        return *this;
    }
    inline Vector operator+( const Vector &obj ) const
    {
        return Vector( i+obj.i, j+obj.j, k+obj.k );
    }
    inline Vector operator-( const Vector &obj ) const
    {
        return Vector( i-obj.i, j-obj.j, k-obj.k );
    }
    inline Vector Normalize()
    {
        ::Normalize( *this );
        return *this;
    }
    inline Vector operator-() const
    {
        return Vector( -i, -j, -k );
    }
    inline bool operator==( const Vector &b ) const
    {
        return i == b.i && j == b.j && k == b.k;
    }
    inline Vector Cross( const Vector &v ) const
    {
        return Vector( this->j*v.k-this->k*v.j,
                        this->k*v.i-this->i*v.k,
                        this->i*v.j-this->j*v.i );
    }
    inline double operator*( const Vector &b ) const
    {
        return i*b.i+j*b.j+k*b.k;
    }
    inline double Dot( const Vector &b ) const
    {
        return DotProduct( *this, b );
    }
    inline double Magnitude() const
    {
        return sqrt( i*i+j*j+k*k );
    }
    inline double MagnitudeSquared() const
    {
        return i*i+j*j+k*k;
    }
    inline Vector Vabs() const
    {
        return Vector( i >= 0 ? i : -i,
                        j >= 0 ? j : -j,
                        k >= 0 ? k : -k );
    }
    inline const Vector Transform( const class Matrix &m1 ) const;

    inline Vector Min( const Vector &other ) const
    {
        return Vector( (i < other.i) ? i : other.i,
                       (j < other.j) ? j : other.j,
                       (k < other.k) ? k : other.k );
    }
    inline Vector Max( const Vector &other ) const
    {
        return Vector( (i > other.i) ? i : other.i,
                       (j > other.j) ? j : other.j,
                       (k > other.k) ? k : other.k );
    }
    inline Vector& operator=( const Vector& other )
    {
        this->i = other.i;
        this->j = other.j;
        this->k = other.k;
        return *this;
    }
    Vector( struct _object* );
};

inline Vector operator/( const Vector &lval, const double obj )
{
    return lval * (double(1) / obj);
}

inline Vector& operator+=( Vector &lval, const Vector &obj )
{
    lval.i += obj.i;
    lval.j += obj.j;
    lval.k += obj.k;
    return lval;
}
inline Vector& operator-=( Vector &lval, const Vector &obj )
{
    lval.i -= obj.i;
    lval.j -= obj.j;
    lval.k -= obj.k;
    return lval;
}

inline Vector& operator*=( Vector &lval, const double &obj )
{
    lval.i *= obj;
    lval.j *= obj;
    lval.k *= obj;
    return lval;
}

inline void Normalize( Vector &r )
{
    double size = r.i*r.i+r.j*r.j+r.k*r.k;
    if (size > 0.00000000001) {
        double isize = double(1.0) / sqrt(size);
        r.i *= isize;
        r.j *= isize;
        r.k *= isize;
    }
}

inline double DotProduct( const Vector &a, const Vector &b )
{
    return a.i*b.i+a.j*b.j+a.k*b.k;
}

inline Vector operator*( const Vector &lval, const double obj )
{
    return Vector( lval.i*obj, lval.j*obj, lval.k*obj );
}
inline Vector operator*( const Vector &lval, const float obj )
{
    return Vector( lval.i*obj, lval.j*obj, lval.k*obj );
}

inline Vector operator*( const double obj, const Vector &rval )
{
    return Vector( rval.i*obj, rval.j*obj, rval.k*obj );
}

inline Vector operator*( const float obj, const Vector &rval )
{
    return Vector( rval.i*obj, rval.j*obj, rval.k*obj );
}
inline Vector operator*( const Vector &lval, const int obj )
{
    return Vector( lval.i*obj, lval.j*obj, lval.k*obj );
}

inline Vector operator*( const int obj, const Vector &rval )
{
    return Vector( rval.i*obj, rval.j*obj, rval.k*obj );
}

inline bool IsShorterThan(const Vector& a, double delta)
{
    return (a.MagnitudeSquared() < (delta * delta));
}

inline void ScaledCrossProduct( const Vector &a, const Vector &b, Vector &r )
{
    r.i = a.j*b.k-a.k*b.j;
    r.j = a.k*b.i-a.i*b.k;
    r.k = a.i*b.j-a.j*b.i;
    double size = sqrt( r.i*r.i+r.j*r.j+r.k*r.k );
    if (size < 0.00001) {
        r.i = r.j = r.k = 0;
    } else {
        r.i /= size;
        r.j /= size;
        r.k /= size;
    }
}

inline Vector PolygonNormal( Vector v1, Vector v2, Vector v3 )
{
    Vector temp;
    ScaledCrossProduct( v2-v1, v3-v1, temp );
    return temp;
}

inline Vector Transform( const Vector &p, const Vector &q, const Vector &r, const Vector &v )
{
    return Vector( p.i*v.i+q.i*v.j+r.i*v.k,
                    p.j*v.i+q.j*v.j+r.j*v.k,
                    p.k*v.i+q.k*v.j+r.k*v.k );
}
inline Vector CrossProduct( const Vector &v1, const Vector &v2 )
{
    Vector result;
    result.i = v1.j*v2.k-v1.k*v2.j;
    result.j = v1.k*v2.i-v1.i*v2.k;
    result.k = v1.i*v2.j-v1.j*v2.i;
    return result;
}

inline void CrossProduct( const Vector &a, const Vector &b, Vector &RES )
{
    RES = a.Cross( b );
}

void Yaw( double rad, Vector &p, Vector &q, Vector &r );
void Pitch( double rad, Vector &p, Vector &q, Vector &r );
void Roll( double rad, Vector &p, Vector &q, Vector &r );
void ResetVectors( Vector &p, Vector &q, Vector &r );
void MakeRVector( Vector &p, Vector &q, Vector &r );
void Orthogonize( Vector &p, Vector &q, Vector &r );
Vector MakeNonColinearVector( const Vector &p );

template<> class vectorizable_traits<Vector> : public struct_vectorizable_traits<Vector, double, 3> {};



#endif

