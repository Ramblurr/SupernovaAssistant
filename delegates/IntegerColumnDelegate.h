#ifndef POSITIVEINTEGERDELEGATE_H
#define POSITIVEINTEGERDELEGATE_H

#include "GenericDelegate.h"

class IntegerColumnDelegate : public AbstractColumnDelegate
{
public:
    IntegerColumnDelegate(int minimum=0, int maximum=100)
        : AbstractColumnDelegate(), m_minimum(minimum), m_maximum(maximum) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private:
    int m_minimum;
    int m_maximum;
};
#endif // INTEGERCOLUMNDELEGATE_H
