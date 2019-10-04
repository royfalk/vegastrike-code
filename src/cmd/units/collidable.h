#ifndef COLLIDABLE_H
#define COLLIDABLE_H


class Collidable
{
public:
    Collidable();

    ///Updates the collide Queue with any possible change in sectors
    ///Queries if this unit is within a given frustum
        bool queryFrustum( double frustum[6][4] ) const;

    ///Queries the bounding sphere with a duo of mouse coordinates that project
    ///to the center of a ship and compare with a sphere...pretty fast*/
        bool querySphereClickList( int, int, float err, Camera *activeCam ) const;
    ///returns true if jump possible even if not taken
};

#endif // COLLIDABLE_H
