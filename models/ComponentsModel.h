#ifndef COMPONENTSMODEL_H
#define COMPONENTSMODEL_H

#include "data/SNItem.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
class ComponentsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ComponentsModel(const QList<SNItem> &items, QObject *parent = 0) : QAbstractListModel(parent), m_items(items) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;

    const static int ComponentRole = 15;

private:
    QList<SNItem> m_items;
};

#endif // COMPONENTSMODEL_H
