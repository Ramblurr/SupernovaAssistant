/*
    File: genericdelegate.hpp

    Copyright (c) 2006 Qtrac Ltd. All rights reserved.

    This file is part of the Qtrac Ltd. set of training materials.

    This file may be used under the terms of the GNU General Public
    License version 2.0 as published by the Free Software Foundation or
    under the terms of the Qt Commercial License Agreement. The
    respective license texts for these are provided with the open source
    and commercial editions of Qt.

    This file is provided as is with no warranty of any kind. In
    particular, it has no warranty of design, merchantability, or
    fitness for a particular purpose.
*/
#ifndef GENERICDELEGATE_HPP
#define GENERICDELEGATE_HPP

#include <QItemDelegate>
#include <QMap>

class QModelIndex;


class AbstractColumnDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    AbstractColumnDelegate(QObject *parent=0) : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
        { QItemDelegate::paint(painter, option, index); }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const = 0;
    void setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const = 0;
};


class GenericDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    GenericDelegate(QObject *parent=0) : QItemDelegate(parent) {}
    ~GenericDelegate();

    void insertColumnDelegate(int column, AbstractColumnDelegate *delegate);
    void removeColumnDelegate(int column);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private:
    QMap<int, AbstractColumnDelegate*> m_delegates;
};

#endif // GENERICDELEGATE_HPP
