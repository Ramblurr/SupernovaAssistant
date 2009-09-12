#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QItemDelegate>

class ItemDelegate : public QItemDelegate
{
public:
    ItemDelegate ( QObject * parent = 0 ) : QItemDelegate( parent ) {}
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // ITEMDELEGATE_H
