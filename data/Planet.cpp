#include "Planet.h"

Planet::Planet()
{
}

Planet::Planet( const QString &orbit, const QString &type, const QString &distance, const QString &diameter,
        const QString &atmosphere)
            : m_orbit( orbit ), m_type( type ), m_distance( distance ),
              m_diameter( diameter ), m_atmosphere( atmosphere )
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
