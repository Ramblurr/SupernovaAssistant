#include "Planet.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

Planet::Planet()
{
}

Planet::Planet( const QString &name, const QString &sysname, const QString &orbit, const QString &suborbit,
                const QString &type, float distance, int diameter, const QString &atmosphere, const QString &special)
                    : m_name(name), m_sysname(sysname), m_orbit( orbit ), m_suborbit( suborbit ),
                      m_type( type ), m_distance( distance ), m_diameter( diameter ), m_atmosphere( atmosphere ),
                      m_special( special )
{
}

Planet::Planet( const QString &temperature, const QString &axialtilt, const QString &gravity,
        const QString &ocean, const QString &microorganisms, const QString &pollution,
        const QString &radiation)
            : m_temperature( temperature ), m_axialtilt( axialtilt ), m_gravity( gravity ),
              m_ocean( ocean ), m_microorganisms( microorganisms ), m_pollution( pollution ),
              m_radiation( radiation )
{
}

void Planet::addTerrain(const QString &terrain, const QString &percentage)
{
    if( terrain.isEmpty() )
        return;
    TerrainPair p;
    p.first = terrain;
    p.second = percentage;
    m_terrain.append(p);
}

void Planet::addResource(const QString &resource, int yield)
{
    if( !resource.isEmpty() )
        return;
    ResourceYield y;
    y.first = resource;
    y.second = yield;
    m_resources.append(y);
}

bool Planet::operator==( const Planet &other ) const
{
    bool result = m_name == other.name() &&
                  m_sysname == other.sysname() &&
                  m_orbit == other.orbit() &&
                  m_suborbit == other.suborbit() &&
                  m_type == other.type() &&
                  m_distance == other.distance() &&
                  m_diameter == other.diameter() &&
                  m_atmosphere == other.atmosphere() &&
                  m_special == other.special() &&
                  m_temperature == other.temp() &&
                  m_axialtilt == other.axialTilt() &&
                  m_gravity == other.gravity() &&
                  m_ocean == other.ocean() &&
                  m_microorganisms == other.microorganisms() &&
                  m_pollution == other.pollution() &&
                  m_radiation == other.radiation();

    QList<TerrainPair> terrains = other.terrain();
    foreach( TerrainPair pair, terrains )
        if( !m_terrain.contains( pair ) )
            return false;

    QList<ResourceYield> resources = other.resources();
    foreach( ResourceYield pair, resources )
        if( !m_resources.contains (pair ) )
            return false;
    return result;
}

QList<Planet> Planet::getPlanets( const QString & systemname )
{
    QList<Planet> planets;

    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( !db.isOpen() )
        return planets;

    // Data from System Scan
    QSqlQuery query( db );
    if( !systemname.isEmpty() )
    {
        query.prepare("SELECT * FROM systembodies WHERE sysname = :sysname");
        query.bindValue(":sysname", systemname);
    }
    else
        query.prepare("SELECT * FROM systembodies");

    query.exec();

    while( query.next() )
    {
        Planet p;
        loadSS(query, p);

        //Data from pmap
        QSqlQuery pmap_query(db);
        pmap_query.prepare("SELECT * FROM pmapdata WHERE pname = :pname");
        pmap_query.bindValue(":pname", p.name() );
        pmap_query.exec();
        if( pmap_query.next() )
        {
            loadPmap(pmap_query, p);
        }

        query.prepare("SELECT * FROM pmapterrain WHERE pname = :pname");
        query.bindValue(":pname", p.name() );
        query.exec();
        while( query.next() )
        {
            loadTerrain(query, p);
        }

        //Data from geodata
        query.prepare("SELECT * FROM geodata WHERE pname = :pname");
        query.bindValue(":pname", p.name() );
        query.exec();
        while( query.next() )
        {
            loadGeo(query, p);
        }
        planets.append(p);
    }
    return planets;
}

Planet Planet::find( const QString & planet )
{
    if( planet.isEmpty() )
        return Planet();
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( !db.isOpen() )
        return Planet();

    // Data from System Scan
    QSqlQuery query( db );
    query.prepare("SELECT * FROM systembodies WHERE pname = :pname");
    query.bindValue(":pname", planet);
    query.exec();

    Planet p;
    if( !query.next() )
        return p; // not found

    loadSS(query, p);

    query.finish();

    //Data from pmap
    query.prepare("SELECT * FROM pmapdata WHERE pname = :pname");
    query.bindValue(":pname", planet);
    query.exec();
    if( query.next() )
    {
        loadPmap(query, p);
    }

    query.prepare("SELECT * FROM pmapterrain WHERE pname = :pname");
    query.bindValue(":pname", planet);
    query.exec();
    while( query.next() )
    {
        loadTerrain(query, p);
    }

    //Data from geodata
    query.prepare("SELECT * FROM geodata WHERE pname = :pname");
    query.bindValue(":pname", planet);
    query.exec();
    while( query.next() )
    {
        loadGeo(query, p);
    }

    return p;
}

void Planet::loadPmap( const QSqlQuery &query, Planet &p )
{
    int idxTemp = query.record().indexOf( "temperature" );
    int idxTilt = query.record().indexOf( "axialtilt" );
    int idxGrav = query.record().indexOf( "gravity" );
    int idxOcean = query.record().indexOf( "ocean" );
    int idxMicro = query.record().indexOf( "microorganisms" );
    int idxPoll = query.record().indexOf( "pollution" );
    int idxRad = query.record().indexOf( "radiation" );

    QString temp = query.value( idxTemp ).toString();
    QString tilt = query.value( idxTilt ).toString();
    QString grav = query.value( idxGrav ).toString();
    QString ocean = query.value( idxOcean ).toString();
    QString micro = query.value( idxMicro ).toString();
    QString poll = query.value( idxPoll ).toString();
    QString rad = query.value( idxRad ).toString();

    p.m_temperature = temp;
    p.m_axialtilt = tilt;
    p.m_gravity = grav;
    p.m_ocean = ocean;
    p.m_microorganisms = micro;
    p.m_radiation = rad;
}

void Planet::loadTerrain( const QSqlQuery &query, Planet &planet )
{
    int idxTerr = query.record().indexOf( "name" );
    int idxPerc = query.record().indexOf( "percent" );
    QString terrain = query.value( idxTerr ).toString();
    QString percent = query.value( idxPerc ).toString();

    planet.addTerrain(terrain, percent);
}

void Planet::loadGeo( const QSqlQuery &query, Planet &planet )
{
    int idxRes = query.record().indexOf( "resource" );
    int idxYield = query.record().indexOf( "yield" );
    QString resource = query.value( idxRes ).toString();
    int yield = query.value( idxYield ).toInt();

    planet.addResource(resource, yield);
}

void Planet::loadSS( const QSqlQuery &query, Planet &planet )
{
    int idxName = query.record().indexOf( "name" );
    int idxSys = query.record().indexOf( "sysname" );
    int idxOrbit = query.record().indexOf( "orbit" );
    int idxSubOrbit = query.record().indexOf( "suborbit" );
    int idxType = query.record().indexOf( "type" );
    int idxOrbDist = query.record().indexOf( "orbdist" );
    int idxDiameter = query.record().indexOf( "diameter" );
    int idxAtmo = query.record().indexOf( "atmosphere" );
    int idxSpecial = query.record().indexOf( "special" );

    QString name =  query.value( idxName ).toString();
    QString sys = query.value( idxSys ).toString();
    int orbit = query.value( idxOrbit ).toInt();
    QString suborbit = query.value( idxSubOrbit ).toString();
    QString type = query.value( idxType ).toString();
    float orbdist = query.value( idxOrbDist ).toDouble();
    int diameter = query.value( idxDiameter ).toInt();
    QString atmosphere = query.value( idxAtmo ).toString();
    QString special = query.value( idxSpecial ).toString();

    planet.m_name = name;
    planet.m_sysname = sys;
    planet.m_orbit = orbit;
    planet.m_suborbit = suborbit;
    planet.m_type = type;
    planet.m_distance = orbdist;
    planet.m_diameter = diameter;
    planet.m_atmosphere = atmosphere;
    planet.m_special = special;
}
