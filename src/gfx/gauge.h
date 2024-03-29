#include "sprite.h"
class Cockpit;
class Gauge : public VSSprite
{
public:
    enum DIRECTION {GAUGE_UP, GAUGE_DOWN, GAUGE_RIGHT, GAUGE_LEFT, GAUGE_TIME};
private:  enum DIRECTION dir;
public: Gauge( const char *file, DIRECTION up );
    void Draw( double percentage );
    void SetSize( double x, double y );
    void GetSize( double &x, double &y );
};

