#ifndef GALAXY_XML_H_
#define GALAXY_XML_H_

#include <string>
#include <map>
#include <vector>
#include "gfx/vec.h"
#include "vsfilesystem.h"

void ComputeSerials( std::vector< std::string > &stak );

namespace GalaxyXML
{
using std::string;
typedef std::map< std::string, std::string >StringMap;
class SubHeirarchy;
class SGalaxy
{
protected:
    friend class Galaxy;
    class SubHeirarchy*subheirarchy;
    StringMap data;
    SGalaxy & operator=( const SGalaxy &a );

public: SGalaxy()
    {
        subheirarchy = NULL;
    }
    SGalaxy( const char *configfile );
    SGalaxy( const SGalaxy &g );
    void writeGalaxy( VSFileSystem::VSFile &f ) const;
    void writeSector( VSFileSystem::VSFile &f, int tabs, const string &sectorType, SGalaxy *planet_types ) const;

    void processGalaxy( const string &sys );
    void processSystem( const string &sys, const Vector &suggested_coordinates );

    ~SGalaxy();
    const string& getVariable( const std::vector< string > &section, const string &name, const string &default_value ) const;
    const string& getRandSystem( const string &section, const string &default_value ) const;
    const string& getVariable( const string &section, const string &name, const string &defaultvalue ) const;
    const string& getVariable( const string &section, const string &subsection, const string &name,
                               const string &defaultvalue ) const;
    bool setVariable( const string &section, const string &name, const string &value );
    bool setVariable( const string &section, const string &subsection, const string &name, const string &value );
    void addSection( const std::vector< string > &section );
    void setVariable( const std::vector< string > &section, const string &name, const string &value );
    SubHeirarchy& getHeirarchy();
    const std::string&operator[]( const std::string &s ) const
    {
        static std::string empty_string;
        StringMap::const_iterator it = data.find( s );
        if ( it != data.end() )
            return it->second;

        else
            return empty_string;
    }
};
class Galaxy : public SGalaxy
{
    SGalaxy * getInitialPlanetTypes();
    SGalaxy  *planet_types;
    SGalaxy & operator=( const SGalaxy &a );
    StringMap initial2name;
    StringMap texture2name;
    void setupPlanetTypeMaps();
public:

    const string& getPlanetNameFromInitial( const string &abbrev ) const
    {
        static std::string empty_string;
        StringMap::const_iterator it = initial2name.find( abbrev );
        if ( it != initial2name.end() )
            return it->second;

        else
            return empty_string;
    }
    const string& getPlanetNameFromTexture( const string &tex ) const
    {
        static std::string empty_string;
        StringMap::const_iterator it = texture2name.find( tex );
        if ( it != texture2name.end() )
            return it->second;

        else
            return empty_string;
    }
    const string& getPlanetVariable( const string &name, const string &defaultvalue ) const;
    const string& getPlanetVariable( const string &planet, const string &name, const string &defaultvalue ) const;
    void writeGalaxy( VSFileSystem::VSFile &f ) const;
    SGalaxy * getPlanetTypes();
    bool setPlanetVariable( const string &name, const string &value );
    void addPlanetSection( const std::vector< string > &section );
    bool setPlanetVariable( const string &planet, const string &name, const string &value );
    Galaxy()
    {
        subheirarchy = NULL;
        planet_types = NULL;
    }
    Galaxy( const char *configfile );
    Galaxy( const SGalaxy &g );
};

class SubHeirarchy : public vsUMap< std::string, class SGalaxy >
{};
}

string getStarSystemFileName( const string &input );
string getStarSystemName( const string &in );
string getStarSystemSector( const string &in );
#endif

