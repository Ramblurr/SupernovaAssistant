#include "ComponentsProxyModel.h"

#include "ComponentsModel.h"
ComponentsProxyModel::ComponentsProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool ComponentsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{

    // search children first
    // perform a depth-first search to check if any
    // children match the filter
    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    if (sourceModel()->hasChildren(idx))
    {
        int num_children = sourceModel()->rowCount(idx);

        int i = 0;
        if( !idx.isValid() ) // skip the first child of the root index, or an infinite loop will result
            i = 1;
        for(; i < num_children; ++i)
        {
            if( filterAcceptsRow(i, idx) )
            {
                return true;
            }
        }
    }

    // if the parent category matches, we want to show the children
    // e.g. so you can filter for "weapon" and have all the weapons expanded
    if( QSortFilterProxyModel::filterAcceptsRow(sourceParent.row(), sourceParent.parent()) )
        return true;

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
