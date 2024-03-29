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
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "gfxlib_struct.h"

class SphereMesh;
class Texture;
class StarVlist;
class Background
{
    bool        Enabled;
    bool        degamma;
    GFXColor    color;
    std::unique_ptr<StarVlist>  stars;
    std::shared_ptr<Texture>    up;
    std::shared_ptr<Texture>    left;
    std::shared_ptr<Texture>    front;
    std::shared_ptr<Texture>    right;
    std::shared_ptr<Texture>    back;
    std::shared_ptr<Texture>    down;
    std::unique_ptr<SphereMesh> SphereBackground;
public: 
    Background(int numstars, float spread, const std::string &starfilename, const GFXColor &color, bool degamma);
    void EnableBG( bool );
    void Draw();
    struct BackgroundClone
    {
        Texture *backups[7];
        void     FreeClone();
    };
    BackgroundClone Cache();
};

#endif

