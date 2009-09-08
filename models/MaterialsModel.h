#ifndef MATERIALSMODEL_H
#define MATERIALSMODEL_H
#include "ItemModel.h"
class MaterialsModel : public ItemModel
{
    Q_OBJECT
public:
    MaterialsModel( QObject *parent=0 );

    virtual Qt::ItemFlags flags( const QModelIndex &index ) const;

public slots:
    void item_changed( const SNItem &item, quint64 diff );

};

#endif // MATERIALSMODEL_H
