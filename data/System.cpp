#include "System.h"

#include "Planet.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
System::System()
{
}

System::System( const QString &name, const QString &type, const QString & size)
    : m_name( name ), m_starType( type ), m_starSize( size )
{

}

QList<System> System::getSystems()
{
    return QList<System>();
}

System System::find( const QString &name )
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( !db.isOpen() )
        return System();
    QSqlQuery query( db );
    query.prepare("SELECT * FROM systems WHERE sysname = :sname");
    query.bindValue(":sname", name);
    query.exec();

    if( !query.next() )
        return System(); // not found


    query.prepare("SELECT * FROM systemscans WHERE sysname = :sname");
    query.bindValue(":sname", name);
    query.exec();

    System sys;
    sys.m_name = name;
    if( !query.next() )
        return sys;

//    int idxName = query.record().indexOf( "sysname" );
    int idxType= query.record().indexOf( "startype" );
    int idxSize = query.record().indexOf( "starsize" );

//    QString name = query.value( idxName ).toString();
    QString type = query.value( idxType ).toString();
    QString size = query.value( idxSize ).toString();

    sys.m_starSize = size;
    sys.m_starType = type;
    query.finish();


    // Get Orbits

    sys.m_orbits = Planet::getPlanets( name );

    return sys;
}
