#include "MaterialsModel.h"

#include "ShipDesigner.h"

#include "Debug.h"

MaterialsModel::MaterialsModel( QObject *parent )
        : QAbstractTableModel( parent )
{
}

Qt::ItemFlags MaterialsModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return 0;

    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void MaterialsModel::item_changed( const SNItem &item, qint64 diff )
{
    if( diff == 0 )
        return;
    QMap<QString, int> comps = item.getComponents();
    QMapIterator<QString, int> it( comps );
    while( it.hasNext() )
    {
        it.next();
        qint64 comp_quantity = it.value() * diff;
        SNItem material = SNItem::getItem( it.key() );
        if( m_hash.contains( material ) )
        {
            int row = m_hash[material];
            int new_value = m_data.at( row ).second + comp_quantity;
            // if the new value is 0 we want to remove it from the model
            if( new_value == 0 )
            {
                beginRemoveRows(QModelIndex(), row, row);
                removeItem(material); // decrement alll m_hash items above row
                m_data.removeAt(row);
                endRemoveRows();
            } else { // the item is still around
                QPair<SNItem, qint64> s( material, new_value );
                m_data.replace( row, s );
                QModelIndex index = this->index(row, 1, QModelIndex());
                emit( dataChanged( index, index ) );
            }
        } else { // we need to insert the item into the model
            int row = m_data.count();
            beginInsertRows(QModelIndex(), row, row);
            QPair<SNItem, qint64> a;
            a.first = material;
            a.second = comp_quantity;
            m_data.append( a );
            m_hash[material] = row;
            endInsertRows();
        }
        item_changed( material, comp_quantity );
    }
}

int MaterialsModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return m_data.size();
}

int MaterialsModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 2;
}

QVariant MaterialsModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= m_hash.size() || index.row() < 0 )
        return QVariant();

    if ( role == Qt::DisplayRole )
    {
        int row = index.row();
        if ( index.column() == 0 )
            return  m_data[row].first.name();
        else if ( index.column() == 1 )
            return  QVariant::fromValue ( m_data[row].second );
    }

    if( role == ComponentRole )
    {
        int row = index.row();
        if ( index.column() == 0 )
            return  QVariant::fromValue( m_data[row].first );
        else
            return  m_data[row].second;
    }
    return QVariant();
}

QVariant MaterialsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal )
    {
        switch ( section )
        {
        case 0:
            return tr( "Item" );

        case 1:
            return tr( "Amt" );

        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool MaterialsModel::removeRows( int position, int rows, const QModelIndex &index )
{

    Q_UNUSED( index );
    beginRemoveRows( QModelIndex(), position, position + rows - 1 );

    for ( int row = 0; row < rows; ++row )
    {
        SNItem item = m_data.at( position ).first;
        qint64 diff = 0 - m_data.at( position ).second;
        qint64 weight = item.weight()*(0 - m_data.at( position ).second);
        emit( statsChanged( diff, weight ) );
        m_data.removeAt( position );
        m_hash.remove( item ); // here it is ok to remove straight from the hash because we remove from m_data too
        emit( componentsChanged( item, diff ) );
    }

    endRemoveRows();
    return true;
}

bool MaterialsModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( index.isValid() && role == Qt::EditRole )
    {
        int row = index.row();
        if ( index.column() == 1 )
        {
            SNItem item = m_data.at(row).first;
            qint64 diff = value.toUInt() - m_data.at( row ).second;
            qint64 weight = item.weight()*(value.toUInt() - m_data.at( row ).second);
            emit( statsChanged( diff, weight ) );
            emit( componentsChanged( item, diff ) );
            QPair<SNItem, qint64> s( m_data.at( row ).first, value.toInt());
            m_data.replace( row, s );

        } else
            return false;
        emit( dataChanged( index, index ) );

        return true;
    }

    return false;
}

QList< QPair<SNItem, qint64> > MaterialsModel::getItems() const
{
    return m_data;
}

bool MaterialsModel::appendData( const SNItem &item, qint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        QPair<SNItem, qint64> a;
        a.first = item;
        a.second = quantity;
        m_data.append( a );
        m_hash[item] = row;
        endInsertRows();
        emit( componentsChanged( item, quantity ) );
        emit( statsChanged( quantity, quantity*item.weight() ) );
        return true;
    }
    return false;
}

bool MaterialsModel::appendOrAlterData( const SNItem &item, qint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        QPair<SNItem, qint64> a;
        a.first = item;
        a.second = quantity;
        m_data.append( a );
        m_hash[item] = row;
        endInsertRows();
        return true;
    } else {
        int row = m_hash[item];
        qint64 quant = quantity + m_data.at( row ).second;
        QPair<SNItem, qint64> s( item, quant );
        m_data.replace( row, s );

        QModelIndex i = index(row, 0);
        QModelIndex i2 = index(row, 1);
//        emit( dataChanged( i2, i2 ) );
    }
    return true;
}

void MaterialsModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1 );
    m_hash.clear();
    m_data.clear();
    endRemoveRows();
}

void MaterialsModel::removeItem( const SNItem & removeme )
{

    int row = -1;
    if( m_hash.contains( removeme) )
        row = m_hash.value( removeme );
    if( row == -1 )
        return;
    QMapIterator<SNItem, qint64> i(m_hash);
    while (i.hasNext())
    {
        i.next();
        if( i.value() > row )
            m_hash.insert( i.key(), i.value() - 1);
    }
    m_hash.remove(removeme);
}

