#ifndef COMPONENTSPROXYMODEL_H
#define COMPONENTSPROXYMODEL_H

#include <QSortFilterProxyModel>

class ComponentsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ComponentsProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // COMPONENTSPROXYMODEL_H
