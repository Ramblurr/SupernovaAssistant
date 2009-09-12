#ifndef COMPONENTSTREEVIEW_H
#define COMPONENTSTREEVIEW_H

#include "models/ComponentsModel.h"

#include <QTreeView>
#include <QDebug>
class ComponentsTreeView : public QTreeView
{
public:
    ComponentsTreeView( QWidget * parent = 0 ) : QTreeView( parent ) {}

    virtual int indexRowSizeHint ( const QModelIndex & index ) const
    {
        qDebug() << "hi";
        switch ( index.model()->data(index, SN::TypeRole).toInt() )
        {
        case SN::Category:
            {
                return QTreeView::indexRowSizeHint( index ) + 50;
            }
        default:
            return QTreeView::indexRowSizeHint( index );
        }
    }
};

#endif // COMPONENTSTREEVIEW_H
