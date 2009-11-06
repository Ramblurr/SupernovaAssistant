#include "ItemModel.h"

#include "SNConstants.h"

#include <QDebug>
#include <QList>
#include <QMimeData>

ItemModel::ItemModel( QObject *parent )
        : QAbstractTableModel( parent )
{
    connect(this, SIGNAL( statsChanged(int,qint64) ), this, SLOT( refreshStats()) );
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
            return  m_data[row].item.name();
        else if ( index.column() == 1 )
            return  QVariant::fromValue ( m_data[row].quantity );
        else if ( index.column() == 2 )
        {
            // item_tonnage / total tonnage = percent tonnage
            QPair<int, int> p = totalItemsTonnage();
            double tot_tons = p.second;
            double item_tons = m_data[row].quantity*m_data[row].item.weight(); // Quantity * Tons
            double percent = 0;
            if( tot_tons != 0)
                percent = (item_tons/tot_tons)*100;
            return QString::number(percent, 'f', 1) + "%";
        }
        else if( index.column() == 3)
            return QVariant();
    }

    if( role == Qt::CheckStateRole )
    {
        int row = index.row();
        if ( index.column() == 3 )
        {
            return m_data[row].lockpercentage ? Qt::Checked : Qt::Unchecked;
        }
    }

    if( role == SN::ComponentRole )
    {
        int row = index.row();
        return  QVariant::fromValue( m_data[row].item );
//        else
//            return  m_data[row].quantity;
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
            return tr( "Amount" );
        case 2:
            return tr( "Tonnage Percent");
        case 3:
            return tr( "Edit Percent");
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
        SNItem item = m_data.at( position ).item;
        qint64 diff = 0 - m_data.at( position ).quantity;
        qint64 weight = item.weight()*(0 - m_data.at( position ).quantity);
        m_data.removeAt( position );
        m_hash.remove( item );
        emit( statsChanged( diff, weight ) );
        emit( componentsChanged( item, diff ) );
    }

    endRemoveRows();
    return true;
}

bool ItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( !index.isValid()  )
        return false;
    if ( role == Qt::EditRole )
    {
        int row = index.row();
        if ( index.column() == 1 )
        {
            SNItem item = m_data.at(row).item;
            qint64 curr_num_of_item = m_data.at( row ).quantity;
            qint64 new_num_of_item = value.toLongLong();
            qint64 diff = new_num_of_item - curr_num_of_item;
            qint64 tondiff = (new_num_of_item - curr_num_of_item)*item.weight();
            ItemEntry s = m_data.at( row );
            s.quantity = value.toLongLong();
            m_data.replace( row, s );
            emit( statsChanged( diff, tondiff ) );
            emit( componentsChanged( item, diff ) );
        }
        else if ( index.column() == 2 )
        {
            // quantity = (percent_tonnage * total(minus item) tonnage) / item_weight*(1-percent_tonnage)
            SNItem item = m_data.at(row).item;
            int item_weight = item.weight();
            int total_tonnage = totalItemsTonnage().second - (item_weight*m_data.at( row ).quantity);
            double percent_tonnage = (value.toDouble() / 100);

            int new_quantity = 0;
            if( item_weight != 0 )
                new_quantity = ( percent_tonnage * total_tonnage ) / (item_weight*(1-percent_tonnage));



            qint64 curr_num_of_item = m_data.at( row ).quantity;
            qint64 new_num_of_item = new_quantity;
            qint64 diff = new_num_of_item - curr_num_of_item;
            qint64 tondiff = new_num_of_item - curr_num_of_item;
            qint64 weight = item.weight()*tondiff;

            ItemEntry s = m_data.at( row );
            s.quantity = new_quantity;
            m_data.replace( row, s );

            emit( statsChanged( diff, weight ) );
            emit( componentsChanged( item, diff ) );

        } else
            return false;
        emit( dataChanged( index, index ) );

        return true;
    }

    if ( role == Qt::CheckStateRole )
    {
        int row = index.row();
        if( index.column() == 3)
        {
            Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
            ItemEntry s = m_data.at( row );
            if( state == Qt::Checked )
                s.lockpercentage = true;
            else
                s.lockpercentage = false;
            m_data.replace( row, s );
            emit( dataChanged( index, index ) );
        }
    }

    return false;
}

Qt::ItemFlags ItemModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return 0;

    if ( index.column() == 0 )
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    else if ( index.column() == 3 )
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable ;

//    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
//    if( index.column() == 1 )
//    {
//        if( m_data[index.row()].lockpercentage )
//            return flags;
//        else
//            return flags | Qt::ItemIsEditable ;
//    } else if( index.column() == 2 )
//    {
//        if( m_data[index.row()].lockpercentage )
//            return flags | Qt::ItemIsEditable ;
//        else
//            return flags;
//    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

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

QList< ItemEntry > ItemModel::getItems() const
{
    return m_data;
}

bool ItemModel::appendData( const SNItem &item, qint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        ItemEntry a;
        a.item = item;
        a.quantity = quantity;
        m_data.append( a );
        m_hash[item] = row;
        endInsertRows();
        emit( componentsChanged( item, quantity ) );
        emit( statsChanged( quantity, quantity*item.weight() ) );
        return true;
    }
    return false;
}

bool ItemModel::appendOrAlterData( const SNItem &item, qint64 quantity )
{
    if( !m_hash.contains( item ) )
    {
        int row = m_data.count();
        beginInsertRows(QModelIndex(), row, row);
        ItemEntry a;
        a.item = item;
        a.quantity = quantity;
        a.lockpercentage = false;
        m_data.append( a );
        m_hash[item] = row;
        endInsertRows();
        return true;
    } else {
        int row = m_hash[item];
        qint64 curr_num_of_item = m_data.at( row ).quantity;
        qint64 new_num_of_item = quantity;
        qint64 diff = new_num_of_item - curr_num_of_item;
        qint64 tondiff = new_num_of_item - curr_num_of_item;
        qint64 weight = item.weight()*tondiff;
        ItemEntry s = m_data.at( row );
        s.quantity = quantity;
        s.lockpercentage = m_data.at( row ).lockpercentage;
        m_data.replace( row, s );
        emit( statsChanged( diff, weight ) );
        emit( componentsChanged( item, diff ) );

        reset();
    }
    return true;
}

void ItemModel::clear()
{
    if( rowCount( QModelIndex() ) == 0 )
        return;
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1);
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
       total += m_data.at(i).quantity;
       tonnage += m_data.at(i).item.weight()*m_data.at(i).quantity;
    }
    return QPair<int, int>(total, tonnage);
}

int ItemModel::actionPoints() const
{
    // AP = total thrust / total tonnage

    int thr = thrust();
    if( thr == 0 ) // no engines
        return 0;

    int tonnage = totalItemsTonnage().second;

    if( tonnage > 0 )
    {
        int val = thr / tonnage;
        if( val >= 2 )
            return val;
        else
            return 2;
    }
    qDebug () << "WARNING! Impossible state reached: ship tonnage = 0, but thrust > 0";
    return 0;
}

double ItemModel::warpBubble() const
{
    // Warp bubble = tonnage/ warp rating
    int wr = warpRating();
    int tons = totalItemsTonnage().second;
    double bub = 0;
    if( wr != 0 )
        bub = tons / wr;
    return bub;
}

int ItemModel::warpRating() const
{
    // Warp rating = JumpDrive1*quant1 + JumpDrive2*quant2 ... + JumpDrive_n*quant_n
    return m_warp;
}

int ItemModel::thrust() const
{
    return m_thrust;
}

int ItemModel::massThrustRatio() const
{
    // thrust / tonnage
    int thr = thrust();
    int tons = totalItemsTonnage().second;
    if( tons != 0 )
        return thr / tons;
    return 0;

}

void ItemModel::refreshStats()
{
    m_thrust = 0;
    m_warp = 0;
    foreach( ItemEntry entry, m_data )
    {
       SNItem item = entry.item;
       qint64 quantity = entry.quantity;

       foreach( ItemEffect effect, item.getEffects() )
       {
           if( item.subcategory() == SN::Category::Engine )
           {
               if( effect.name() == "Maneuverability" )
                   m_thrust += quantity * effect.value();
           }
           else if( item.subcategory() == SN::Category::JumpDrive )
           {
                if( effect.name() == "Jump Drive Output" )
                    m_warp += quantity * effect.value();
           }

       }
    }
}
