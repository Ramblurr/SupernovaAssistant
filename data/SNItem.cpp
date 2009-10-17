#include "SNItem.h"
#include "ItemEffect.h"

#include <QMap>
#include <QDesktopServices>
#include <QtSql>
#include <QMessageBox>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QXmlStreamWriter>

SNItem::SNItem() : m_components(), m_tons( 0 ), m_structure( 0 )
{
}

SNItem::SNItem( const QString &name, const QString &desc, const QString &category ) : m_name( name ), m_desc( desc ), m_category( category ), m_components(), m_tons( 0 ), m_structure( 0 )
{
}

SNItem::SNItem( const QString &name, const QString &desc, const QString &category, int weight ): m_name( name ), m_desc( desc ), m_tons( weight ), m_category( category ), m_components(), m_structure( 0 )
{
}

SNItem::SNItem( const QString &name, const QString &desc, const QString &category, const QString &subcat, int weight, int structure )
        : m_name( name )
        , m_desc( desc )
        , m_tons( weight )
        , m_category( category )
        , m_subcategory( subcat )
        , m_structure( structure )
        , m_components()
        , m_effects()
{}

SNItem::SNItem( const SNItem &other )
{
    m_name = other.name();
    m_category = other.category();
    m_desc = other.desc();
    m_tons = other.weight();
    m_subcategory = other.subcategory();
    m_structure = other.structure();
    m_components = other.getComponents();
    m_effects = other.getEffects();
}

SNItem& SNItem::operator=( const SNItem &other )
                         {
    if ( this != &other )
    {
        m_name = other.name();
        m_category = other.category();
        m_desc = other.desc();
        m_tons = other.weight();
        m_subcategory = other.subcategory();
        m_structure = other.structure();
        m_components = other.getComponents();
        m_effects = other.getEffects();
    }
    return *this;
}

void SNItem::addComponent( const QString &item, int quantity )
{
    if ( quantity <= 0 )
        return;
    m_components.insert( item, quantity );
}

void SNItem::addComponents( QMap<QString, int> comps )
{
    QMapIterator<QString, int> i(comps);
    while (i.hasNext()) {
        i.next();
        m_components.insert(i.key(), i.value());
    }

}

void SNItem::addEffect( const ItemEffect &effect)
{
    m_effects.push_back( effect );
}

void SNItem::addEffects( QList<ItemEffect> effects )
{
    foreach(ItemEffect eff, effects)
        m_effects.push_back( eff );
}

QMap<QString, int> SNItem::getComponents() const
{
    return m_components;
}

QList<ItemEffect> SNItem::getEffects() const
{
    return m_effects;
}

bool SNItem::operator==(const SNItem &other) const
{
    return ( m_name == other.name() );
}

bool SNItem::saveItem() const
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        if( !db.transaction() )
            return false;
        QSqlQuery query( db );
        query.prepare( "INSERT INTO items( iname, category, subcategory, size, desc, structure )"
                       "VALUES(:iname, :category, :subcategory, :size, :desc, :structure)" );
        query.bindValue( ":iname", m_name );
        query.bindValue( ":category", m_category);
        query.bindValue( ":subcategory", m_subcategory);
        query.bindValue( ":size", m_tons );
        query.bindValue( ":desc", m_desc );
        query.bindValue( ":structure", m_structure );

        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        QMapIterator<QString, int> it( m_components );
        while( it.hasNext() )
        {
            it.next();
            query.prepare( "INSERT INTO itemcomp( iname, quantity, resource )"
                       "VALUES(:iname, :quantity, :resource)" );
            query.bindValue( ":iname", m_name );
            query.bindValue( ":quantity", it.value() );
            query.bindValue( ":resource", it.key() );

            if ( !query.exec() )
                qDebug() << query.executedQuery()<< "\n error: " << query.lastError();
        }

        foreach( ItemEffect effect, m_effects )
        {
            it.next();
            query.prepare( "INSERT INTO itemeffects( iname, effect, value, prettyvalue, counter )"
                       "VALUES(:iname, :effect, :value, :prettyvalue, :counter)" );
            query.bindValue( ":iname", m_name );
            query.bindValue( ":effect", effect.name() );
            query.bindValue( ":value", effect.value() );
            query.bindValue( ":prettyvalue", effect.prettyValue() );
            query.bindValue( ":counter", effect.counter() );

            if ( !query.exec() )
                qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        }
        query.finish();
        if( !db.commit() )
            return false;

    }
    return true;
}

bool SNItem::updateItem( const SNItem &item ) const
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        if( !db.transaction() )
            return false;
        QSqlQuery query( db );
        query.prepare( "UPDATE items SET iname=:iname, category=:category, subcategory=:subcategory, size=:size, desc=:desc, structure=:structure "
                       "WHERE iname = :oldname" );
        query.bindValue( ":iname", item.name());
        query.bindValue( ":category", item.category());
        query.bindValue( ":subcategory", item.subcategory());
        query.bindValue( ":size", item.weight());
        query.bindValue( ":desc", item.desc() );
        query.bindValue( ":structure", item.structure() );
        query.bindValue( ":oldname", m_name );

        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        QMapIterator<QString, int> it( item.getComponents() );
        while( it.hasNext() )
        {
            it.next();
            query.prepare( "UPDATE itemcomp SET iname = :iname, quantity=:quantity, resource=:resource "
                       "WHERE iname = :oldname" );
            query.bindValue( ":iname", item.name() );
            query.bindValue( ":quantity", it.value() );
            query.bindValue( ":resource", it.key() );
            query.bindValue( ":oldname", m_name );

            if ( !query.exec() )
                qDebug() << query.executedQuery()<< "\n error: " << query.lastError();
        }

        foreach( ItemEffect effect, item.getEffects() )
        {
            it.next();
            query.prepare( "UPDATE itemeffects SET iname = :iname, effect=:effect, value=:value, prettyvalue=:prettyvalue, counter=:counter "
                       "WHERE iname = :oldname" );
            query.bindValue( ":iname", item.name() );
            query.bindValue( ":effect", effect.name() );
            query.bindValue( ":value", effect.value() );
            query.bindValue( ":prettyvalue", effect.prettyValue() );
            query.bindValue( ":counter", effect.counter() );
            query.bindValue( ":oldname", m_name );

            if ( !query.exec() )
                qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        }
        query.finish();
        if( !db.commit() )
            return false;

    }
    return true;
}

bool SNItem::deleteItem() const
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        if( !db.transaction() )
            return false;
        QSqlQuery query( db );
        query.prepare( "DELETE FROM items WHERE iname = :iname");
        query.bindValue( ":iname", m_name );

        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        query.prepare( "DELETE FROM itemcomp WHERE iname = :iname");
        query.bindValue( ":iname", m_name );

        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        query.prepare( "DELETE FROM itemeffects WHERE iname = :iname");
        query.bindValue( ":iname", m_name );

        if ( !query.exec() )
            qDebug() << query.executedQuery()<< "\n error: " << query.lastError();

        query.finish();
        if( !db.commit() )
            return false;
    }
    return true;
}

void SNItem::writeToDatabase( const QList<SNItem> &list)
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.exec("DELETE * FROM items");
        foreach(SNItem item, list )
        {
            qDebug() << "Added: " << item.name();
            item.saveItem();
        }
        query.finish();
    }
}

QList<SNItem> SNItem::getItemsFromDatabase()
{
    QList<SNItem> list;
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.exec("SELECT * FROM items");
        int idxName = query.record().indexOf( "iname" );
        int idxCategory = query.record().indexOf( "category" );
        int idxSubCategory = query.record().indexOf( "subcategory" );
        int idxSize = query.record().indexOf( "size" );
        int idxDesc = query.record().indexOf( "desc" );
        int idxStructure = query.record().indexOf( "structure" );
        while ( query.next() )
        {
            QString name = query.value( idxName ).toString();
            QString category = query.value( idxCategory ).toString();
            QString sub_category = query.value( idxSubCategory ).toString();
            QString desc = query.value( idxDesc ).toString();
            int size = query.value( idxSize ).toInt();
            int structure = query.value( idxStructure ).toInt();

            SNItem item( name, desc, category, sub_category, size, structure );

            // Get Components
            QSqlQuery comp_query( db );
            comp_query.prepare("SELECT * FROM itemcomp WHERE iname = :iname" );
            comp_query.bindValue(":iname", name );
            if( !comp_query.exec() )
                qDebug() << comp_query.executedQuery()<< "\n error: " << comp_query.lastError();
            int idxItem = comp_query.record().indexOf( "resource" );
            int idxQuan = comp_query.record().indexOf( "quantity" );
            while ( comp_query.next() )
            {
                QString name = comp_query.value( idxItem ).toString();
                quint64 quant = comp_query.value( idxQuan ).toUInt();
                item.addComponent( name, quant );
            }
            comp_query.finish();

            // Get Effects
            QSqlQuery effect_query( db );
            effect_query.prepare("SELECT * from itemeffects WHERE iname = :iname" );
            effect_query.bindValue(":iname", name );
            if( !effect_query.exec() )
                qDebug() << effect_query.executedQuery()<< "\n error: " << effect_query.lastError();
            int idxEffect = effect_query.record().indexOf( "effect" );
            int idxValue = effect_query.record().indexOf( "value" );
            int idxPrettyValue = effect_query.record().indexOf( "prettyvalue" );
            int idxCounter = effect_query.record().indexOf( "counter" );
            while ( effect_query.next() )
            {
                QString name = effect_query.value( idxEffect ).toString();
                int value = effect_query.value( idxValue ).toInt();
                QString prettyval = effect_query.value( idxPrettyValue ).toString();
                QString counter = effect_query.value( idxCounter ).toString();
                ItemEffect effect( name, value, prettyval, counter );
                item.addEffect( effect );
            }
            effect_query.finish();

            list << item;
        }
        query.finish();
    }
    return list;
}

QList<SNItem> SNItem::getItemsFromXml( const QString &filename)
{
    QList<SNItem> list;
    qDebug() << "loading items";
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "file open failed";
        return list;
    }
    QDomDocument doc( "items" );
    QString msg;
    int line;
    if ( !doc.setContent( &file, false, &msg, &line ) )
    {
        qDebug() << "setting doc failed";
        qDebug() << msg;
        qDebug() << line;
        file.close();
        return list;
    }
    QDomElement root = doc.documentElement();
    if ( root.tagName() != "items" )
        return list;

    QDomNode n = root.firstChild();
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == "item" )
        {
            QString name = n.firstChildElement( "name" ).text();
            QString desc = n.firstChildElement( "desc" ).text();
            QString weight = n.firstChildElement( "weight" ).text();
            QString category = n.firstChildElement( "category" ).text();
            QString subcategory = n.firstChildElement( "subcategory" ).text();
            QString structure = n.firstChildElement( "structure" ).text();
            if( weight == "" )
            {
                n = n.nextSibling();
                continue;
            }
            bool ok;
            SNItem item = SNItem( name, desc, category, subcategory, weight.toInt( &ok ), structure.toInt(&ok) );
            if ( !ok )
            {
                n = n.nextSibling();
                continue;
            }

            QDomNode n_comps = n.firstChildElement( "components" ).firstChild();
            while ( !n_comps.isNull() )
            {
                QDomElement e = n_comps.toElement();
                if ( !e.isNull() && e.tagName() == "component" )
                {
                    QString compname = e.text();
                    int quantity = e.attribute( "quantity", "0" ).toInt( &ok );
                    if ( ok )
                    {

                        item.addComponent( compname, quantity );
                    }
                    else
                        qWarning() << "Error parsing component '" << quantity << "' for item '" << name << "'";
                }
                n_comps = n_comps.nextSibling();
            }

            if ( ok )
                list << item;
        }
        n = n.nextSibling();
    }
    return list;
}

SNItem SNItem::getItem( const QString &name )
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );
        query.prepare("SELECT * FROM items WHERE iname = :iname");
        query.bindValue(":iname", name);
        query.exec();
        query.next();

        int idxName = query.record().indexOf( "iname" );
        int idxCategory = query.record().indexOf( "category" );
        int idxSubCategory = query.record().indexOf( "subcategory" );
        int idxSize = query.record().indexOf( "size" );
        int idxDesc = query.record().indexOf( "desc" );
        int idxStructure = query.record().indexOf( "structure" );

        QString name = query.value( idxName ).toString();
        QString category = query.value( idxCategory ).toString();
        QString sub_category = query.value( idxSubCategory ).toString();
        QString desc = query.value( idxDesc ).toString();
        int size = query.value( idxSize ).toInt();
        int structure = query.value( idxStructure ).toInt();

        query.finish();

        SNItem item( name, desc, category, sub_category, size, structure );

        // Get Components
        QSqlQuery comp_query( db );
        comp_query.prepare("SELECT * FROM itemcomp WHERE iname = :iname" );
        comp_query.bindValue(":iname", name );
        if( !comp_query.exec() )
            qDebug() << comp_query.executedQuery()<< "\n error: " << comp_query.lastError();
        int idxItem = comp_query.record().indexOf( "resource" );
        int idxQuan = comp_query.record().indexOf( "quantity" );
        while ( comp_query.next() )
        {
            QString name = comp_query.value( idxItem ).toString();
            quint64 quant = comp_query.value( idxQuan ).toUInt();
            item.addComponent( name, quant );
        }
        comp_query.finish();

        // Get Effects
        QSqlQuery effect_query( db );
        effect_query.prepare("SELECT * from itemeffects WHERE iname = :iname" );
        effect_query.bindValue(":iname", name );
        if( !effect_query.exec() )
            qDebug() << effect_query.executedQuery()<< "\n error: " << effect_query.lastError();
        int idxEffect = effect_query.record().indexOf( "effect" );
        int idxValue = effect_query.record().indexOf( "value" );
        int idxPrettyValue = effect_query.record().indexOf( "prettyvalue" );
        int idxCounter = effect_query.record().indexOf( "counter" );
        while ( effect_query.next() )
        {
            QString name = effect_query.value( idxEffect ).toString();
            int value = effect_query.value( idxValue ).toInt();
            QString prettyval = effect_query.value( idxPrettyValue ).toString();
            QString counter = effect_query.value( idxCounter ).toString();
            ItemEffect effect( name, value, prettyval, counter );
            item.addEffect( effect );
        }
        effect_query.finish();


        return item;
    }
    return SNItem();
}

bool SNItem::createXML( const QList<SNItem> & items, const QString &filename )
{
    QString output;
    QFile *file = new QFile(filename);
    if ( !file->open( QIODevice::WriteOnly ) )
    {
        qDebug() << "file open failed";
        return false;
    }
    QXmlStreamWriter stream( file );
    stream.setAutoFormatting(true);
    stream.setAutoFormattingIndent(1);
    stream.writeStartDocument("1.0");

    stream.writeStartElement("items");

    foreach(SNItem item, items)
    {
        stream.writeStartElement("item");

        stream.writeTextElement("name", item.name());

        stream.writeStartElement("desc");
        stream.writeCDATA(item.desc());
        stream.writeEndElement(); //desc

        if( item.weight() > 0 )
        {
            stream.writeTextElement("weight", QString::number( item.weight() ) );
        }

        stream.writeTextElement("category", item.category());

        if( item.subcategory() != "" )
        {
            stream.writeTextElement("subcategory", item.subcategory());
        }

        if( item.structure() > 0 )
        {
            stream.writeTextElement("structure", QString::number( item.structure() ) );
        }

        if( item.getComponents().size() > 0 )
        {
            stream.writeStartElement("components");
            QMapIterator<QString, int> it( item.getComponents() );
            while( it.hasNext() )
            {
                it.next();
                stream.writeStartElement("component");
                stream.writeAttribute("quantity", QString::number( it.value() ) );
                stream.writeCharacters(it.key());
                stream.writeEndElement(); //component
            }
            stream.writeEndElement(); //components
        }

        if( item.getEffects().size() > 0 )
        {
            stream.writeStartElement("effects");
            foreach( ItemEffect effect, item.getEffects() )
            {
                stream.writeStartElement("effect");
                stream.writeAttribute( "value", QString::number( effect.value() ) );
                stream.writeAttribute( "prettyvalue", effect.prettyValue() );
                stream.writeAttribute( "counters", effect.counter() );
                stream.writeCharacters( effect.name() );
                stream.writeEndElement(); //effect
            }
            stream.writeEndElement(); //effects
        }

        stream.writeEndElement(); //item
    }
    stream.writeEndElement(); //items

    stream.writeEndDocument();

    file->close();
    return true;
}
