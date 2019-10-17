#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "key_mutable_set.h"
#include "vegastrike.h"
#include "gfx/vec.h"

#include <limits>
#include <vector>

class Unit;

class Collidable
{
public:
    Vector position;
    float   radius; //radius == 0: to-be-deleted, radius <0 bolt (radius == speed in phys frame), radius >0 unit

    union CollideRef
    {
        Unit *unit;
        unsigned int bolt_index;
    }
    ref;
    Vector GetPosition() const
    {
        return position;
    }
    void SetPosition( const Vector &bpos )
    {
        //in case we want to drop in an xtra radius parameter when we get performance testing
        this->position = bpos;

        if ( ISNAN( getKey() ) )
            position = Vector( 0, 0, 0 );      //hack for now
    }
    Collidable&operator*()
    {
        return *this;
    }
    Collidable* operator->()
    {
        return this;
    }

    double getKey() const
    {
        return position.i;
    }
    bool operator<( const Collidable &other ) const
    {
        return getKey() < other.getKey();
    }
    Collidable& get()
    {
        return *this;
    }
    Collidable() : radius(
            std::numeric_limits< float >::quiet_NaN()
                         ) {}
    Collidable( Unit *un );
    Collidable( unsigned int bolt_index, float speed, const Vector &p )
    {
        ref.bolt_index = bolt_index;
        radius = -speed*SIMULATION_ATOM;
        if (

            ISNAN( radius )

            || radius >= -FLT_MIN) radius = -FLT_MIN*2;
        this->SetPosition( p );
    }
};

#endif // COLLIDABLE_H
