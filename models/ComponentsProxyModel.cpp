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
        // perform a depth-first search to check if any
        // children match the filter
        for(int i = 0; i < num_children; i++)
        {
            if( filterAcceptsRow(i, idx) )
                return true;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
