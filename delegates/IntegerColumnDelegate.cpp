#include "IntegerColumnDelegate.h"
#include <QIntValidator>
#include <QSpinBox>
#include <QStyleOptionViewItem>

void IntegerColumnDelegate::paint(QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOption = option;
    myOption.displayAlignment = Qt::AlignRight|Qt::AlignVCenter;
    drawDisplay(painter, myOption, myOption.rect,
                index.model()->data(index, Qt::DisplayRole).toString());
}


QWidget *IntegerColumnDelegate::createEditor(QWidget *parent,
                    const QStyleOptionViewItem &, const QModelIndex &) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(m_minimum, m_maximum);
    editor->installEventFilter(const_cast<IntegerColumnDelegate*>(this));
    return editor;
}


void IntegerColumnDelegate::setEditorData(QWidget *editor,
                                          const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);
    spinbox->setValue(value);
}


void IntegerColumnDelegate::setModelData(QWidget *editor,
                    QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);
    spinbox->interpretText();
    model->setData(index, spinbox->value());
}
