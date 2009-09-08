#include "MaterialsModel.h"

#include "ShipDesigner.h"

#include <QDebug>
MaterialsModel::MaterialsModel( QObject *parent )
        : ItemModel( parent )
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

