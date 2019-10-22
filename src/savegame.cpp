#include "cs_python.h"
#include "cmd/unit.h"
#include "hashtable.h"
#include <float.h>
#include "vsfilesystem.h"
#include <vector>
#include <string>
#include "configxml.h"
#include "vs_globals.h"
#include "savegame.h"
#include "load_mission.h"
#include <algorithm>
#include "cmd/script/mission.h"
#include "gfx/cockpit_generic.h"
#include "vsfilesystem.h"
#include "cmd/fg_util.h"

#include "options.h"



using namespace VSFileSystem;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::allocator;
using std::cerr;

std::string CurrentSaveGameName = "";
std::string GetHelperPlayerSaveGame( int num )
{
  if (CurrentSaveGameName.length() > 0) {
      if (game_options.remember_savegame) {
          VSFile  f;
          VSError err = f.OpenCreateWrite( "save.4.x.txt", UnknownFile );
          if (err <= Ok) {
              f.Write( CurrentSaveGameName );
              f.Close();
          }
      }
      if (num != 0)
          return CurrentSaveGameName+XMLSupport::tostring( num );
      return CurrentSaveGameName;
  }
  cout<<"Hi helper play "<<num<<endl;
  static string *res = NULL;
  if (res == NULL) {
      res = new std::string;
      VSFile  f;
      //TRY TO OPEN THE save.4.x.txt FILE WHICH SHOULD CONTAIN THE NAME OF THE SAVE TO USE
      VSError err = f.OpenReadOnly( "save.4.x.txt", UnknownFile ); // DELETE .4.x no longer used
      if (err > Ok) {
          //IF save.4.x.txt DOES NOT EXIST WE CREATE ONE WITH "default" AS SAVENAME
          err = f.OpenCreateWrite( "save.4.x.txt", UnknownFile );
          if (err <= Ok) {
              f.Write( game_options.new_game_save_name.c_str(), game_options.new_game_save_name.length() );
              f.Write( "\n", 1 );
              f.Close();
          } else {
              fprintf( stderr, "!!! ERROR : Creating default save.4.x.txt file : %s\n", f.GetFullPath().c_str() );
              exit( 1 );
          }
          err = f.OpenReadOnly( "save.4.x.txt", UnknownFile );
          if (err > Ok) {
              fprintf( stderr, "!!! ERROR : Opening the default save we just created\n" );
              exit( 1 );
          }
      }
      if (err <= Ok) {
          long length = f.Size();
          if (length > 0) {
              char *temp = (char*) malloc( length+1 );
              temp[length] = '\0';
              f.Read( temp, length );
              bool  end  = true;
              for (int i = length-1; i >= 0; i--) {
                  if (temp[i] == '\r' || temp[i] == '\n')
                      temp[i] = (end ? '\0' : '_');
                  else if (temp[i] == '\0' || temp[i] == ' ' || temp[i] == '\t')
                      temp[i] = (end ? '\0' : '_');
                  else
                      end = false;
              }
              *res = (temp);
              free( temp );
          }
          f.Close();
      }
      if ( game_options.remember_savegame && !res->empty() ) {
          //Set filetype to Unknown so that it is searched in homedir/
          if (*res->begin() == '~') {
              err = f.OpenCreateWrite( "save.4.x.txt", VSFileSystem::UnknownFile );
              if (err <= Ok) {
                  for (unsigned int i = 1; i < res->length(); i++) {
                      char cc = *(res->begin()+i);
                      f.Write( &cc, sizeof (char) );
                  }
                  char cc = 0;
                  f.Write( &cc, sizeof (char) );
                  f.Close();
              }
          }
      }
  }
  if ( num == 0 || res->empty() ) {
      cout<<"Here";
      return *res;
  }
  return (*res)+XMLSupport::tostring( num );

}

std::string GetWritePlayerSaveGame( int num )
{
    string ret = GetHelperPlayerSaveGame( num );
    if ( !ret.empty() )
        if (*ret.begin() == '~')
            return ret.substr( 1, ret.length() );
    return ret;
}

std::string GetReadPlayerSaveGame( int num )
{
    string ret = GetHelperPlayerSaveGame( num );
    if ( !ret.empty() )
        if (*ret.begin() == '~')
            return "";
    return ret;
}

//Used only to copy a savegame to a different named one
void SaveFileCopy( const char *src, const char *dst )
{
    if (dst[0] != '\0' && src[0] != '\0') {
        VSFile  f;
        VSError err = f.OpenReadOnly( src, SaveFile );
        if (err <= Ok) {
            string savecontent = f.ReadFull();
            f.Close();
            err = f.OpenCreateWrite( dst, SaveFile );
            if (err <= Ok) {
                f.Write( savecontent );
                f.Close();
            } else {
                fprintf( stderr, "WARNING : couldn't open savegame to copy to : %s as SaveFile", dst );
            }
        } else {
            fprintf( stderr, "WARNING : couldn't find the savegame to copy : %s as SaveFile", src );
        }
    }
}

class MissionStringDat
{
public:
    typedef std::map< string, vector< std::string > >MSD;
    MSD m;
};

class MissionFloatDat
{
public:
    typedef vsUMap< string, vector< float > >MFD;
    MFD m;
};

SaveGame::SaveGame( const std::string &pilot )
{
    callsign = pilot;
    ForceStarSystem   = string( "" );
    PlayerLocation.Set( FLT_MAX, FLT_MAX, FLT_MAX );
    missionstringdata = new MissionStringDat;
    missiondata = new MissionFloatDat;
}

SaveGame::~SaveGame()
{
    delete missionstringdata;
    delete missiondata;
}

void SaveGame::SetStarSystem( string sys )
{
    ForceStarSystem = sys;
}

string SaveGame::GetStarSystem()
{
    return ForceStarSystem;
}

string SaveGame::GetOldStarSystem()
{
    return ForceStarSystem;
}

void SaveGame::SetPlayerLocation( const Vector &v )
{
    if ( ( FINITE( v.i ) && FINITE( v.j ) && FINITE( v.k ) ) ) {
        PlayerLocation = v;
    } else {
        VSFileSystem::vs_fprintf( stderr, "NaN ERROR saving unit\n" );
        assert( FINITE( v.i ) && FINITE( v.j ) && FINITE( v.k ) );
        PlayerLocation.Set( 1, 1, 1 );
    }
}

Vector SaveGame::GetPlayerLocation()
{
    return PlayerLocation;
}

void SaveGame::RemoveUnitFromSave( long address ) // DELETE ? unused function?
{
}

string SaveGame::WriteNewsData()
{
    string ret( "" );
    gameMessage last;
    vector< gameMessage >tmp;
    int    i = 0;
    vector< string >newsvec;
    newsvec.push_back( "news" );
    while ( ( mission->msgcenter->last( i++, last, newsvec ) ) )
        tmp.push_back( last );
    ret += XMLSupport::tostring( i )+"\n";
    for (int j = tmp.size()-1; j >= 0; j--) {
        char *msg = strdup( tmp[j].message.get().c_str() );
        int   k   = 0;
        while (msg[k]) {
            if (msg[k] == '\r')
                msg[k] = ' ';
            if (msg[k] == '\n')
                msg[k] = '/';
            k++;
        }
        ret += string( msg )+"\n";
        free( msg );
    }
    return ret;
}

vector< string >parsePipedString( string s )
{
    string::size_type loc;
    vector< string >  ret;
    while ( ( loc = s.find( "|" ) ) != string::npos ) {
        ret.push_back( s.substr( 0, loc ) );
        cout<<"Found ship named : "<<s.substr( 0, loc )<<endl;
        s = s.substr( loc+1 );
    }
    if ( s.length() ) {
        ret.push_back( s );
        cout<<"Found ship named : "<<s<<endl;
    }
    return ret;
}

string createPipedString( vector< string >s )
{
    string ret;
    for (unsigned int i = 0; i < s.size()-1; i++)
        ret += s[i]+"|";
    if ( s.size() )
        ret += s.back();
    return ret;
}

void CopySavedShips( std::string filename, int player_num, const std::vector< std::string > &starships, bool load )
{
    for (unsigned int i = 0; i < starships.size(); i += 2) {
        if (i == 2) i = 1;
        VSFile src, dst;
        string srcnam = filename;
        string dstnam = GetWritePlayerSaveGame( player_num );
        string tmp;
        if (load) {
            tmp    = srcnam;
            srcnam = dstnam;
            dstnam = tmp;
        }
        VSError e = src.OpenReadOnly( srcnam+"/"+starships[i]+".csv", UnitSaveFile );
        if (e <= Ok) {
            VSFileSystem::CreateDirectoryHome( VSFileSystem::savedunitpath+"/"+dstnam );
            VSError f = dst.OpenCreateWrite( dstnam+"/"+starships[i]+".csv", UnitFile );
            if (f <= Ok) {
                string srcdata = src.ReadFull();
                dst.Write( srcdata );
            } else {
                printf( "Error: Cannot Copy Unit %s from save file %s to %s\n",
                       starships[i].c_str(),
                       srcnam.c_str(),
                       dstnam.c_str() );
            }
        } else {
            printf( "Error: Cannot Open Unit %s from save file %s.\n",
                   starships[i].c_str(),
                   srcnam.c_str() );
        }
    }
}

void WriteSaveGame( Cockpit *cp, bool auto_save )
{
    int player_num = 0;
    for (unsigned int kk = 0; kk < _Universe->numPlayers(); ++kk)
        if (_Universe->AccessCockpit( kk ) == cp)
            player_num = kk;
    Unit *un = cp->GetSaveParent();
    if (!un)
        return;
    if (un->GetHull() > 0) {
        vector< string > packedInfo;
        cp->PackUnitInfo(packedInfo);
        
        cp->savegame->WriteSaveGame( cp->activeStarSystem->getFileName().c_str(),
                                     un->LocalPosition(), cp->credits, packedInfo, auto_save ? -1 : player_num );
        un->WriteUnit( cp->GetUnitModifications().c_str() );
        if (GetWritePlayerSaveGame( player_num ).length() && !auto_save) {
            cp->savegame->SetSavedCredits( _Universe->AccessCockpit()->credits );
            cp->savegame->SetStarSystem( cp->activeStarSystem->getFileName() );
            cp->savegame->SetPlayerLocation( un->LocalPosition() );
            CopySavedShips( cp->GetUnitModifications(), player_num, packedInfo, false );
        }
    }
}

int hopto( char *buf, char endln, char endln2, int readlen )
{
    if (endln == ' ' || endln2 == ' ')
        while (buf[readlen] && buf[readlen] == ' ')
            readlen++;
    for (; buf[readlen] != 0 && buf[readlen] != endln && buf[readlen] != endln2; readlen++)
        ;
    if ( (buf[readlen] && buf[readlen] == endln) || buf[readlen] == endln2 )
        readlen++;
    return readlen;
}

void SaveGame::ReadNewsData( char* &buf, bool just_skip )
{
    int numnews;
    int i = 0;
    vector< string >n00s;
    n00s.push_back( "news" );
    vector< string >nada;
    mission->msgcenter->clear( n00s, nada );
    int offset = hopto( buf, '\n', '\n', 0 );
    if (offset > 0) {
        sscanf( buf, "%d\n", &numnews );
        buf += offset;
        for (i = 0; i < numnews; i++) {
            offset = hopto( buf, '\n', '\n', 0 );
            if (offset > 0)
                buf[offset-1] = 0;
            int l = 0;
            for (l = 0; l < offset-1; l++)
                if (buf[l] != '\r' && buf[l] != '\n')
                    break;
            if (!just_skip && buf[l] != '\r' && buf[l] != '\n' && buf[l])
                mission->msgcenter->add( "game", "news", buf+l );
            buf += offset;
        }
    }
}

void SaveGame::AddUnitToSave( const char *filename, int type, const char *faction, long address )
{
    if ( game_options.Drone.compare( filename ) )
        RemoveUnitFromSave( address );
}

std::vector< float >& SaveGame::getMissionData( const std::string &magic_number )
{
    return missiondata->m[magic_number];
}

const std::vector< float >& SaveGame::readMissionData( const std::string &magic_number ) const
{
    static const std::vector< float > empty;
    MissionFloatDat::MFD::const_iterator it = missiondata->m.find( magic_number );
    return ( it == missiondata->m.end() ) ? empty : it->second;
}

unsigned int SaveGame::getMissionDataLength( const std::string &magic_number ) const
{
    MissionFloatDat::MFD::const_iterator it = missiondata->m.find( magic_number );
    return ( it == missiondata->m.end() ) ? 0 : it->second.size();
}

const std::vector< string >& SaveGame::readMissionStringData( const std::string &magic_number ) const
{
    static const std::vector< string > empty;
    MissionStringDat::MSD::const_iterator it = missionstringdata->m.find( magic_number );
    return ( it == missionstringdata->m.end() ) ? empty : it->second;
}

std::vector< string >& SaveGame::getMissionStringData( const std::string &magic_number )
{
    return missionstringdata->m[magic_number];
}

unsigned int SaveGame::getMissionStringDataLength( const std::string &magic_number ) const
{
    MissionStringDat::MSD::const_iterator it = missionstringdata->m.find( magic_number );
    return ( it == missionstringdata->m.end() ) ? 0 : it->second.size();
}

template < class MContainerType >
void RemoveEmpty( MContainerType &t )
{
    typename MContainerType::iterator i;
    for (i = t.begin(); i != t.end();)
        if ( i->second.empty() )
            t.erase( i++ );

        else
            ++i;
}

string SaveGame::WriteMissionData()
{
    string ret( " " );
    RemoveEmpty< MissionFloatDat::MFD > ( missiondata->m );
    ret += XMLSupport::tostring( (int) missiondata->m.size() );
    for (MissionFloatDat::MFD::iterator i = missiondata->m.begin(); i != missiondata->m.end(); i++) {
        unsigned int siz = (*i).second.size();
        
        // Escape spaces within the key by replacing them with a special char ¬
        string k = (*i).first;
        { for (size_t i=0,len=k.length(); i<len; ++i)
            if (k[i] == ' ') k[i] = '`'; }
        
        ret += string( "\n" )+k+string( " " )+XMLSupport::tostring( siz )+" ";
        for (unsigned int j = 0; j < siz; j++)
            ret += XMLSupport::tostring( (*i).second[j] )+" ";
    }
    return ret;
}

std::string scanInString( char* &buf )
{
    std::string str;
    while ( *buf && isspace( *buf ) )
        buf++;
    char *start = buf;
    while ( *buf && ( !isspace( *buf ) ) )
        buf++;
    str.resize(buf - start);
    for (size_t i=0; start < buf; ++i)
        str[i] = *(start++);
    return str;
}

void SaveGame::ReadMissionData( char* &buf, bool select_data, const std::set< std::string > &select_data_filter )
{
    missiondata->m.clear();
    int   mdsize;
    char *buf2 = buf;
    sscanf( buf2, " %d ", &mdsize );
    //Put ptr to point after the number we just read
    buf2 += hopto( buf2, ' ', '\n', 0 );
    for (int i = 0; i < mdsize; i++) {
        int    md_i_size;
        string mag_num( scanInString( buf2 ) );
        
        // Unescape spaces within the key by replacing the special char ¬
        { for (size_t i=0,len=mag_num.length(); i<len; ++i)
            if (mag_num[i] == '`') mag_num[i] = ' '; }
        
        sscanf( buf2, "%d ", &md_i_size );
        //Put ptr to point after the number we just read
        buf2 += hopto( buf2, ' ', '\n', 0 );
        vector< float > *vecfloat = 0;
        bool skip = true;
        if ( !select_data || select_data_filter.count( mag_num ) ) {
            vecfloat = &missiondata->m[mag_num];
            vecfloat->clear();
            vecfloat->reserve(md_i_size);
            skip = false;
        }
        for (int j = 0; j < md_i_size; j++) {
            if (!skip) {
                double float_val = atof(buf2);
                vecfloat->push_back( float_val );
            }
            //Put ptr to point after the number we just read
            buf2 += hopto( buf2, ' ', '\n', 0 );
        }
    }
    buf = buf2;
}

string AnyStringScanInString( char* &buf )
{
    unsigned int size = 0;
    bool found = false;
    while ( (*buf) && ( (*buf) != ' ' || (!found) ) ) {
        if ( (*buf) >= '0' && (*buf) <= '9' ) {
            size *= 10;
            size += (*buf)-'0';
            found = true;
        }
        buf++;
    }
    if (*buf)
        buf++;
    string ret;
    ret.resize( size );
    unsigned int i = 0;
    while (i < size && *buf)
        ret[i++] = *(buf++);
    return ret;
}

void AnyStringSkipInString( char* &buf )
{
    unsigned int size = 0;
    bool found = false;
    while ( (*buf) && ( (*buf) != ' ' || (!found) ) ) {
        if ( (*buf) >= '0' && (*buf) <= '9' ) {
            size *= 10;
            size += (*buf)-'0';
            found = true;
        }
        buf++;
    }
    if (*buf)
        buf++;
    unsigned int i = 0;
    while (i < size && *buf)
        ++i, ++buf;
}

string AnyStringWriteString( string input )
{
    return XMLSupport::tostring( (int) input.length() )+" "+input;
}

void SaveGame::ReadMissionStringData( char* &buf, bool select_data, const std::set< std::string > &select_data_filter )
{
    missionstringdata->m.clear();
    int   mdsize;
    char *buf2 = buf;
    sscanf( buf2, " %d ", &mdsize );
    //Put ptr to point after the number we just read
    buf2 += hopto( buf2, ' ', '\n', 0 );
    for (int i = 0; i < mdsize; i++) {
        int    md_i_size;
        string mag_num( AnyStringScanInString( buf2 ) );
        md_i_size = strtol( buf2, (char**) NULL, 10 );
        //Put ptr to point after the number we just read
        buf2 += hopto( buf2, ' ', '\n', 0 );
        vector< string > *vecstring = 0;
        bool skip = true;
        if ( !select_data || select_data_filter.count( mag_num ) ) {
            vecstring = &missionstringdata->m[mag_num];
            vecstring->clear();
            vecstring->reserve(md_i_size);
            skip = false;
        }
        for (int j = 0; j < md_i_size; j++) {
            if (skip)
                AnyStringSkipInString( buf2 );

            else
                vecstring->push_back( AnyStringScanInString( buf2 ) );
        }
    }
    buf = buf2;
    this->PurgeZeroStarships();
}

void SaveGame::PurgeZeroStarships() // DELETE unused function?
{
    for (MissionStringDat::MSD::iterator i = missionstringdata->m.begin(), ie = missionstringdata->m.end(); i != ie; ++i)
        if ( fg_util::IsFGKey( i->first ) )
            if ( fg_util::CheckFG( i->second ) )
            {
                //printf( "correcting flightgroup %s to have right landed ships\n", i->first.c_str() );
            }
}

static inline void PushBackUInt( unsigned int i, vector< char > &ret )
{
    char tmp[32];
    if (!i) {
        ret.push_back( '0' );
    } else {
        unsigned int p = 0, q = ret.size();
        while (i) {
            tmp[p++] = (i%10+'0');
            i /= 10;
        }
        ret.resize( q+p );
        while (p)
            ret[q++] = tmp[--p];
    }
}

static inline void PushBackChars( const char *c, vector< char > &ret )
{
    int ini = ret.size();
    ret.resize( ret.size()+strlen( c ) );
    while (*c)
        ret[ini++] = *(c++);
}

static inline void PushBackString( const string &input, vector< char > &ret )
{
    PushBackUInt( input.length(), ret );
    PushBackChars( " ", ret );
    PushBackChars( input.c_str(), ret );
}

void SaveGame::WriteMissionStringData( std::vector< char > &ret )
{
    RemoveEmpty< MissionStringDat::MSD > ( missionstringdata->m );
    PushBackUInt( missionstringdata->m.size(), ret );
    for (MissionStringDat::MSD::iterator i = missionstringdata->m.begin(); i != missionstringdata->m.end(); i++) {
        const string &key = (*i).first;
        unsigned int  siz = (*i).second.size();
        if (key == "mission_descriptions" || key == "mission_scripts" || key == "mission_vars" || key == "mission_names") {
            //*** BLACKLIST ***
            //Don't bother to write these out since they waste a lot of space and aren't used.
            siz = 0; //Not writing them out altogether will cause saved games to break.
        }
        PushBackChars( "\n", ret );
        PushBackString( key, ret );
        PushBackUInt( siz, ret );
        PushBackChars( " ", ret );
        for (unsigned int j = 0; j < siz; j++)
            PushBackString( (*i).second[j], ret );
    }
}

void SaveGame::ReadStardate( char* &buf )
{
    string stardate( AnyStringScanInString( buf ) );
    cout<<"Read stardate: "<<stardate<<endl;
    _Universe->current_stardate.InitTrek( stardate );
}

void SaveGame::ReadSavedPackets( char* &buf,
                                 bool commitfactions,
                                 bool skip_news,
                                 bool select_data,
                                 const std::set< std::string > &select_data_filter )
{
    int  a = 0;
    char unitname[1024];
    char factname[1024];
    while ( 3 == sscanf( buf, "%d %s %s", &a, unitname, factname ) ) {
        //Put i to point after what we parsed (on the 3rd space read)
        while ( (*buf) && isspace( *buf ) )
            ++buf;
        buf += hopto( buf, ' ', '\n', 0 );
        buf += hopto( buf, ' ', '\n', 0 );
        buf += hopto( buf, ' ', '\n', 0 );
        if ( a == 0 && 0 == strcmp( unitname, "factions" ) && 0 == strcmp( factname, "begin" ) ) {
            if (commitfactions) FactionUtil::LoadSerializedFaction( buf );
            break;             //GOT TO BE THE LAST>... cus it's stupid :-) and mac requires the factions to be loaded AFTER this function call
        } else if ( a == 0 && 0 == strcmp( unitname, "mission" ) && 0 == strcmp( factname, "data" ) ) {
            ReadMissionData( buf, select_data, select_data_filter );
        } else if ( a == 0 && 0 == strcmp( unitname, "missionstring" ) && 0 == strcmp( factname, "data" ) ) {
            ReadMissionStringData( buf, select_data, select_data_filter );
        } else if ( a == 0 && 0 == strcmp( unitname, "python" ) && 0 == strcmp( factname, "data" ) ) {
            last_written_pickled_data = last_pickled_data = UnpickleAllMissions( buf );
        } else if ( a == 0 && 0 == strcmp( unitname, "news" ) && 0 == strcmp( factname, "data" ) ) {
            if (commitfactions) ReadNewsData( buf, skip_news );
        } else if ( a == 0 && 0 == strcmp( unitname, "stardate" ) && 0 == strcmp( factname, "data" ) ) {
            //On server side we expect the latest saved stardate in dynaverse.dat too
            if (commitfactions && !SERVER /*server never wants to take "orders" from shapeshifters...*/)
                ReadStardate( buf );
            else
                AnyStringScanInString( buf );
        } else {
            char output[31] = {0};
            strncpy( output, buf, 30 );
            printf( "buf unrecognized %s...\n", output );
        }
    }
}

void SaveGame::LoadSavedMissions()
{
    unsigned int    i;
    vector< string >scripts  = getMissionStringData( "active_scripts" );
    vector< string >missions = getMissionStringData( "active_missions" );
    PyRun_SimpleString(
        "import VS\nVS.loading_active_missions=True\nprint(\"Loading active missions \"+str(VS.loading_active_missions))\n" );
    //kill any leftovers so they don't get loaded twice.
    Mission *ignoreMission = Mission::getNthPlayerMission( _Universe->CurrentCockpit(), 0 );
    for (i = active_missions.size()-1; i > 0; --i)      //don't terminate zeroth mission
        if (active_missions[i]->player_num == _Universe->CurrentCockpit()
            && active_missions[i] != ignoreMission)
            active_missions[i]->terminateMission();
    for (i = 0; i < scripts.size() && i < missions.size(); ++i) {
        try {
            if ( !missions[i].empty() )             //Built-in/networking missions
                LoadMission( missions[i].c_str(), scripts[i], false );
        }
        catch (...) {
            if ( PyErr_Occurred() ) {
                PyErr_Print();
                PyErr_Clear();
                fflush( stderr );
                fflush( stdout );
            } else {throw; }}
    }
    PyRun_SimpleString( "import VS\nVS.loading_active_missions=False\n" );
    getMissionStringData( "active_scripts" )  = scripts;
    getMissionStringData( "active_missions" ) = missions;
}

string SaveGame::WriteSavedUnit( SavedUnits *su )
{
    return string( "\n" )+XMLSupport::tostring( su->type )+string( " " )+su->filename+" "+su->faction;
}

extern bool STATIC_VARS_DESTROYED;

static char * tmprealloc( char *var, int &oldlength, int newlength )
{
    if (oldlength < newlength) {
        oldlength = newlength;
        var = (char*) realloc( var, newlength );
    }
    memset( var, 0, newlength );
    return var;
}

string SaveGame::WritePlayerData( const Vector &FP,
                                  std::vector< std::string >unitname,
                                  const char *systemname,
                                  float credits,
                                  std::string fact )
{
    string  playerdata( "" );
    int     MB  = MAXBUFFER;
    char   *tmp = (char*) malloc( MB );
    memset( tmp, 0, MB );

    Vector FighterPos = PlayerLocation-FP;
    FighterPos = FP;
    string pipedunitname = createPipedString( unitname );
    tmp = tmprealloc( tmp, MB, pipedunitname.length()+strlen( systemname )+256 /*4 floats*/ );
    //If we specify no faction, it won't be saved in there
    if (fact != "")
        sprintf( tmp, "%s^%f^%s %f %f %f %s", systemname, credits,
                pipedunitname.c_str(), FighterPos.i, FighterPos.j, FighterPos.k, fact.c_str() );
    else
        sprintf( tmp, "%s^%f^%s %f %f %f", systemname, credits, pipedunitname.c_str(), FighterPos.i, FighterPos.j, FighterPos.k );
    playerdata = string( tmp );
    this->playerfaction = fact;
    SetSavedCredits( credits );
    free( tmp );
    tmp = NULL;

    return playerdata;
}

string SaveGame::WriteDynamicUniverse()
{
    string dyn_univ( "" );
    int    MB  = MAXBUFFER;
    char  *tmp = (char*) malloc( MB );
    memset( tmp, 0, MB );
    //Write mission data
    //we save the stardate
    if (SERVER) {
        cerr<<"SAVING STARDATE - SERVER="<<SERVER<<endl;
    }
    string stardate = AnyStringWriteString( _Universe->current_stardate.GetFullTrekDate() );
    dyn_univ += "\n0 stardate data " + stardate;

    memset( tmp, 0, MB );
    sprintf( tmp, "\n%d %s %s", 0, "mission", "data " );
    dyn_univ += string( tmp );
    dyn_univ += WriteMissionData();
    memset( tmp, 0, MB );
    sprintf( tmp, "\n%d %s %s", 0, "missionstring", "data " );
    dyn_univ += string( tmp );
    vector< char >missionstringdata1;
    WriteMissionStringData( missionstringdata1 );
    dyn_univ += string( &missionstringdata1[0], missionstringdata1.size() );
    if (!STATIC_VARS_DESTROYED)
        last_written_pickled_data = PickleAllMissions();
    tmp = tmprealloc( tmp, MB, last_written_pickled_data.length()+256 /*4 floats*/ );
    sprintf( tmp, "\n%d %s %s %s ", 0, "python", "data", last_written_pickled_data.c_str() );
    dyn_univ += string( tmp );

    //Write news data
    memset( tmp, 0, MB );
    sprintf( tmp, "\n%d %s %s", 0, "news", "data " );
    dyn_univ += string( tmp );
    dyn_univ += WriteNewsData();
    //Write faction relationships
    memset( tmp, 0, MB );
    sprintf( tmp, "\n%d %s %s", 0, "factions", "begin " );
    dyn_univ += string( tmp );
    dyn_univ += FactionUtil::SerializeFaction();

    free( tmp );
    tmp = NULL;

    return dyn_univ;
}

using namespace VSFileSystem;

string SaveGame::WriteSaveGame( const char *systemname,
                                const Vector &FP,
                                float credits,
                                std::vector< std::string >unitname,
                                int player_num,
                                std::string fact,
                                bool write )
{
    savestring  = string( "" );
    printf( "Writing Save Game %s", outputsavegame.c_str() );
    savestring += WritePlayerData( FP, unitname, systemname, credits, fact );
    savestring += WriteDynamicUniverse();
    if (outputsavegame.length() != 0) {
        if (write) {
            VSFile  f;
            VSError err = f.OpenCreateWrite( outputsavegame, SaveFile );
            if (err <= Ok) {
                //check
                //WRITE THE SAVEGAME TO THE MISSION SAVENAME
                f.Write( savestring.c_str(), savestring.length() );
                f.Close();
                if (player_num != -1) {
                    //AND THEN COPY IT TO THE SPECIFIED SAVENAME (from save.4.x.txt)
                    last_pickled_data = last_written_pickled_data;
                    string sg = GetWritePlayerSaveGame( player_num );
                    SaveFileCopy( outputsavegame.c_str(), sg.c_str() );
                }
            } else {
                //error occured while opening file
                cerr<<"occured while opening file: "<<outputsavegame<<endl;
            }
        }
    }
    return savestring;
}

static float savedcredits = 0;
float SaveGame::GetSavedCredits()
{
    return savedcredits;
}

void SaveGame::SetSavedCredits( float c )
{
    savedcredits = c;
}

void SaveGame::SetOutputFileName( const string &filename )
{
    if ( !filename.empty() )
        outputsavegame = string( "Autosave-" )+filename;
    else
        outputsavegame = string( "Autosave" ); //empty name?
}

void SaveGame::ParseSaveGame( const string &filename_p,
                              string &FSS,
                              const string &originalstarsystem,
                              Vector &PP,
                              bool &shouldupdatepos,
                              float &credits,
                              vector< string > &savedstarship,
                              int player_num,
                              const string &save_contents,
                              bool read,
                              bool commitfaction,
                              bool quick_read,
                              bool skip_news,
                              bool select_data,
                              const std::set< std::string > &select_data_filter )
{
    const string &str     = save_contents;     //alias
    string filename;
    //Now leave filename empty, use the default name regardless...
    shouldupdatepos = !(PlayerLocation.i == FLT_MAX || PlayerLocation.j == FLT_MAX || PlayerLocation.k == FLT_MAX);
    //WE WILL ALWAYS SAVE THE CURRENT SAVEGAME IN THE MISSION SAVENAME (IT WILL BE COPIED TO THE SPECIFIED SAVENAME)
    SetOutputFileName( filename );
    VSFile  f;
    VSError err = FileNotFound;
    if (read) {
        //TRY TO GET THE SPECIFIED SAVENAME TO LOAD
        string plsave = GetReadPlayerSaveGame( player_num );
        if ( plsave.length() ) {
            err = f.OpenReadOnly( plsave, SaveFile );
            if (err > Ok)                             //failed in SaveFile
                //Try as an UnknownFile to get a datadir saved game, like New_Game.
                err = f.OpenReadOnly( plsave, UnknownFile );
        } else if (filename.length() > 0) {
            //IF NONE SIMPLY LOAD THE MISSION DEFAULT ONE
            err = f.OpenReadOnly( filename, SaveFile );
        }
    }
    if (err <= Ok) {
        if (quick_read) {
            char *buf = (char*) malloc( game_options.quick_savegame_summaries_buffer+1 );
            buf[game_options.quick_savegame_summaries_buffer] = '\0';
            err = f.ReadLine( buf, game_options.quick_savegame_summaries_buffer );
            savestring = buf;
            free( buf );
        } else {
            savestring = f.ReadFull();
        }
    }
    if ( err <= Ok || (!read && str != "") ) {
        if (!read)
            savestring = str;
        if (savestring.length() > 0) {
            char *buf = new char[savestring.length()+1];
            buf[savestring.length()] = '\0';
            memcpy( buf, savestring.c_str(), savestring.length() );
            int   headlen     = hopto( buf, '\n', '\n', 0 );
            char *deletebuf   = buf;
            char *tmp2 = (char*) malloc( headlen+2 );
            char *freetmp2    = tmp2;
            char *factionname = (char*) malloc( headlen+2 );
            if ( headlen > 0 && (buf[headlen-1] == '\n' || buf[headlen-1] == ' ' || buf[headlen-1] == '\r') )
                buf[headlen-1] = '\0';
            factionname[headlen+1] = '\0';
            Vector tmppos;
            int     res = sscanf( buf, "%s %lf %lf %lf %s", tmp2, &tmppos.i, &tmppos.j, &tmppos.k, factionname );
            if (res == 4 || res == 5) {
                //Extract credits & starship
                for (int j = 0; '\0' != tmp2[j]; j++)
                    if (tmp2[j] == '^') {
                        sscanf( tmp2+j+1, "%f", &credits );
                        tmp2[j] = '\0';
                        for (int k = j+1; tmp2[k] != '\0'; k++)
                            if (tmp2[k] == '^') {
                                tmp2[k] = '\0';
                                savedstarship.clear();
                                savedstarship = parsePipedString( tmp2+k+1 );
                                break;
                            }
                        break;
                    }
                //In networking save we include the faction at the end of the first line
                if (res == 5) {
                    playerfaction = string( factionname );
                    cout<<"Found faction in save file : "<<playerfaction<<endl;
                } else {
                    //If no faction -> default to privateer
                    playerfaction = string( "privateer" );
                    cout<<"Faction not found assigning default one: privateer"<<endl;
                }
                free( factionname );
                if (ForceStarSystem.length() == 0)
                    ForceStarSystem = string( tmp2 );
                if (PlayerLocation.i == FLT_MAX || PlayerLocation.j == FLT_MAX || PlayerLocation.k == FLT_MAX) {
                    shouldupdatepos = true;
                    PlayerLocation  = tmppos; //LaunchUnitNear(tmppos);
                }
                buf += headlen;
                ReadSavedPackets( buf, commitfaction, skip_news, select_data, select_data_filter );
            }
            free( freetmp2 );
            freetmp2 = NULL;
            tmp2     = NULL;
            delete[] deletebuf;
        }
        if (read)
            f.Close();
    }
    if (PlayerLocation.i == FLT_MAX || PlayerLocation.j == FLT_MAX || PlayerLocation.k == FLT_MAX) {
        shouldupdatepos = false;
        PlayerLocation  = PP;
    } else {
        PP = PlayerLocation;
        shouldupdatepos = true;
    }
    if (ForceStarSystem.length() == 0) {
        ForceStarSystem = FSS;
        originalsystem  = FSS;
    } else {
        originalsystem = ForceStarSystem;
        FSS = ForceStarSystem;
    }
    SetSavedCredits( credits );
}

const string& GetCurrentSaveGame()
{
    return CurrentSaveGameName;
}

string SetCurrentSaveGame( string newname )
{
    string oldname = CurrentSaveGameName;
    CurrentSaveGameName = newname;
    return oldname;
}

const string& GetSaveDir()
{
    static string rv = VSFileSystem::homedir+"/save/";
    return rv;
}

