#ifndef SYSTEM_H
#define SYSTEM_H

#include "Planet.h"
#include "WarpPoint.h"

#include <QString>
#include <QList>

class System
{
public:
    System();
    System( const QString &name, const QString &type, const QString & size);

    QString name() const { return m_name; }
    QString starType() const {return m_starType; }
    QString starSize() const { return m_starSize; }

    QList<Planet> orbits() const{ return m_orbits; }
    int orbitsCount() const { return m_orbits.size(); }
    void addOrbit( const Planet &planet );

    QList<WarpPoint> warpPoints() const { return m_warppoints; }
    int warpPointsCount() const { return m_warppoints.size(); }
    void addWP( const WarpPoint &wp );

    static QList<System> getSystems();
    static System find( const QString &name );

private:
    QString m_name;
    QString m_starType;
    QString m_starSize;

    QList<Planet> m_orbits;
    QList<WarpPoint> m_warppoints;
};

#endif // SYSTEM_H
