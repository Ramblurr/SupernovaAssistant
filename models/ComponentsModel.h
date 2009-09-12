#ifndef COMPONENTSMODEL_H
#define COMPONENTSMODEL_H

#include "data/SNItem.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QMimeData>

namespace SN
{
    enum Type
    {
        Root = 0,
        Category,
        Component
    };

    enum Role
    {
        ComponentRole = Qt::UserRole,
        TypeRole

    };
}
class ComponentTreeItem;
class ComponentsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ComponentsModel(const QList<SNItem> &items, QObject *parent = 0);

    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;

    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeItem ( const QModelIndex & index );

    void appendItem( const SNItem &item );

    QList<SNItem> getItems() const;

private:
    void emitRowChanged(int parent, int child = -1);
    void setupModelData();
    bool prepareSubCat( const SNItem &item, ComponentTreeItem *parent );
    void getItemsRecursive( const ComponentTreeItem *parent, QList<SNItem> &list ) const;

    QList<SNItem> m_items;
    ComponentTreeItem *m_rootItem;
    QHash<QString, ComponentTreeItem*> m_cats;
    QHash<QString, ComponentTreeItem*> m_subcats;
};

class ComponentTreeItem
{
public:
    ComponentTreeItem( const SN::Type &type, const QVariant &data,  ComponentTreeItem *parent = 0 );
    ~ComponentTreeItem();

    void appendChild ( ComponentTreeItem *child );
    bool removeChild(  ComponentTreeItem *child );
    void setData( const QVariant &data );

    ComponentTreeItem *child ( int row );
    QList<ComponentTreeItem*> children() const;

    int childCount() const;
    int columnCount() const;
    QVariant data () const;
    int row() const;
    ComponentTreeItem *parent();
//    SNItem item() const;
    SN::Type type() const
    {
        return m_type;
    }
private:
    QList<ComponentTreeItem*> childItems;
    SN::Type m_type;
    ComponentTreeItem *parentItem;
    QVariant itemData;

};

#endif // COMPONENTSMODEL_H
