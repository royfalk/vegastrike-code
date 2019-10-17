#include "collidable.h"
#include "../unit.h"

/**
 * A collidable class is one that inherits from Collidable
 * and represents a physical object that may collide with something
 * And may be physically affected by forces.
 */

class Unit;

Collidable::Collidable( Unit *un )
{
    radius = un->rSize();
    if ( radius <= FLT_MIN || !FINITE( radius ) ) radius = 2*FLT_MIN;
    assert( !un->isSubUnit() );
    this->SetPosition( un->LocalPosition() );
    ref.unit = un;
}
