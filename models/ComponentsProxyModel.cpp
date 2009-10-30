#include "ComponentsProxyModel.h"

#include "ComponentsModel.h"
ComponentsProxyModel::ComponentsProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool ComponentsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{

    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    if (sourceModel()->hasChildren(idx))
    {
        int num_children = sourceModel()->rowCount(idx);
        qDebug() << sourceModel()->data(idx, Qt::DisplayRole);
        // perform a depth-first search to check if any
        // children match the filter
        for(int i = 1; i < num_children; ++i)
        {
            if( filterAcceptsRow(i, idx) )
                return true;
        }
    }
    bool test = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);;
    qDebug() << test;
    return test;
}
