#ifndef SHIPDESIGNSMODEL_H
#define SHIPDESIGNSMODEL_H

#include "data/ShipDesign.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>

class ShipDesignsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ShipDesignsModel( const QList<ShipDesign> &items, QObject *parent = 0 ) : QAbstractListModel(parent), m_items( items ) {}

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

    const static int ShipDesignRole = 15;

    private:
    QList<ShipDesign> m_items;
};

#endif // SHIPDESIGNSMODEL_H
