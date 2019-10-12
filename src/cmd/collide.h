#ifndef __COLLIDE_FUNCS_H
#define __COLLIDE_FUNCS_H

#include "gfx/vec.h"
#include "linecollide.h"
#include "star_system_generic.h"

bool TableLocationChanged( const Vector &Mini, const Vector &minz );
bool TableLocationChanged( const LineCollide &lc, const Vector &minx, const Vector &maxx );
void KillCollideTable( LineCollide *lc, StarSystem *ss );
bool EradicateCollideTable( LineCollide *lc, StarSystem *ss );
void AddCollideQueue( LineCollide &tmp, StarSystem *ss );
bool lcwithin( const LineCollide &lc, const LineCollide &tmp );

#endif

