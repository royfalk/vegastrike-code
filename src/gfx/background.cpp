/*
 * Vega Strike
 * Copyright (C) 2001-2002 Daniel Horn
 *
 * http://vegastrike.sourceforge.net/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "vegastrike.h"
#include "camera.h"
#include "aux_texture.h"
#include "star.h"
#include "background.h"
#include "gfxlib.h"
#include "aux_texture.h"
#include "sphere.h"
#include "vs_globals.h"
#include "../gldrv/gl_globals.h"
#include "config_xml.h"
#include "universe_util.h"
#include <float.h>
#include <typeinfo>

constexpr double size = 100;
Background::Background(int numstars, float spread, const std::string &filename, const GFXColor &color_, bool degamma_ )
    : Enabled( true )
    , degamma( degamma_ )
    , color( color_ )
    , stars( nullptr) //std::make_unique<StarVlist>(nullptr) )
{
    string temp;
    static string starspritetextures = vs_config->getVariable( "graphics", "far_stars_sprite_texture", "" );
    static float  starspritesize     =
        XMLSupport::parse_float( vs_config->getVariable( "graphics", "far_stars_sprite_size", "2" ) );
    if (starspritetextures.length() == 0) {
        stars =
            std::make_unique<PointStarVlist>( numstars, 200 /*spread*/,
            //new PointStarVlist( numstars, 200 /*spread*/,
                                XMLSupport::parse_bool( vs_config->getVariable( "graphics",
                                                                                "use_star_coords",
                                                                                "true" ) ) ? filename : "" );
    } else {
        stars =
            std::make_unique<SpriteStarVlist>(numstars, 200 /*spread*/,
            // new SpriteStarVlist( numstars, 200 /*spread*/,
                                 XMLSupport::parse_bool( vs_config->getVariable( "graphics",
                                                                                 "use_star_coords",
                                                                                 "true" ) ) ? filename : "", starspritetextures,
                                 starspritesize );
    }
    up = left = down = front = right = back = nullptr;

    SphereBackground = nullptr;
}

void Background::EnableBG( bool tf )
{
    Enabled = tf;
}


Background::BackgroundClone Background::Cache()
{
    BackgroundClone ret;
    return ret;
}

void Background::BackgroundClone::FreeClone() {}

void Background::Draw()
{
    GFXClear( Enabled ? GFXFALSE : GFXTRUE );
    if (Enabled) {
        if (degamma)
            GFXBlendMode( SRCCOLOR, ZERO );
        else
            GFXBlendMode( ONE, ZERO );
        GFXDisable( LIGHTING );
        GFXDisable( DEPTHWRITE );
        GFXDisable( DEPTHTEST );
        GFXEnable( TEXTURE0 );
        GFXDisable( TEXTURE1 );
        GFXColorf(color);
        if (SphereBackground) {
            SphereBackground->DrawNow( FLT_MAX, true );
            //Mesh::ProcessUndrawnMeshes();//background must be processed...dumb but necessary--otherwise might collide with other mehses
        } else {
            GFXCenterCamera( true );
            GFXLoadMatrixModel( identity_matrix );
            //GFXLoadIdentity(MODEL);
            //GFXTranslate (MODEL,_Universe->AccessCamera()->GetPosition());

            //GFXTextureWrap(0,GFXCLAMPTEXTURE);
            //glMatrixMode(GL_MODELVIEW);

            /***********************?????????
            *                        //Matrix oldproject;
            *                        //GFXGetMatrix(VIEW, oldproject);
            *                        //glPushMatrix();
            *                        //gluPerspective (90,1.33,0.01,1500); //set perspective to 78 degree FOV
            ********************************/
            //_Universe->AccessCamera()->UpdateGLCenter();

            static struct skybox_rendering_record
            {
                std::shared_ptr<Texture> tex;
                float    vertices[4][3];              //will be *= size
                char     tcoord[4][4];             //S-T-S-T: 0 >= min, 1 => max
            }
            skybox_rendering_sequence[6] = {
                //For rendering with a single cube map as texture

                {                 //up
                    nullptr,
                    {
                        {-1, +1, +1}, {-1, +1, -1}, {+1, +1, -1}, {+1, +1, +1}
                    },
                    {
                        {-1, +2, +1, 0}, {-1, +2, -1, 0}, {+1, +2, -1, 0}, {+1, +2, +1, 0}
                    },
                },
                {                 //left
                    nullptr,
                    {
                        {-1,+1, -1}, {-1, +1, +1}, {-1, -1, +1}, {-1, -1, -1}
                    },
                    {
                        {-2, +1, -1, 0}, {-2, +1, +1, 0}, {-2, -1, +1, 0}, {-2, -1, -1, 0}
                    },
                },
                {                 //front
                    nullptr,
                    {
                        {-1, +1, +1}, {+1, +1, +1}, {+1, -1, +1}, {-1, -1, +1}
                    },
                    {
                        {-1, +1, +2, 0}, {+1, +1, +2, 0}, {+1, -1, +2, 0}, {-1, -1, +2, 0}
                    },
                },
                {                 //right
                    nullptr,
                    {
                        {+1, +1, +1}, {+1, +1, -1}, {+1, -1, -1}, {+1, -1, +1}
                    },
                    {
                        {+2, +1, +1, 0}, {+2, +1, -1, 0}, {+2, -1, -1, 0}, {+2, -1, +1, 0}
                    },
                },
                {                 //back
                    nullptr,
                    {
                        {+1, +1, -1}, {-1, +1, -1}, {-1, -1, -1}, {+1, -1, -1}
                    },
                    {
                        {+1, +1, -2, 0}, {-1, +1, -2, 0}, {-1, -1, -2, 0}, {+1, -1, -2, 0}
                    },
                },
                {                 //down
                    nullptr,
                    {
                        {-1, -1, +1}, {+1, -1, +1}, {+1, -1, -1}, {-1, -1, -1}
                    },
                    {
                        {-1, -2, +1, 0}, {+1, -2, +1, 0}, {+1, -2, -1, 0}, {-1, -2, -1, 0}
                    },
                }


            };
            skybox_rendering_sequence[0].tex = up;
            skybox_rendering_sequence[1].tex = left;
            skybox_rendering_sequence[2].tex = front;
            skybox_rendering_sequence[3].tex = right;
            skybox_rendering_sequence[4].tex = back;
            skybox_rendering_sequence[5].tex = down;
            for (size_t skr = 0; skr < sizeof (skybox_rendering_sequence)/sizeof (skybox_rendering_sequence[0]); skr++) {
                Texture* tex = skybox_rendering_sequence[skr].tex.get();

                if (tex == nullptr)
                    tex = _Universe->getLightMap();
                const int    numpasses = 1;
                static const float edge_fixup =
                    XMLSupport::parse_float( vs_config->getVariable( "graphics", "background_edge_fixup", "0" ) );
                const float  ms      = 0.f, Ms = 1.f-edge_fixup/tex->boundSizeX;
                const float  mt      = 0.f, Mt = 1.f-edge_fixup/tex->boundSizeY;
                const float  _stca[] = {-1.f, -Ms, ms, Ms, +1.f}, _ttca[] = {-1.f, -Mt, mt, Mt, +1.f};
                const float *stca    = _stca+2, *ttca = _ttca+2;

                GFXColorf(color);

                _Universe->activateLightMap( 0 );
                GFXToggleTexture( true, 0, CUBEMAP );

                for (int pass = 0; pass < numpasses; pass++)
                    if ( !tex || tex->SetupPass( pass, 0, ONE, ZERO ) ) {
                        if (tex)
                            tex->MakeActive( 0, pass );
                        GFXTextureAddressMode( CLAMP );
                        GFXTextureEnv( 0, GFXMODULATETEXTURE );
                        GFXTextureCoordGenMode( 0, NO_GEN, NULL, NULL );

#define X( i ) skybox_rendering_sequence[skr].vertices[i][0]*size
#define Y( i ) skybox_rendering_sequence[skr].vertices[i][1]*size
#define Z( i ) skybox_rendering_sequence[skr].vertices[i][2]*size
#define S( i ) stca[size_t(skybox_rendering_sequence[skr].tcoord[i][0])]
#define T( i ) ttca[size_t(skybox_rendering_sequence[skr].tcoord[i][1])]
#define U( i ) stca[size_t(skybox_rendering_sequence[skr].tcoord[i][2])]
#define V( i ) ttca[size_t(skybox_rendering_sequence[skr].tcoord[i][3])]

                        const float verts[4 * (3 + 3)] = { 
                            X(0), Y(0), Z(0), S(0), T(0), U(0),
                            X(1), Y(1), Z(1), S(1), T(1), U(1),
                            X(2), Y(2), Z(2), S(2), T(2), U(2),
                            X(3), Y(3), Z(3), S(3), T(3), U(3),
                        };
                        GFXDraw( GFXQUAD, verts, 4, 3, 0, 3 );


#undef X
#undef Y
#undef Z
#undef S
#undef T
#undef U
#undef V
                    }

                GFXToggleTexture( false, 0, CUBEMAP );

            }

            GFXActiveTexture( 0 );
            GFXTextureAddressMode( WRAP );
            GFXCenterCamera( false );
        }
    }
    //GFXLoadIdentity(MODEL);
    //GFXTranslate (MODEL,_Universe->AccessCamera()->GetPosition());
    GFXCenterCamera( true );
//GFXEnable(DEPTHWRITE);
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXColor( 1, 1, 1, 1 );
    GFXDisable( TEXTURE1 );
    GFXDisable( DEPTHWRITE );
    GFXBlendMode( ONE, ONE );
    static float background_velocity_scale =
        XMLSupport::parse_float( vs_config->getVariable( "graphics", "background_star_streak_velocity_scale", "0" ) );
    stars->DrawAll( Vector( 0, 0, 0 ), _Universe->AccessCamera()->GetVelocity().Scale(
                        background_velocity_scale ), _Universe->AccessCamera()->GetAngularVelocity(), true, true );
    GFXBlendMode( ONE, ZERO );
    GFXEnable( DEPTHTEST );
    GFXEnable( DEPTHWRITE );
    GFXCenterCamera( false );
    //_Universe->AccessCamera()->UpdateGFX(false);
}

