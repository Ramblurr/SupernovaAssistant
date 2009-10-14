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
    m_components = other.getComponents();
}

ShipDesign::ShipDesign( const QString &name, const QString &type, const QString &missionclass ) :
        m_name( name ),
        m_type( type ),
        m_class( missionclass )
{}

void ShipDesign::addComponent( const QString &item, quint64 quantity )
{
    if ( quantity <= 0 )
        return;
    m_components.insert( item, quantity );
}

bool ShipDesign::saveDesign()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.prepare( "INSERT INTO designs( dname, type, class )"
                       "VALUES(:dname, :type, :class)" );
        query.bindValue( ":dname", m_name );
        query.bindValue( ":type", m_type );
        query.bindValue( ":class", m_class );
        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        QMapIterator<QString, quint64> it( m_components );
        while( it.hasNext() )
        {
            it.next();
            QSqlQuery query2( db );
            query2.prepare("INSERT INTO designcomp( dname, quantity, item )"
                           "VALUES(?, ?, ?)" );
            query2.bindValue( ":dname", m_name );
            query2.bindValue( ":quantity", QString::number( it.value() ) );
            query2.bindValue( ":item", it.key() );
            query2.exec();
            if ( !query2.exec() )
                qDebug() << query2.executedQuery()<< "\n error: " << query2.lastError();
        }
    }
    else
        QMessageBox::critical( 0, "Database error", "Could not open the database" );

}

bool ShipDesign::deleteDesign()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.prepare( "DELETE FROM designs WHERE dname = :dname" );
        query.bindValue( ":dname", m_name );
        query.exec();
    }

}

QList<ShipDesign> ShipDesign::getDesigns()
{
    QList<ShipDesign> list;
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.exec("SELECT * FROM designs");
        int idxName = query.record().indexOf( "dname" );
        int idxType = query.record().indexOf( "type" );
        int idxClass = query.record().indexOf( "class" );
        while ( query.next() )
        {
            QString name = query.value( idxName ).toString();
            QString type = query.value( idxType ).toString();
            QString mclass = query.value( idxClass ).toString();
            ShipDesign des( name, type, mclass );
            QSqlQuery comp_query( db );
            comp_query.prepare("SELECT * FROM designcomp WHERE dname = :dname" );
            comp_query.bindValue(":dname", name );
            if( !comp_query.exec() )
                qDebug() << comp_query.executedQuery()<< "\n error: " << comp_query.lastError();
            int idxItem = comp_query.record().indexOf( "item" );
            int idxQuan = comp_query.record().indexOf( "quantity" );
            while ( comp_query.next() )
            {
                QString name = comp_query.value( idxItem ).toString();
                quint64 quant = comp_query.value( idxQuan ).toUInt();
                des.addComponent( name, quant );
            }
            list << des;
            comp_query.finish();
        }
        query.finish();
    }
    return list;
}

bool ShipDesign::designExists( const QString &name )
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.exec("SELECT name FROM designs");
        int idxName = query.record().indexOf( "dname" );
        while ( query.next() )
        {
            QString stored_name = query.value( idxName ).toString();
            if( stored_name == name )
                return true;
        }
        query.finish();
    }
    return false;
}

ShipDesign& ShipDesign::operator=( const ShipDesign & other )
{
    if ( this != &other )
    {
        m_name = other.name();
        m_class = other.missionClass();
        m_type = other.type();
        m_components = other.getComponents();
    }
    return *this;
}
