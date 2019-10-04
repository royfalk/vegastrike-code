#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <vector>
#include "gfx/mesh.h"

class Drawable {
    // TODO: think about converting meshes to pointers
    std::vector<Mesh> meshes;

    double sparkle_accum;
std::auto_ptr< HaloSystem >phalos;
///Process all meshes to be deleted
///Split this mesh with into 2^level submeshes at arbitrary planes
void Split( int level );
void FixGauges();
///Sets the camera to be within this unit.
void UpdateHudMatrix( int whichcam );
///What's the HudImage of this unit
VSSprite * getHudImage() const;
///Draws this unit with the transformation and matrix (should be equiv) separately
virtual void Draw( const Transformation &quat, const Matrix &m );
virtual void Draw( const Transformation &quat );
virtual void Draw();
virtual void DrawNow( const Matrix &m, float lod = 1000000000 );
virtual void DrawNow();
///Deprecated
void addHalo( const char *filename,
              const Matrix &trans,
              const Vector &size,
              const GFXColor &col,
              std::string halo_type,
              float halo_speed );

virtual void applyTechniqueOverrides(const std::map<std::string, std::string> &overrides);
public:
    Drawable();
    unsigned int meshesCount() { return meshes.size(); }

    // No idea what this does. Perhaps
    ///fils in corner_min,corner_max and radial_size
    UnitImages<void>& GetImageInformation();
};

#endif // DRAWABLE_H
