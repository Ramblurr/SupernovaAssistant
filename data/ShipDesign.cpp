#include "ShipDesign.h"
#include <QDesktopServices>
#include <QtSql>
#include <QMessageBox>

ShipDesign::ShipDesign()
{
}

ShipDesign::ShipDesign( const ShipDesign &other )
{
    m_name = other.name();
    m_type = other.type();
    m_class = other.missionClass();
    m_id = other.id();
    m_components = other.getComponents();
}

ShipDesign::ShipDesign( const QString &name, const QString &type, const QString &missionclass, int id ) :
        m_name( name ),
        m_type( type ),
        m_class( missionclass ),
        m_id( id )
{}

void ShipDesign::addComponent( const QString &item, quint64 quantity )
{
    if ( quantity <= 0 )
        return;
    m_components.insert( item, quantity );
}

QSqlDatabase ShipDesign::getDb( const QString &empireId )
{
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    dataLoc.cd( "SNAssistant" );
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString dbPath = dataLoc.absolutePath() + QDir::separator() + "empire_" + empireId + ".sql";
    db.setDatabaseName( dbPath );
    return db;
}

bool ShipDesign::saveDesign( const QString &empireId )
{
    QSqlDatabase db = ShipDesign::getDb( empireId );
    if ( db.open() )
    {
        QString sql = "INSERT INTO shipdesigns( name, type, class ) VALUES( '" + m_name + "', '" + m_type + "', '" + m_class + "')";
        QSqlQuery query =  db.exec( sql );
        m_id = query.lastInsertId().toInt();
        QMapIterator<QString, quint64> it( m_components );
        while( it.hasNext() )
        {
            it.next();
            QString value = "'";
            value.append( QString::number( m_id ) );
            value.append("', '").append(it.key()).append("', '").append(QString::number( it.value() )).append("'");
            sql = "INSERT INTO shipdesigns_lines( design_id, item, quantity ) VALUES( " + value + ")";
            QSqlQuery q = db.exec( sql );
        }
        db.close();
    }
    else
        QMessageBox::critical( 0, "Database error", "Could not open the database" );

}

bool ShipDesign::deleteDesign( const QString &empireId )
{
    QSqlDatabase db = ShipDesign::getDb( empireId );
    if ( db.open() )
    {
        QString sql = "DELETE FROM shipdesigns WHERE design_id = " + QString::number( m_id );
        db.exec( sql );
        sql = "DELETE FROM shipdesigns_lines WHERE design_id = " + QString::number( m_id );
        db.close();
    }

}

QList<ShipDesign> ShipDesign::getDesigns( const QString &empireId )
{
    QList<ShipDesign> list;
    QSqlDatabase db = ShipDesign::getDb( empireId );
    if ( db.open() )
    {
        QString sql = "SELECT design_id, name, type, class FROM shipdesigns";
        QSqlQuery query = db.exec( sql );
        int idxID = query.record().indexOf( "design_id" );
        int idxName = query.record().indexOf( "name" );
        int idxType = query.record().indexOf( "type" );
        int idxClass = query.record().indexOf( "class" );
        while ( query.next() )
        {

            int id = query.value( idxID ).toInt();

            QString name = query.value( idxName ).toString();
            QString type = query.value( idxType ).toString();
            QString mclass = query.value( idxClass ).toString();
            ShipDesign des( name, type, mclass, id );
            sql = "SELECT * FROM shipdesigns_lines WHERE design_id = " + query.value( idxID ).toString();
            QSqlQuery comp_query = db.exec( sql );
            int idxItem = comp_query.record().indexOf( "item" );
            int idxQuan = comp_query.record().indexOf( "quantity" );
            while ( comp_query.next() )
            {
                QString name = comp_query.value( idxItem ).toString();
                quint64 quant = comp_query.value( idxQuan ).toUInt();
                des.addComponent( name, quant );
            }
            list << des;
        }
        db.close();
    }
    return list;
}

ShipDesign& ShipDesign::operator=( const ShipDesign & other )
{
    if ( this != &other )
    {
        m_name = other.name();
        m_class = other.missionClass();
        m_type = other.type();
        m_id = other.id();
        m_components = other.getComponents();
    }
    return *this;
}
