#include "ItemDelegate.h"
#include "../models/ComponentsModel.h"
#include <QPainter>
#include <QApplication>
void ItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    switch ( index.model()->data(index, SN::TypeRole).toInt() )
    {
        case SN::Category:
        case SN::SubCategory:
        {
            QStyleOptionButton button;
            button.state |= option.state;
            QRect rect = option.rect;
            QFontMetrics metrics( painter->fontMetrics() );
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            rect = button.fontMetrics.boundingRect( rect, Qt::AlignCenter, text);
            button.rect = option.rect;
            button.text = text;
            QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
            return;
        }
     default:
            break;
    }
    QItemDelegate::paint(painter, option, index);
}

QSize ItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    switch ( index.model()->data(index, SN::TypeRole).toInt() )
    {
        case SN::Category:
        case SN::SubCategory:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QRect rect = option.fontMetrics.boundingRect(option.rect, Qt::AlignCenter,  text );
            rect.setHeight( rect.height() + 5 );
            return rect.size();
        }
     default:
            break;
    }
    return QItemDelegate::sizeHint( option, index );
}
