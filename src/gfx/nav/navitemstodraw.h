//This draws the mouse cursor
//**********************************
void NavigationSystem::DrawCursor( double x, double y, double wid, double hei, const GFXColor &col )
{
    double sizex, sizey;
    static bool modern_nav_cursor =
        XMLSupport::parse_bool( vs_config->getVariable( "graphics", "nav", "modern_mouse_cursor", "true" ) );
    if (modern_nav_cursor) {
        static string   mouse_cursor_sprite = vs_config->getVariable( "graphics", "nav", "mouse_cursor_sprite", "mouse.spr" );
        static VSSprite MouseVSSprite( mouse_cursor_sprite.c_str(), BILINEAR, GFXTRUE );
        GFXBlendMode( SRCALPHA, INVSRCALPHA );
        GFXColorf( GUI_OPAQUE_WHITE() );

        //Draw the cursor sprite.
        GFXEnable( TEXTURE0 );
        GFXDisable( DEPTHTEST );
        GFXDisable( TEXTURE1 );
        MouseVSSprite.GetSize( sizex, sizey );
        MouseVSSprite.SetPosition( x+sizex/2, y+sizey/2 );
        MouseVSSprite.Draw();
    } else {
        GFXColorf( col );
        GFXDisable( TEXTURE0 );
        GFXDisable( LIGHTING );
        GFXBlendMode( SRCALPHA, INVSRCALPHA );

        const double verts[8 * 3] = {
            x,          y,          0,
            x,          y-hei,      0,
            x,          y,          0,
            x+wid,      (y-0.75*hei), 0,
            x,          y-hei,      0,
            (x+0.35*wid), (y-0.6*hei),  0,
            (x+0.35*wid), (y-0.6*hei),  0,
            x+wid,      static_cast<double>(y-0.75*hei), 0,
        };
        GFXDraw( GFXLINE, verts, 24, 8 );

        GFXEnable( TEXTURE0 );
    }
}
//**********************************

//This draws the grid over the nav screen area
//**********************************
void NavigationSystem::DrawGrid( double &x1, double &x2, double &y1, double &y2, const GFXColor &col )
{
    static bool draw_grid = XMLSupport::parse_bool( vs_config->getVariable( "graphics", "hud", "draw_nav_grid", "true" ) );
    if (!draw_grid)
        return;
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    double deltax = x2-x1;
    deltax = deltax/10;
    double deltay = y2-y1;
    deltay = deltay/10;

    static VertexBuilder<> verts;
    verts.clear();
    for (int i = 1; i < 10; i++) {
        verts.insert(x1+i*deltax, y1, 0);
        verts.insert(x1+i*deltax, y2, 0);
    }
    for (int i = 1; i < 10; i++) {
        verts.insert(x1, y1+i*deltay, 0);
        verts.insert(x2, y1+i*deltay, 0);
    }
    GFXDraw( GFXLINE, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a circle over the screen
//**********************************
void NavigationSystem::DrawCircle( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    // 20 segments
    static VertexBuilder<> verts;
    verts.clear();
    for ( double i = 0; i < 2*M_PI + M_PI/10; i += M_PI/10 ) {
        verts.insert(
            x+0.5*size*cos( i ),
            y+0.5*size*sin( i ),
            0
        );
    }
    GFXDraw( GFXLINESTRIP, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a half circle, centered at the top 1/4 center
//**********************************
void NavigationSystem::DrawHalfCircleTop( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    // 10 segments
    static VertexBuilder<> verts;
    verts.clear();
    for (double i = 0; i < M_PI + M_PI/10; i += M_PI/10 ) {
        verts.insert(
            x+0.5*size*cos( i ),
            y+0.5*size*sin( i )-0.25*size,
            0
        );
    }
    GFXDraw( GFXLINESTRIP, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a half circle, centered at the bottom 1/4 center
//**********************************
void NavigationSystem::DrawHalfCircleBottom( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    // 10 segments
    static VertexBuilder<> verts;
    verts.clear();
    for (double i = M_PI; i < 2*M_PI + M_PI/10; i += M_PI/10 ) {
        verts.insert(
            x+0.5*size*cos( i ),
            y+0.5*size*sin( i )+0.25*size,
            0
        );
    }
    GFXDraw( GFXLINESTRIP, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a planet icon. circle + lightning thingy
//**********************************
void NavigationSystem::DrawPlanet( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    static VertexBuilder<> verts;
    verts.clear();
    for ( double i = 0; i < 2*M_PI; i += M_PI/10 ) {
        verts.insert(
            x+0.5*size*cos( i ),
            y+0.5*size*sin( i ),
            0
        );
        verts.insert(
            x+0.5*size*cos( i+M_PI/10 ),
            y+0.5*size*sin( i+M_PI/10 ),
            0
        );
    }
    verts.insert(x-0.5*size     , y             , 0 );
    verts.insert(x              , y+0.2*size    , 0 );
    verts.insert(x              , y+0.2*size    , 0 );
    verts.insert(x              , y-0.2*size    , 0 );
    verts.insert(x              , y-0.2*size    , 0 );
    verts.insert(x+0.5*size     , y             , 0 );
    GFXDraw( GFXLINE, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a station icon. 3x3 grid
//**********************************
void NavigationSystem::DrawStation( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    double segment = size/3;
    static VertexBuilder<> verts;
    verts.clear();
    for (int i = 0; i < 4; i++) {
        verts.insert(
            x-0.5*size,
            y-0.5*size+i*segment,
            0
        );
        verts.insert(
            x+0.5*size,
            y-0.5*size+i*segment,
            0
        );
    }
    for (int i = 0; i < 4; i++) {
        verts.insert(
            x-0.5*size+i*segment,
            y-0.5*size,
            0
        );
        verts.insert(
            x-0.5*size+i*segment,
            y+0.5*size,
            0
        );
    }
    GFXDraw( GFXLINE, verts );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a jump node icon
//**********************************
void NavigationSystem::DrawJump( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    static VertexBuilder<> verts;
    verts.clear();
    for ( double i = 0; i < 2*M_PI; i += M_PI/10 ) {
        verts.insert(
            x+0.5*size*cos( i ),
            y+0.5*size*sin( i ),
            0
        );
        verts.insert(
            x+0.5*size*cos( i+M_PI/10 ),
            y+0.5*size*sin( i+M_PI/10 ),
            0
        );
    }
    verts.insert( x             , y+0.5*size    , 0 );
    verts.insert( x+0.125*size  , y+0.125*size  , 0 );
    verts.insert( x             , y+0.5*size    , 0 );
    verts.insert( x-0.125*size  , y+0.125*size  , 0 );
    verts.insert( x             , y-0.5*size    , 0 );
    verts.insert( x+0.125*size  , y-0.125*size  , 0 );
    verts.insert( x             , y-0.5*size    , 0 );
    verts.insert( x-0.125*size  , y-0.125*size  , 0 );
    verts.insert( x-0.5*size    , y             , 0 );
    verts.insert( x-0.125*size  , y+0.125*size  , 0 );
    verts.insert( x-0.5*size    , y             , 0 );
    verts.insert( x-0.125*size  , y-0.125*size  , 0 );
    verts.insert( x+0.5*size    , y             , 0 );
    verts.insert( x+0.125*size  , y+0.125*size  , 0 );
    verts.insert( x+0.5*size    , y             , 0 );
    verts.insert( x+0.125*size  , y-0.125*size  , 0 );
    GFXDraw( GFXLINE, verts );

    GFXEnable( TEXTURE0 );
}

//**********************************

//This will draw a missile icon
//**********************************
void NavigationSystem::DrawMissile( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    const float verts[12 * 3] = {
        x-0.5f*size,  y-0.125f*size, 0,
        x,           y+0.375f*size, 0,
        x+0.5f*size,  y-0.125f*size, 0,
        x,           y+0.375f*size, 0,
        x-0.25f*size, y-0.125f*size, 0,
        x-0.25f*size, y+0.125f*size, 0,
        x+0.25f*size, y-0.125f*size, 0,
        x+0.25f*size, y+0.125f*size, 0,
        x-0.25f*size, y+0.125f*size, 0,
        x,           y-0.125f*size, 0,
        x+0.25f*size, y+0.125f*size, 0,
        x,           y-0.125f*size, 0,
    };
    GFXDraw( GFXLINE, verts, 12 );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw a square set of corners
//**********************************
void NavigationSystem::DrawTargetCorners( double x, double y, double size, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    const float verts[16 * 3] = {
        x-0.5f*size, y+0.5f*size, 0,
        x-0.3f*size, y+0.5f*size, 0,
        x-0.5f*size, y+0.5f*size, 0,
        x-0.5f*size, y+0.3f*size, 0,
        x+0.5f*size, y+0.5f*size, 0,
        x+0.3f*size, y+0.5f*size, 0,
        x+0.5f*size, y+0.5f*size, 0,
        x+0.5f*size, y+0.3f*size, 0,
        x-0.5f*size, y-0.5f*size, 0,
        x-0.3f*size, y-0.5f*size, 0,
        x-0.5f*size, y-0.5f*size, 0,
        x-0.5f*size, y-0.3f*size, 0,
        x+0.5f*size, y-0.5f*size, 0,
        x+0.3f*size, y-0.5f*size, 0,
        x+0.5f*size, y-0.5f*size, 0,
        x+0.5f*size, y-0.3f*size, 0,
    };
    GFXDraw( GFXLINE, verts, 16 );

    GFXEnable( TEXTURE0 );
}
//**********************************

//This will draw an oriented circle
//**********************************
void NavigationSystem::DrawNavCircle( double x, double y, double size, double rot_x, double rot_y, const GFXColor &col )
{
    GFXColorf( col );
    GFXDisable( TEXTURE0 );
    GFXDisable( LIGHTING );
    GFXBlendMode( SRCALPHA, INVSRCALPHA );

    const int circles = 4;
    const int segments = 20;
    const int segments2 = 12; 
    const int vnum = 2 * (circles * segments + segments2);
    static VertexBuilder<float, 3, 0, 4> verts;
    verts.clear();
    verts.reserve(vnum);
    for ( double i = 0; i < 2*M_PI; i += (2*M_PI/segments) ) {
        GFXColor ci( col.r, col.g, col.b * fabs(sin(i / 2.0)), col.a );
        Vector pos1( ( 0.6*size*cos( i ) ), ( 0.6*size*sin( i ) ), 0 );
        Vector pos2( ( 0.6*size*cos( i+(2*M_PI/segments) ) ), ( 0.6*size*sin( i+(6.28/segments) ) ), 0 );

        pos1 = dxyz( pos1, 0, 0, rot_y );
        pos1 = dxyz( pos1, rot_x, 0, 0 );
        pos2 = dxyz( pos2, 0, 0, rot_y );
        pos2 = dxyz( pos2, rot_x, 0, 0 );

        double standard_unit = 0.25*1.2*size;
        double zdistance1    = ( (1.2*size)-pos1.k );
        double zdistance2    = ( (1.2*size)-pos2.k );
        double zscale1 = standard_unit/zdistance1;
        double zscale2 = standard_unit/zdistance2;
        pos1 *= (zscale1*5.0);
        pos2 *= (zscale2*5.0);

        for (int j = circles; j > 0; j--) {
            pos1 *= ( double(j)/double(circles) );
            pos2 *= ( double(j)/double(circles) );

            Vector pos1t( (x+pos1.i), ( y+(pos1.j) ), 0 );
            Vector pos2t( (x+pos2.i), ( y+(pos2.j) ), 0 );

            verts.insert( GFXColorVertex(pos1t, ci) );
            verts.insert( GFXColorVertex(pos2t, ci) );
        }
    }
    for ( double i = 0; i < 2*M_PI; i += (2*M_PI/segments2) ) {
        GFXColor ci( col.r, col.g, col.b * fabs(sin(i / 2.0)), col.a );
        Vector pos1( ( 0.6*size*cos( i )/double(circles*2) ), ( 0.6*size*sin( i )/double(circles*2) ), 0 );
        Vector pos2( ( 0.6*size*cos( i ) ), ( 0.6*size*sin( i ) ), 0 );

        if ( (fabs( i-1.57 ) < 0.01) || (fabs( i-3.14 ) < 0.01) || (fabs( i-4.71 ) < 0.01) || (i < 0.01) )
            pos2 *= 1.1;
        pos1 = dxyz( pos1, 0, 0, rot_y );
        pos1 = dxyz( pos1, rot_x, 0, 0 );
        pos2 = dxyz( pos2, 0, 0, rot_y );
        pos2 = dxyz( pos2, rot_x, 0, 0 );

        double standard_unit = 0.25*1.2*size;
        double zdistance1    = ( (1.2*size)-pos1.k );
        double zdistance2    = ( (1.2*size)-pos2.k );
        double zscale1 = standard_unit/zdistance1;
        double zscale2 = standard_unit/zdistance2;
        pos1 *= (zscale1*5.0);
        pos2 *= (zscale2*5.0);

        pos1.i += x;
        pos1.j += y;
        pos2.i += x;
        pos2.j += y;

        // pos, col
        verts.insert( GFXColorVertex(pos1, ci) );
        verts.insert( GFXColorVertex(pos2, ci) );
    }
    GFXDraw( GFXLINE, verts);

    GFXEnable( TEXTURE0 );
}
//**********************************

