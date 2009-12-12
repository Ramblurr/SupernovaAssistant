#include "WarpPoint.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

WarpPoint::WarpPoint()
{
}
WarpPoint::WarpPoint( int idnum, const QString &sysname, const QString &classtype, const float orbdistance )
    : m_id( idnum )
    , m_sysname( sysname )
    , m_class( classtype )
    , m_orbdistance( orbdistance )
{
}

void WarpPoint::setDestinationSystem( const QString &destsys )
{
    m_destinationsys = destsys;
}

bool WarpPoint::operator==(const WarpPoint &other) const
{
    return m_class == other.warpClass() &&
            m_destinationsys == other.destinationSystem() &&
            m_id == other.warpId() &&
            m_orbdistance == other.orbDistance() &&
            m_sysname == other.systemName();
}

QList<WarpPoint> WarpPoint::getWPs( const QString & systemname )
{
    QList<WarpPoint> wps;

    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( !db.isOpen() )
        return wps;

    // Data from System Scan
    QSqlQuery query( db );
    if( !systemname.isEmpty() )
    {
        query.prepare("SELECT * FROM warppoints WHERE sysname = :sysname");
        query.bindValue(":sysname", systemname);
    }
    else
        query.prepare("SELECT * FROM warppoints");

    query.exec();

    while( query.next() )
    {
        WarpPoint wp;

        int idxId = query.record().indexOf( "id" );
        int idxSysName = query.record().indexOf( "sysname" );
        int idxOrbDist = query.record().indexOf( "orbdist" );
        int idxClass = query.record().indexOf( "class" );
        int idxDestSys = query.record().indexOf( "destsys" );

        int idnum = query.value( idxId ).toInt();
        QString sysname = query.value( idxSysName ).toString();
        float orbdist = query.value( idxOrbDist).toDouble();
        QString classtype = query.value( idxClass ).toString();
        QString destsys = query.value( idxDestSys ).toString();

        wp.m_id = idnum;
        wp.m_sysname = sysname;
        wp.m_orbdistance = orbdist;
        wp.m_class = classtype;
        wp.m_destinationsys = destsys;

        wps.append(wp);
    }
    return wps;
}
