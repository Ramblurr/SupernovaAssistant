#include "ShipDesignsModel.h"

#include "Debug.h"

int ShipDesignsModel::rowCount( const QModelIndex &parent ) const
{
    return m_items.count();
}

QVariant ShipDesignsModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() < 0 || index.row() >= m_items.size() )
        return QVariant();

    if ( role == Qt::DisplayRole )
        return m_items.at( index.row() ).name();
    else if ( role == ShipDesignRole )
        return QVariant::fromValue( m_items.at( index.row() ) );
    else
        return QVariant();
}

QVariant ShipDesignsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal )
        return QString( "Column %1" ).arg( section );
    else
        return QString( "Row %1" ).arg( section );
}

bool ShipDesignsModel::removeRow( int row, const QModelIndex & parent )
{
    debug() << "remove called";
    m_items.removeAt( row );
    return true;
}
