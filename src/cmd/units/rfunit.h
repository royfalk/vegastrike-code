#ifndef RFUNIT_H
#define RFUNIT_H



/**
 * A Unit is an Object. It can be drawn, may collide with something,
 * be physically affected by forces, take damage and be destroyed.
 * Units are also a representation of ship or base.
 * Therefore, they may have any number of subunit weapons, which are themselves units and belong to a faction.
 * The aistate indicates how the unit will behave in the upcoming phys frame
 */
class RFUnit : RFObject {
    bool subunit = false;
    int faction = 0;
public:
    RFUnit();

    // TODO: think about converting meshes to pointers
    RFUnit(std::vector<Mesh> meshes, bool Subunit, int faction);

    RFUnit(const std:string filename, bool SubUnit, int faction, std::string customizedUnit = "",
           Flightgroup *flightgroup = nullptr, int fg_subnumber = 0, std::string *netxml = nullptr );

    ///The name (type) of this unit shouldn't be public
        virtual void Cloak( bool cloak );

    bool TransferUnitToSystem( unsigned int whichJumpQueue, StarSystem*&previouslyActiveStarSystem, bool DoSightAndSound );
///Begin and continue explosion
    bool Explode( bool draw, float timeit );

    bool UpgradeSubUnits( const Unit *up, int subunitoffset, bool touchme, bool downgrade, int &numave, double &percentage );
    double Upgrade( const std::string &file, int mountoffset, int subunitoffset, bool force, bool loop_through_mounts );

    ///Holds temporary values for inter-function XML communication Saves deprecated restr info
        Matrix WarpMatrix( const Matrix &ctm ) const;
};

#endif // RFUNIT_H
