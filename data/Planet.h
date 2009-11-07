#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QList>
#include <QPair>

typedef QPair<QString, QString> TerrainPair;
typedef QPair<QString, int> ResourceYield;
class Planet
{
public:
    Planet();
    Planet( const QString &orbit, const QString &type, const QString &distance, const QString &diameter,
            const QString &atmosphere);

    Planet( const QString &temperature, const QString &axialtilt, const QString &gravity,
            const QString &ocean, const QString &microorganisms, const QString &pollution,
            const QString &radiation);

    QString orbit() const { return m_orbit; }
    QString type() const { return m_type; }
    QString distance() const { return m_distance; }
    QString diameter() const { return m_diameter; }
    QString atmosphere() const { return m_atmosphere; }

    QString temp() const { return m_temperature; }
    QString axialTilt() const { return m_axialtilt; }
    QString gravity() const { return m_gravity; }
    QString ocean() const { return m_ocean; }
    QString microorganisms() const { return m_microorganisms; }
    QString pollution() const { return m_pollution; }
    QString radiation() const { return m_radiation; }

    QList<TerrainPair> terrain() { return m_terrain; }

    QList<ResourceYield> resources() { return m_resources; }

    void addTerrain(const QString &terrain, const QString &percentage);
    void addResource(const QString &resource, int yield);

private:

    //From SS
    QString m_orbit;
    QString m_type;
    QString m_distance;
    QString m_diameter;
    QString m_atmosphere;

    //From PMAP
    QString m_temperature;
    QString m_axialtilt;
    QString m_gravity;
    QString m_ocean;
    QString m_microorganisms;
    QString m_pollution;
    QString m_radiation;

    QList<TerrainPair> m_terrain;

    //From GEO
    QList<ResourceYield> m_resources;

};

#endif // PLANET_H
