#include "gauge.h"

Gauge::Gauge( const char *file, DIRECTION up ) : VSSprite( file )
{
    dir = up;
    double sx, sy;
    VSSprite::GetSize( sx, sy );
    SetSize( -sx, -sy );
}

void Gauge::SetSize( double x, double y )
{
    if (dir == GAUGE_UP)
        y = -y;
    else if (dir == GAUGE_LEFT)
        x = -x;
    VSSprite::SetSize( x, y );
}

void Gauge::GetSize( double &x, double &y )
{
    VSSprite::GetSize( x, y );
    if (dir == GAUGE_UP)
        y = -y;
    else if (dir == GAUGE_LEFT)
        x = -x;
}

void Gauge::Draw( double percentage )
{
    if (percentage < 0) percentage = 0;

    else if (percentage > 1)
        percentage = 1;
    double sx, sy, px, py;
    VSSprite::GetSize( sx, sy );
    VSSprite::GetPosition( px, py );
    switch (dir)
    {
    case GAUGE_RIGHT:
    case GAUGE_LEFT:
        SetST( (1-percentage), 0 );
        VSSprite::SetSize( sx*percentage, sy );
        VSSprite::SetPosition( px+sx*(1-percentage)*.5, py );
        break;
    case GAUGE_DOWN:
    case GAUGE_UP:
        SetST( 0, (1-percentage) );
        VSSprite::SetSize( sx, sy*percentage );
        VSSprite::SetPosition( px, py+sy*(1-percentage)*.5 );
        break;
    case GAUGE_TIME:
        VSSprite::SetTime( percentage );
        break;
    }
    VSSprite::Draw();
    VSSprite::SetSize( sx, sy );
    VSSprite::SetPosition( px, py );
}

