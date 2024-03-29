#include "faction_generic.h"
#include "unit.h"
#include "pilot.h"
#include "ai/order.h"
#include "universe_util.h"
#include "cmd/unit_util.h"

#include <vector>

Pilot::Pilot( int faction )
{
    static float reaction = XMLSupport::parse_float( vs_config->getVariable( "AI", "Firing", "ReactionTime", ".2" ) );
    static float ran = XMLSupport::parse_float( vs_config->getVariable( "AI", "DefaultRank", ".01" ) );
    this->rank    = ran;

    this->reaction_time = reaction;
    this->faction = faction;
    comm_face     = NULL;
    gender = 0;
}

void Pilot::SetComm( Unit *parent )
{
    this->faction = parent->faction;
    //GET BETTER REACTION TIME AND RANK HERE
    comm_face = FactionUtil::GetRandCommAnimation( faction, parent, gender );
}

float Pilot::adjustSpecificRelationship( Unit *parent, void *aggressor, float factor, int faction )
{
    relationmap::iterator i = effective_relationship.insert( std::pair< const void*, float > ( aggressor, 0 ) ).first;
    if ( faction != FactionUtil::GetNeutralFaction() ) {
        float rel = UnitUtil::getRelationToFaction( parent, faction );         /* What the bloody hell? */
        bool  abovezero = (*i).second+rel < 0;
        if (!abovezero) {
            static float slowrel = XMLSupport::parse_float( vs_config->getVariable( "AI", "SlowDiplomacyForEnemies", ".25" ) );
            factor *= slowrel;
        }
        (*i).second += factor;
        if (rel+factor < 0 && parent->Target() == NULL && parent->aistate)
            parent->aistate->ChooseTarget();
    } else {
        static float lessrel = XMLSupport::parse_float( vs_config->getVariable( "AI", "UnknownRelationEnemy", "-.05" ) );
        bool abovezero = (*i).second < lessrel;
        if (!abovezero) {
            static float slowrel = XMLSupport::parse_float( vs_config->getVariable( "AI", "SlowDiplomacyForEnemies", ".25" ) );
            factor *= slowrel;
        }
        (*i).second += factor;
        if ( (*i).second < lessrel && parent->Target() == NULL && parent->aistate )
            parent->aistate->ChooseTarget();
    }
    return (*i).second;
}
void Pilot::DoHit( Unit *parent, void *aggressor, int faction )
{
    static float hitcost = XMLSupport::parse_float( vs_config->getVariable( "AI", "UnknownRelationHitCost", ".01" ) );
    if (hitcost) {
        adjustSpecificRelationship( parent, aggressor, hitcost, faction );
        int whichCp = _Universe->whichPlayerStarship( parent );
        if (whichCp != -1 && faction != parent->faction) {
            UniverseUtil::adjustRelationModifierInt( whichCp, faction, hitcost*getRank() );
        } else {
            /* Instead use the Aggressor's cockpit? */
            whichCp = _Universe->whichPlayerStarship( (const Unit*) aggressor );
            if (whichCp != -1) {
                Flightgroup *fg = parent->getFlightgroup();
                if (parent->faction != faction)
                    UniverseUtil::adjustRelationModifierInt( whichCp, parent->faction, hitcost*getRank() );
                if (fg)
                    UniverseUtil::adjustFGRelationModifier( whichCp, fg->name, hitcost*getRank() );
            }
        }
    }
}
float Pilot::getAnger( const Unit *parent, const Unit *target ) const
{
    relationmap::const_iterator iter = effective_relationship.find( target );
    float rel = 0;
    if ( iter != effective_relationship.end() )
        rel = iter->second;
    if ( _Universe->isPlayerStarship( target ) ) {
        if (FactionUtil::GetFactionName( faction ).find( "pirates" ) != std::string::npos) {
            static unsigned int cachedCargoNum = 0;
            static bool good = true;
            if ( cachedCargoNum != target->numCargo() ) {
                cachedCargoNum = target->numCargo();
                good = true;
                for (unsigned int i = 0; i < cachedCargoNum; ++i) {
                    Cargo *c = &target->pImage->cargo[i];
                    if (c->quantity != 0 && c->GetCategory().find( "upgrades" ) == string::npos) {
                        good = false;
                        break;
                    }
                }
            }
            if (good) {
                static float goodness_for_nocargo =
                    XMLSupport::parse_float( vs_config->getVariable( "AI", "pirate_bonus_for_empty_hold", ".75" ) );
                rel += goodness_for_nocargo;
            }
        }
    }
    {
        int fac = faction;
        MapStringFloat::iterator mapiter = factions[fac]->ship_relation_modifier.find( target->name );
        if ( mapiter != factions[fac]->ship_relation_modifier.end() )
            rel += (*mapiter).second;
    }
    {
        int parent_cp = _Universe->whichPlayerStarship( parent );
        int target_cp = _Universe->whichPlayerStarship( target );
        if (parent_cp != -1) {
            Flightgroup *fg = target->getFlightgroup();
            if (fg)
                rel += UniverseUtil::getFGRelationModifier( parent_cp, fg->name );
        }
        if (target_cp != -1) {
            //... do we count it both ways? else?
            Flightgroup *fg = parent->getFlightgroup();
            if (fg)
                rel += UniverseUtil::getFGRelationModifier( target_cp, fg->name );
        }
    }

    return rel;
}

float Pilot::GetEffectiveRelationship( const Unit *parent, const Unit *target )  const
{
    return getAnger( parent, target )+UnitUtil::getFactionRelation( parent, target );
}

extern float myroundclamp( float i );

Animation* Pilot::getCommFace( Unit *parent, float mood, unsigned char &sex )
{
    vector< Animation* > *ani = getCommFaces( sex );
    if (ani == NULL) {
        ani = FactionUtil::GetRandCommAnimation( parent->faction, parent, sex );
        if (ani == NULL)
            return NULL;
    }
    if (ani->size() == 0)
        return NULL;
    mood += .1;
    mood *= ( ani->size() )/.2;
    unsigned int index = (unsigned int) myroundclamp( floor( mood ) );
    if ( index >= ani->size() )
        index = ani->size()-1;
    return (*ani)[index];
}

