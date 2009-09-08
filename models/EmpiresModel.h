#ifndef EMPIRESMODEL_H
#define EMPIRESMODEL_H

#include "data/Empire.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
class EmpiresModel : public QAbstractListModel
{
    Q_OBJECT
public:
    EmpiresModel(const QList<Empire> &items, QObject *parent = 0) : QAbstractListModel(parent), m_items(items) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;

    const static int EmpireRole = 15;

private:
    QList<Empire> m_items;
};

#endif // EMPIRESMODEL_H
