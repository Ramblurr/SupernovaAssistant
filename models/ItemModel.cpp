#include "ItemModel.h"
#include <QDebug>
#include <QList>
#include <QMimeData>

ItemModel::ItemModel( QObject *parent )
        : QAbstractTableModel( parent )
{
}

int ItemModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return m_data.size();
}

int ItemModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 3;
}

QVariant ItemModel::data( const QModelIndex &index, int role ) const
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
        else if ( index.column() == 2 ) {
            QPair<int, int> p = totalItemsTonnage();
            double tot_tons = p.second;
            double item_tons = m_data[row].second*m_data[row].first.weight(); // Quantity * Tons
            double percent = (item_tons/tot_tons)*100; // item_tonnage / total tonnage = percent tonnage
            return QString::number(percent, 'f', 1) + "%";
        }
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

QVariant ItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
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
        case 2:
            return tr( "Percentage");
        default:
            return QVariant();
        }
    }
    if ( orientation == Qt::Vertical )
    {
        return section + 1; // we don't want it 0 indexed
    }
    return QVariant();
}

bool ItemModel::insertRows( int position, int rows, const QModelIndex &index )
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

bool ItemModel::removeRows( int position, int rows, const QModelIndex &index )
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

bool ItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
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

Qt::ItemFlags ItemModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    if ( index.column() == 0 || index.column() == 2 )
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    return  QAbstractItemModel::flags( index ) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;

}

QStringList ItemModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.sn.item.list";
    return types;
}

bool ItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
     if (action == Qt::IgnoreAction)
         return true;

     if (!data->hasFormat("application/vnd.sn.item.list"))
         return false;

     if (column > 0)
         return false;

     int beginRow;

     if (row != -1)
         beginRow = row;
     else if (parent.isValid())
         beginRow = parent.row();
     else
         beginRow = rowCount(QModelIndex());

     QByteArray encodedData = data->data("application/vnd.sn.item.list");
     QDataStream stream(&encodedData, QIODevice::ReadOnly);
     QList<SNItem> items;
     int rows = 0;
     while (!stream.atEnd()) {
         SNItem item;
         stream >> item;
         items << item;
         ++rows;
     }

//     insertRows(beginRow, rows, QModelIndex());
     foreach (SNItem item, items) {
         appendData( item, 0 );
//         QModelIndex idx = index(beginRow, 0, QModelIndex());
//         setData(idx, text);
//         beginRow++;
     }

     return true;
}

Qt::DropActions ItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QList< QPair<SNItem, quint64> > ItemModel::getItems() const
{
    return m_data;
}

bool ItemModel::appendData( const SNItem &item, quint64 quantity )
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

bool ItemModel::appendOrAlterData( const SNItem &item, quint64 quantity )
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

void ItemModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
    m_hash.clear();
    m_data.clear();
    endRemoveRows();
}

QPair<int, int> ItemModel::totalItemsTonnage() const
{
    int tonnage = 0;
    int total = 0;
    for( int i = 0; i < m_data.size(); i++ )
    {
       total += m_data.at(i).second;
       tonnage += m_data.at(i).first.weight()*m_data.at(i).second;
    }
    return QPair<int, int>(total, tonnage);
}
