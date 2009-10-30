#include "ComponentsModel.h"

#include "data/SNItem.h"
#include <QHash>
#include <QDebug>
#include <QMutableListIterator>
#include <QStringList>
#include <QtSql>
ComponentsModel::ComponentsModel(QObject *parent, const QString &category_filter )
        : QAbstractItemModel(parent), m_catfilter( category_filter )
{
    m_rootItem = new ComponentTreeItem( SN::Root, "" );
    m_rootItem->setQuery( "SELECT count(DISTINCT category) FROM items" );
    fetchMore( QModelIndex() );
}

QModelIndex ComponentsModel::index ( int row, int column, const QModelIndex & parent ) const
{
    if ( !hasIndex ( row, column, parent ) )
        return QModelIndex();

    ComponentTreeItem *parentItem;

    if ( !parent.isValid() )
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ComponentTreeItem*> ( parent.internalPointer() );

    ComponentTreeItem *childItem = parentItem->child ( row );
    if ( childItem )
        return createIndex ( row, column, childItem );
    else
        return QModelIndex();
}

bool ComponentsModel::hasChildren ( const QModelIndex & parent ) const
{
    return  rowCount(parent) != 0;
}

QModelIndex ComponentsModel::parent ( const QModelIndex & index ) const
{
    if ( !index.isValid() )
        return QModelIndex();
    ComponentTreeItem *childItem = static_cast<ComponentTreeItem*> ( index.internalPointer() );
    ComponentTreeItem *parentItem = childItem->parent();

    if ( parentItem == m_rootItem )
        return QModelIndex();

    return createIndex ( parentItem->row(), 0, parentItem );
}

Qt::ItemFlags ComponentsModel::flags(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return 0;
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( index.internalPointer() );
    switch ( i->type() )
    {
        case SN::Category:
        case SN::SubCategory:
            break;
        case SN::Component:
            flags |= Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
            break;
        default:
            break;
    }
    return flags;
}

int ComponentsModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent.column() > 0 )
        return 0;

    ComponentTreeItem *parentItem;
    if ( !parent.isValid() )
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ComponentTreeItem*> ( parent.internalPointer() );

    if( parentItem->type() == SN::Component )
        return 0;

    if( parentItem->childCount() == 0)
    {
        QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
        if ( db.isOpen() )
        {
            QSqlQuery query( db );
            query.exec( parentItem->query() );
            query.next();
            return query.value(0).toInt();
        }
    }
    return parentItem->childCount();
}

QVariant ComponentsModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( index.internalPointer() );

    if ( role == Qt::DisplayRole )
        switch ( i->type() )
        {
            case SN::Category:
            case SN::SubCategory:
                return i->data();
            case SN::Component:
                return i->data().value<SNItem>().name();
            default:
                return QVariant();
        }

    else if ( role == SN::ComponentRole )
        switch ( i->type() )
        {
            case SN::Component:
                return i->data();
            default:
                return QVariant();
        }
    else if ( role == SN::TypeRole )
        return i->type();
    else
        return QVariant();
}

QVariant ComponentsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( section )
    Q_UNUSED( orientation )
    Q_UNUSED( role )
//    if ( role != Qt::DisplayRole )
        return QVariant();

//    if ( orientation == Qt::Horizontal )
//        return QString( "Column %1" ).arg( section );
//    else
//        return QString( "Row %1" ).arg( section );

}

int ComponentsModel::columnCount ( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    return 1;
}

bool ComponentsModel::canFetchMore ( const QModelIndex & parent ) const
{
    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( parent.internalPointer() );
    if( i != 0 ) {
        qDebug() << i->data().toString() << i->loaded();
        return !i->loaded();
    }
    return false;
}

void ComponentsModel::fetchMore ( const QModelIndex & parent )
{
    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( parent.internalPointer() );
    if( i == 0 )
        i = m_rootItem;
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        QSqlQuery query( db );

        QList<ComponentTreeItem*> newitems;

        switch( i->type() )
        {
            case SN::Root:
            {
                QString sql = "SELECT DISTINCT category FROM items";
                if( m_catfilter != "" )
                    sql += " WHERE category = '" + m_catfilter + "'";
                query.exec( sql );
                int idxCategory = query.record().indexOf( "category" );
                while ( query.next() )
                {
                    QString cat = query.value( idxCategory ).toString();
                    ComponentTreeItem* child = new ComponentTreeItem( SN::Category, cat, i );
                    m_cats.insert(cat, child);
                    child->setQuery( "SELECT count(*) FROM items WHERE category = '" + cat + "'");
                    newitems  << child;
                }
                break;
            }
        case SN::Category:
            {
                query.exec( "SELECT DISTINCT subcategory FROM items WHERE category = '" + i->data().toString() + "'" );
                int idxSubCategory = query.record().indexOf( "subcategory" );
                while ( query.next() )
                {
                    QString subcat = query.value( idxSubCategory ).toString();
                    if( subcat.trimmed().length() == 0 )
                        continue;
                    ComponentTreeItem* child = new ComponentTreeItem( SN::SubCategory, subcat, i );
                    m_subcats.insert(subcat, child);
                    child->setQuery("SELECT count(*) FROM items WHERE subcategory = '" + subcat + "'");
                    newitems << child;
                }

                query.exec( "SELECT iname FROM items WHERE category = '" + i->data().toString() + "' AND (subcategory IS NULL OR subcategory = '')" );
                int idxName = query.record().indexOf( "iname" );
                qDebug() << query.lastQuery();
                while ( query.next() )
                {
                    QString name = query.value( idxName ).toString();
                    SNItem item = SNItem::getItem( name );
                    ComponentTreeItem* child = new ComponentTreeItem( SN::Component, item, i );
                    newitems << child;
                }
                break;
            }
        case SN::SubCategory:
            {
                query.exec( "SELECT * FROM items WHERE subcategory = '" + i->data().toString() + "'" );
                int idxName = query.record().indexOf( "iname" );
                qDebug() << query.lastQuery();
                while ( query.next() )
                {
                    QString name = query.value( idxName ).toString();
                    SNItem item = SNItem::getItem( name );
                    ComponentTreeItem* child = new ComponentTreeItem( SN::Component, item, i );
                    newitems << child;
                }
                break;
            }
        default:
            break;
        }

        QList<ComponentTreeItem*> olditems = i->children();


        if( olditems.size() != newitems.size() )
        {
//            beginInsertRows( parent, 0, newitems.size() );
            i->setChildren( newitems );
//            endInsertRows();
        }
        else {
            i->setChildren( newitems );
            for(int i = 0; i < olditems.size(); i++ )
            {
                if( olditems.at(i)->data() != newitems.at(i)->data() )
                    emit( dataChanged( parent.child( i,0 ), parent.child( i, 0 ) ) );
            }
        }
        i->setLoaded( true );
    }
}

bool ComponentsModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    Q_UNUSED( role )
    if( !index.isValid() )
        return false;

    if( !value.canConvert<SNItem>()  )
        return false;

    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( index.internalPointer() );

    if( i->type() != SN::Component )
        return false;

    SNItem old_item = i->data().value<SNItem>();
    SNItem new_item = value.value<SNItem>();
    old_item.updateItem( new_item );

    //Check to see if we need to change the category
    // and hence change the tree structure.
    if( new_item.category() != old_item.category() )
    {
        //First, create a new item with the new data under the correct category
        appendItem( new_item );

        //Second, remove the old data
        ComponentTreeItem* oldcat = m_cats.value(old_item.category());
        if( oldcat )
            oldcat->removeChild( i );
        else
            m_rootItem->removeChild( i );
    }

    i->setData( value );
//    emit dataChanged( index, index );
    emit ( reset() );
    return true;
}

bool ComponentsModel::insertRows ( int row, int count, const QModelIndex & parent )
{
    Q_UNUSED( row )
    Q_UNUSED( count )
    Q_UNUSED( parent )
    return false;
}

QStringList ComponentsModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.sn.item.list";
    return types;
}

QMimeData* ComponentsModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            SNItem item = data(index, SN::ComponentRole).value<SNItem>();
            qDebug() << "added " << item.name();
            stream << item;
        }
    }

    mimeData->setData("application/vnd.sn.item.list", encodedData);
    return mimeData;
}

bool ComponentsModel::removeItem ( const QModelIndex & index )
{
    if( !index.isValid() )
        return false;

    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( index.internalPointer() );

    if( i->type() != SN::Component )
        return false;

    SNItem old_item = i->data().value<SNItem>();
    //Second, remove the old data
    ComponentTreeItem* oldcat = m_cats.value(old_item.category());
    old_item.deleteItem();
    if( oldcat )
        oldcat->removeChild( i );
    else
        m_rootItem->removeChild( i );
     emit reset();
    return true;
}

void ComponentsModel::appendItem( const SNItem &item )
{
    if( item.category() == "")
    {
        ComponentTreeItem* c = new ComponentTreeItem( SN::Component, item, m_rootItem );
        m_rootItem->appendChild(c);
    }
    else if( !m_cats.contains( item.category() ) ) // category not seen before
    {
        ComponentTreeItem* p = new ComponentTreeItem(SN::Category, item.category(), m_rootItem );
        m_rootItem->appendChild( p );
        if( !prepareSubCat( item, p ) )
        {
            ComponentTreeItem* c = new ComponentTreeItem( SN::Component, item, p );
            p->appendChild( c );
        }
        m_cats.insert( item.category(), p );

    } else if( item.category() != "") { // category created
        ComponentTreeItem* p = m_cats.value( item.category(), 0);
        if( !prepareSubCat( item, p ) )
        {
            ComponentTreeItem* c = new ComponentTreeItem( SN::Component, item, p );
            p->appendChild( c );
        }
    } else
        qDebug() << "IMPOSSIBLE!";
    item.saveItem();
    emit( reset() );
}

QList<SNItem> ComponentsModel::getItems() const
{
    ComponentTreeItem* top_idx = m_rootItem;
    QList<SNItem> list;
    getItemsRecursive( top_idx, list );
    return list;
}

void ComponentsModel::getItemsRecursive( const ComponentTreeItem *parent, QList<SNItem> &list ) const
{
    if( parent->type() == SN::Component ) {
        SNItem item = parent->data().value<SNItem>();
        list << item;
    }

    foreach( ComponentTreeItem* idx, parent->children() )
    {
        if( idx )
            getItemsRecursive( idx, list );
    }
}

bool ComponentsModel::prepareSubCat( const SNItem &item, ComponentTreeItem *parent )
{
    if( item.subcategory() != "" )
    {
        if( !m_subcats.contains( item.subcategory() ) )
        {
            ComponentTreeItem* s = new ComponentTreeItem( SN::Category, item.subcategory(), parent );
            ComponentTreeItem* c = new ComponentTreeItem( SN::Component, QVariant::fromValue( item ), s );
            m_subcats.insert( item.subcategory(), s );
            s->appendChild( c );
            parent->appendChild( s );
        } else { // subcat already created
            ComponentTreeItem* s = m_subcats.value( item.subcategory(), 0 );
            ComponentTreeItem* c = new ComponentTreeItem(SN::Component, QVariant::fromValue( item ), s );
            s->appendChild( c );
        }
        return true;
    }
    return false;
}

void
ComponentsModel::emitRowChanged( int parent_row, int child_row )
{
    QModelIndex parent;
    if (child_row != -1)
    parent = index( parent_row, 0 );

    QModelIndex i = index( child_row, 0, parent );

    emit dataChanged( i, i );
}


void ComponentsModel::clear()
{
    SNItem::clearDatabase();
    m_rootItem->clearAllChildren();
    reset();
}







//////ComponentTreeItem
ComponentTreeItem::ComponentTreeItem( const SN::Type &type, const QVariant &data,  ComponentTreeItem *parent )
        : m_type( type )
        , itemData( data )
        , parentItem( parent )
        , m_loaded( false )
{}

ComponentTreeItem::~ComponentTreeItem()
{
    qDeleteAll ( childItems );
}

void ComponentTreeItem::clearAllChildren()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void ComponentTreeItem::appendChild ( ComponentTreeItem *item )
{
    childItems.append ( item );
}

bool ComponentTreeItem::removeChild(  ComponentTreeItem *child )
{
    QMutableListIterator<ComponentTreeItem*> it( childItems );
    while( it.hasNext() )
    {
        ComponentTreeItem* c = it.next();
        if( c == child ) {
            it.remove();
            delete c;
            return true;
        }
    }
    return false;
}

void ComponentTreeItem::setChildren( QList<ComponentTreeItem*> children )
{
    childItems = children;
}

void ComponentTreeItem::setData( const QVariant &data )
{
    itemData = data;
}

void ComponentTreeItem::setQuery( const QString &query )
{
    m_query = query;
}

 void ComponentTreeItem::setLoaded( bool loaded )
 {
     m_loaded = loaded;
 }

ComponentTreeItem *ComponentTreeItem::child ( int row )
{
    return childItems.value ( row );
}

QList<ComponentTreeItem*> ComponentTreeItem::children() const
{
    return childItems;
}

int ComponentTreeItem::childCount() const
{
    return childItems.count();
}

int ComponentTreeItem::columnCount() const
{
    return 1;
}

QVariant ComponentTreeItem::data () const
{
    return itemData;
}

bool ComponentTreeItem::loaded () const
{
    return m_loaded;
}

ComponentTreeItem *ComponentTreeItem::parent()
{
    return parentItem;
}

int ComponentTreeItem::row() const
{
    if ( parentItem )
        return parentItem->childItems.indexOf ( const_cast<ComponentTreeItem*> ( this ) );

    return 0;
}
