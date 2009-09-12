#include "MaterialsWidget.h"

#include "data/SNItem.h"

#include <QDebug>
QStringList MaterialsWidget::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.sn.item.list";
    return types;
}

bool MaterialsWidget::dropMimeData( int row, int column, const QMimeData * data, Qt::DropAction action )
{
    qDebug() << "HI!!";
     if (action == Qt::IgnoreAction)
         return true;

     if (!data->hasFormat("application/vnd.sn.item.list"))
         return false;

     if (column > 0)
         return false;

     int beginRow;

     if (row != -1)
         beginRow = row;
     else
         beginRow = rowCount();

     QByteArray encodedData = data->data("application/vnd.sn.item.list");
     QDataStream stream(&encodedData, QIODevice::ReadOnly);
     QList<SNItem> items;
     qDebug() << "got :" << items.size();
     int rows = 0;
     while (!stream.atEnd()) {
         SNItem item;
         qDebug() << item.name();
         stream >> item;
         items << item;
         ++rows;
     }

//     insertRows(beginRow, rows, QModelIndex());
     foreach (SNItem item, items) {
         int row = rowCount();
         insertRow( row );
         QTableWidgetItem *item0 = new QTableWidgetItem( item.name() );
         QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( 1 ) );
         setItem( row, 0, item0 );
         setItem( row, 1, item1 );
//         QModelIndex idx = index(beginRow, 0, QModelIndex());
//         setData(idx, text);
//         beginRow++;
     }

     return true;
}

Qt::DropActions MaterialsWidget::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
