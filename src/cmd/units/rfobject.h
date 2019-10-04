#ifndef RFOBJECT_H
#define RFOBJECT_H

#include "drawable.h"
#include "collidable.h"

/**
 * An Object is any physical object that can be drawn, may collide with something
 * and may be physically affected by forces.
 * Objects are also assumed to have various damage and explode when they are dead.
 */
class RFObject: public Drawable, public Collidable
{
public:
    RFObject();

    ///Updates physics given unit space transformations and if this is the last physics frame in the current gfx frame
        virtual void UpdatePhysics2( const Transformation &trans,
                                     const Transformation &old_physical_state,
                                     const Vector &accel,
                                     float difficulty,
                                     const Matrix &transmat,
                                     const Vector &CumulativeVelocity,
                                     bool ResolveLast,
                                     UnitCollection *uc = NULL );
    ///Thrusts by ammt and clamps accordingly (afterburn or not)
        void Thrust( const Vector &amt, bool afterburn = false );
    ///Resolves forces of given unit on a physics frame
        Vector ResolveForces( const Transformation&, const Matrix& );
    //these functions play the damage sounds
        virtual void ArmorDamageSound( const Vector &pnt );
        virtual void HullDamageSound( const Vector &pnt );
    ///applies damage from the given pnt to the shield, and returns % damage applied and applies lighitn
        float DealDamageToShield( const Vector &pnt, float &Damage );
};

#endif // RFOBJECT_H
