#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QList>
#include <QPair>

typedef QPair<QString, QString> TerrainPair;
typedef QPair<QString, int> ResourceYield;

class QSqlQuery;

class Planet
{
public:
    Planet();
    Planet( const QString &name, const QString &sysname, const QString &orbit, const QString &suborbit,
            const QString &type, float distance, int diameter, const QString &atmosphere, const QString &special);

    Planet( const QString &temperature, const QString &axialtilt, const QString &gravity,
            const QString &ocean, const QString &microorganisms, const QString &pollution,
            const QString &radiation);

    QString name() const { return m_name; }
    QString sysname() const { return m_sysname; }
    QString orbit() const { return m_orbit; }
    QString suborbit() const { return m_suborbit; }
    QString type() const { return m_type; }
    float distance() const { return m_distance; }
    int diameter() const { return m_diameter; }
    QString atmosphere() const { return m_atmosphere; }
    QString special() const { return m_special; }

    QString temp() const { return m_temperature; }
    QString axialTilt() const { return m_axialtilt; }
    QString gravity() const { return m_gravity; }
    QString ocean() const { return m_ocean; }
    QString microorganisms() const { return m_microorganisms; }
    QString pollution() const { return m_pollution; }
    QString radiation() const { return m_radiation; }

    QList<TerrainPair> terrain() const { return m_terrain; }

    QList<ResourceYield> resources() const { return m_resources; }

    void addTerrain(const QString &terrain, const QString &percentage);
    void addResource(const QString &resource, int yield);

    bool operator==( const Planet &other ) const;

    /**
      @brief Gets planetary bodies from the database
      @param systemname Optionally specify the name of the system to retrieve planets from
      @return A list of all planetary bodies, or the ones on the requested system
      */
    static QList<Planet> getPlanets( const QString & systemname = "" );
    static Planet find( const QString & planet );


private:
    static void loadPmap( const QSqlQuery &query, Planet &planet );
    static void loadTerrain( const QSqlQuery &query, Planet &planet );
    static void loadGeo( const QSqlQuery &query, Planet &planet );
    static void loadSS( const QSqlQuery &query, Planet &planet );

    //From SS
    QString m_name;
    QString m_sysname;
    QString m_orbit;
    QString m_suborbit;
    QString m_type;
    float m_distance;
    int m_diameter;
    QString m_atmosphere;
    QString m_special;

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
