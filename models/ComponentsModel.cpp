#include "ComponentsModel.h"

#include "data/SNItem.h"
#include <QHash>
#include <QDebug>
#include <QMutableListIterator>
#include <QStringList>
ComponentsModel::ComponentsModel(const QList<SNItem> &items, QObject *parent )
        : QAbstractItemModel(parent)
        , m_items(items)
{
    m_rootItem = new ComponentTreeItem( SN::Root, "" );
    setupModelData();
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
    ComponentTreeItem *parentItem;
    if ( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ComponentTreeItem*> ( parent.internalPointer() );

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

bool ComponentsModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if( !index.isValid() )
        return false;

    if( !value.canConvert<SNItem>()  )
        return false;

    ComponentTreeItem *i = static_cast<ComponentTreeItem*> ( index.internalPointer() );

    if( i->type() != SN::Component )
        return false;

    SNItem old_item = i->data().value<SNItem>();
    SNItem new_item = value.value<SNItem>();

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
}

bool ComponentsModel::insertRows ( int row, int count, const QModelIndex & parent )
{
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
    if( oldcat )
        oldcat->removeChild( i );
    else
        m_rootItem->removeChild( i );
     emit reset();
}

void ComponentsModel::appendItem( const SNItem &item )
{
    if( item.category() == "")
    {
        ComponentTreeItem* c = new ComponentTreeItem( SN::Component, QVariant::fromValue(item), m_rootItem );
        m_rootItem->appendChild(c);
    }
    else if( !m_cats.contains( item.category() ) ) // category not seen before
    {
        ComponentTreeItem* p = new ComponentTreeItem(SN::Category, item.category(), m_rootItem );
        m_rootItem->appendChild( p );
        if( !prepareSubCat( item, p ) )
        {
            ComponentTreeItem* c = new ComponentTreeItem( SN::Component, QVariant::fromValue(item), p );
            p->appendChild( c );
        }
        m_cats.insert( item.category(), p );

    } else if( item.category() != "") { // category created
        ComponentTreeItem* p = m_cats.value( item.category(), 0);
        if( !prepareSubCat( item, p ) )
        {
            ComponentTreeItem* c = new ComponentTreeItem( SN::Component, QVariant::fromValue(item), p );
            p->appendChild( c );
        }
    } else
        qDebug() << "IMPOSSIBLE!";
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

void ComponentsModel::setupModelData()
{
//    m_items = SNItem::getItems();

    foreach(SNItem item, m_items)
    {
        appendItem( item );
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











//////ComponentTreeItem
ComponentTreeItem::ComponentTreeItem( const SN::Type &type, const QVariant &data,  ComponentTreeItem *parent )
        : m_type( type )
        , itemData( data )
        , parentItem( parent )
{}

ComponentTreeItem::~ComponentTreeItem()
{
    qDeleteAll ( childItems );
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

void ComponentTreeItem::setData( const QVariant &data )
{
    itemData = data;
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
