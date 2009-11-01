#ifndef COMPONENTSMODEL_H
#define COMPONENTSMODEL_H

#include "../data/SNItem.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QMimeData>

namespace SN
{
    enum Type
    {
        Root = 0,
        Category,
        SubCategory,
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
    ComponentsModel( QObject *parent = 0, const QString &category_filter = "");

    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool canFetchMore ( const QModelIndex & parent ) const;
    void fetchMore ( const QModelIndex & parent );
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;


    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeItem ( const QModelIndex & index );
    bool removeItem ( const SNItem &item );

    void appendItem( const SNItem &item );

    QList<SNItem> getItems() const;

    /*! ERASES THE DATABASE !!
        ... as well as the model.
        */
    void clear(); //!< ERASES the database!

private:
//    void emitRowChanged(int parent, int child = -1);
    bool prepareSubCat( const SNItem &item, ComponentTreeItem *parent );
    void getItemsRecursive( const ComponentTreeItem *parent, QList<SNItem> &list ) const;
    /*!
      Returns the QModelIndex of an SNItem.
     */
    QModelIndex getSNItemIndexRecursive( const QModelIndex & parent, const QString &item_name ) const;
    /*!
      Returns the QModelIndex of a ComponentTreeItem* item
     */
    QModelIndex getIndexRecursive( const QModelIndex & parent, const ComponentTreeItem *item ) const;

    /*!
      Returns the newly created ComponentTreeItem
      */
    ComponentTreeItem* insertItem( ComponentTreeItem* parent, const QVariant & value,  const SN::Type &type );

    ComponentTreeItem *m_rootItem;
    QHash<QString, ComponentTreeItem*> m_cats;
    QHash<QString, ComponentTreeItem*> m_subcats;
    QString m_catfilter;
};

class ComponentTreeItem
{
public:
    ComponentTreeItem( const SN::Type &type, const QVariant &data,  ComponentTreeItem *parent = 0 );
    ~ComponentTreeItem();

    void clearAllChildren();
    void appendChild ( ComponentTreeItem *child );
    bool removeChild(  ComponentTreeItem *child );
    void setChildren( QList<ComponentTreeItem*> children );
    void setData( const QVariant &data );
    void setQuery( const QString &query );
    void setLoaded( bool loaded );

    ComponentTreeItem *child ( int row );
    QList<ComponentTreeItem*> children() const;

    int childCount() const;
    int columnCount() const;
    QVariant data () const;
    bool loaded() const;
    int row() const;
    ComponentTreeItem *parent();
//    SNItem item() const;
    SN::Type type() const
    {
        return m_type;
    }
    QString query() const { return m_query; }
private:
    QList<ComponentTreeItem*> childItems;
    SN::Type m_type;
    QVariant itemData;
    ComponentTreeItem *parentItem;

    QString m_query;
    bool m_loaded;

};

#endif // COMPONENTSMODEL_H
