#ifndef MATERIALSMODEL_H
#define MATERIALSMODEL_H
#include "ItemModel.h"
#include <QAbstractTableModel>
class MaterialsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MaterialsModel( QObject *parent=0 );


    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool appendData( const SNItem &name, qint64 quantity );
    bool appendOrAlterData( const SNItem &name, qint64 quantity );

    void clear();

    const static int ComponentRole = 15;

    QList< QPair<SNItem, qint64> > getItems() const;

signals:
    void componentsChanged( const SNItem &, qint64 diff );
    void statsChanged( int, qint64 );

public slots:
    void item_changed( const SNItem &item, qint64 diff );

private:

    /*!
      Removes an item from m_hash, and decrements every element where
      m_hash[element] > m_hash[removeme]
      */
    void removeItem( const SNItem & removeme );
    QMap< SNItem, qint64 > m_hash;
    QList< QPair<SNItem, qint64> > m_data;

};

#endif // MATERIALSMODEL_H
