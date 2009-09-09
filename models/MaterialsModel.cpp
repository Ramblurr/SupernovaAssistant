#include "MaterialsModel.h"

#include "ShipDesigner.h"

#include <QDebug>

MaterialsModel::MaterialsModel( QObject *parent )
        : QAbstractTableModel( parent )
{
}

Qt::ItemFlags MaterialsModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return Qt::ItemIsEnabled;

    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void MaterialsModel::item_changed( const SNItem &item, quint64 diff )
{
    QMap<QString, int> comps = item.getComponents();
    QMapIterator<QString, int> it( comps );
    while( it.hasNext() )
    {
        it.next();
        quint64 comp_quantity = it.value() * diff;
        SNItem material = ShipDesigner::getItem( it.key() );
        if( m_hash.contains( material ) )
        {
            int row = m_hash[material];
            QPair<SNItem, quint64> s( material, m_data.at( row ).second + comp_quantity );
            m_data.replace( row, s );
            QModelIndex index = this->index(row, 1, QModelIndex());
            emit( dataChanged( index, index ) );
        } else {
            int row = m_data.count();
            beginInsertRows(QModelIndex(), row, row);
            QPair<SNItem, quint64> a;
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

bool MaterialsModel::insertRows( int position, int rows, const QModelIndex &index )
{
    Q_UNUSED( index );
    beginInsertRows( QModelIndex(), position, position + rows - 1 );

    for ( int row = 0; row < rows; row++ )
    {
//        m_hash.insert( position, pair );
    }

    endInsertRows();
    return true;
}

bool MaterialsModel::removeRows( int position, int rows, const QModelIndex &index )
{

    Q_UNUSED( index );
    beginRemoveRows( QModelIndex(), position, position + rows - 1 );

    for ( int row = 0; row < rows; ++row )
    {
        SNItem item = m_data.at( position ).first;
        quint64 diff = 0 - m_data.at( position ).second;
        quint64 weight = item.weight()*(0 - m_data.at( position ).second);
        emit( statsChanged( diff, weight ) );
        m_data.removeAt( position );
        m_hash.remove( item );
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
            quint64 diff = value.toUInt() - m_data.at( row ).second;
            quint64 weight = item.weight()*(value.toUInt() - m_data.at( row ).second);
            emit( statsChanged( diff, weight ) );
            emit( componentsChanged( item, diff ) );
            QPair<SNItem, quint64> s( m_data.at( row ).first, value.toInt());
            m_data.replace( row, s );

        } else
            return false;
        emit( dataChanged( index, index ) );

        return true;
    }

    return false;
}

QList< QPair<SNItem, quint64> > MaterialsModel::getItems() const
{
    return m_data;
}

bool MaterialsModel::appendData( const SNItem &item, quint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        QPair<SNItem, quint64> a;
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

bool MaterialsModel::appendOrAlterData( const SNItem &item, quint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        QPair<SNItem, quint64> a;
        a.first = item;
        a.second = quantity;
        m_data.append( a );
        m_hash[item] = row;
        endInsertRows();
        return true;
    } else {
        int row = m_hash[item];
        quint64 quant = quantity + m_data.at( row ).second;
        QPair<SNItem, quint64> s( item, quant );
        m_data.replace( row, s );

        QModelIndex i = index(row, 0);
        QModelIndex i2 = index(row, 1);
//        emit( dataChanged( i2, i2 ) );
    }
    return true;
}

void MaterialsModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
    m_hash.clear();
    m_data.clear();
    endRemoveRows();
}


