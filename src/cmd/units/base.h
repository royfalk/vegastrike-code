#ifndef BASE_H
#define BASE_H

/**
 * @brief A representation of a ship, base or planet where you can dock,
 * trade and upgrade your ship.
 */
class Base {
public:
    Base();
    bool requestClearance(RFUnit *requestingUnit);

    ///Loads a user interface for the user to upgrade his ship
    void UpgradeInterface(Unit *base);

};

#endif // BASE_H
