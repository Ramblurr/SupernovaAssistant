#ifndef SYSTEM_H
#define SYSTEM_H

#include "Planet.h"

#include <QString>
#include <QList>
class System
{
public:
    System( const QString &name, const QString &type, const QString & size);

    QString name() const;
    QString starType() const;
    QString starSize() const;

    QList<Planet> orbits() const;
    int orbitsCount() const;

    static QList<System> getSystems();
    static System find( const QString &name );

private:
    QString m_name;
    QString m_starType;
    QString m_starSize;

    QList<Planet> m_orbits;
};

#endif // SYSTEM_H
