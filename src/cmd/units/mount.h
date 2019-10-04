#ifndef MOUNT_H
#define MOUNT_H

//#include "gfx/quaternion.h"
//#include "gfx/vec.h"
//#include "weapons/beam.h"

#include "vegastrike.h"
#include "vs_globals.h"

#include <string>
#include <set>
#include <map>
#include "cmd/unit_armorshield.h"
#include "gfx/matrix.h"
#include "gfx/quaternion.h"
#include "gfxlib_struct.h"
#include "weapon_xml.h"
#include "xml_support.h"
#include "container.h"
#include "collection.h"
#include "script/flightgroup.h"
#include "faction_generic.h"
#include "star_system_generic.h"
#include "networking/const.h"
#include "networking/lowlevel/vsnet_clientstate.h"
#include "gfx/cockpit_generic.h"
#include "vsfilesystem.h"
#include "collide_map.h"
#include "SharedPool.h"

class Beam;

class Mount {
protected:
//Where is it
    Vector     pos;
    Quaternion orient;
    double last_sound_refire_time;
    void ReplaceSound();
public:
//Serial used to store missiles serial id before they are really created
    ObjSerial serial;
//for guns!
    float     xyscale;
    float     zscale;
    float ComputeAnimatedFrame( Mesh *gun );
//pass inunit so it can update gunspeed
    void ReplaceMounts( Unit *unit, const Mount *other );
    double Percentage( const Mount *newammo ) const;
//Gotta look at that, if we can make Beam a string in AcctUnit and a Beam elsewhere
    union REF
    {
        //only beams are actually coming out of the gun at all times...bolts, balls, etc aren't
        Beam *gun;
        //Other weapons must track refire times
        float refire;
    }
    ref;
//the size that this mount can hold. May be any bitwise combination of weapon_info::MOUNT_SIZE
    unsigned int size;                                           //short fix
//-1 is infinite
    int ammo;                                            //short
    int volume;                                          //-1 is infinite //short fix
//The data behind this weapon. May be accordingly damaged as time goes on
    enum MOUNTSTATUS {REQUESTED, ACCEPTED, PROCESSED, UNFIRED, FIRED}
    processed;
//Status of the selection of this weapon. Does it fire when we hit space
    enum STATUS {ACTIVE, INACTIVE, DESTROYED, UNCHOSEN}
    status;
    bool  bank;
//bank implies whether the weapon is linked with the next mount (i.e. only one firing at a time)
    const weapon_info *type;
    float functionality;
    float maxfunctionality;
    int   sound;
//The sound this mount makes when fired
    float time_to_lock;
    Mount();
//short fix
    Mount( const std::string &name,
           int ammo,
           int volume,
           float xyscale,
           float zscale,
           float functionality,
           float maxfunctionality,
           bool banked );

    void Activate( bool Missile );
    void DeActive( bool Missile );
//Sets this gun's position on the mesh
    void SetMountPosition( const Vector& );
    void SetMountOrientation( const Quaternion& );
//Gets the mount's position and transform
    const Vector& GetMountLocation() const
    {
        return pos;
    }
    const Quaternion& GetMountOrientation() const
    {
        return orient;
    }
//Turns off a firing beam (upon key release for example)
    void UnFire();
/**
 *  Fires a beam when the firing unit is at the Cumulative location/transformation
 * owner (won't crash into)  as owner and target as missile target. bool Missile indicates if it is a missile
 * should it fire
 */
//Uses Sound Forcefeedback and other stuff
    void PhysicsAlignedUnfire();
    bool PhysicsAlignedFire( Unit*caller,
                             const Transformation &Cumulative,
                             const Matrix &mat,
                             const Vector &Velocity,
                             void *owner,
                             Unit*target,
                             signed char autotrack,
                             float trackingcone,
                             CollideMap::iterator hint[] );
    bool NextMountCloser( Mount *nextmount, Unit* );
    bool Fire( Unit *firer, void *owner, bool Missile = false, bool collide_only_with_target = false );
    bool IsEmpty() const
    {
        return !(status == ACTIVE || status == INACTIVE);
    }
};

#endif // MOUNT_H
