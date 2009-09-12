/*
    File: genericdelegate.cpp

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
#include "GenericDelegate.h"
#include <QModelIndex>


GenericDelegate::~GenericDelegate()
{
    QMapIterator<int, AbstractColumnDelegate*> i(m_delegates);
    while (i.hasNext()) {
        AbstractColumnDelegate *delegate = i.next().value();
        delete delegate;
    }
}


void GenericDelegate::insertColumnDelegate(int column,
                                           AbstractColumnDelegate *delegate)
{
    AbstractColumnDelegate *olddelegate = m_delegates.value(column);
    delete olddelegate;
    m_delegates[column] = delegate;
}


void GenericDelegate::removeColumnDelegate(int column)
{
    if (m_delegates.contains(column)) {
        AbstractColumnDelegate *olddelegate = m_delegates[column];
        delete olddelegate;
        m_delegates.remove(column);
    }
}


void GenericDelegate::paint(QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    AbstractColumnDelegate *delegate = m_delegates.value(index.column());
    if (delegate)
        delegate->paint(painter, option, index);
    else
        QItemDelegate::paint(painter, option, index);
}


QWidget *GenericDelegate::createEditor(QWidget *parent,
            const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    AbstractColumnDelegate *delegate = m_delegates.value(index.column());
    if (delegate)
        return delegate->createEditor(parent, option, index);
    else
        return QItemDelegate::createEditor(parent, option, index);
}


void GenericDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    AbstractColumnDelegate *delegate = m_delegates.value(index.column());
    if (delegate)
        delegate->setEditorData(editor, index);
    else
        QItemDelegate::setEditorData(editor, index);
}


void GenericDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    AbstractColumnDelegate *delegate = m_delegates.value(index.column());
    if (delegate)
        delegate->setModelData(editor, model, index);
    else
        QItemDelegate::setModelData(editor, model, index);
}

