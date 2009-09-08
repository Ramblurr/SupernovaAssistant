#ifndef ITEMMODEL_H
#define ITEMMODEL_H
#include "data/SNItem.h"
#include <QAbstractTableModel>
class ItemModel : public QAbstractTableModel
{
     Q_OBJECT

 public:

     ItemModel( QObject *parent=0 );

     int rowCount(const QModelIndex &parent) const;
     int columnCount(const QModelIndex &parent) const;
     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role) const;
     virtual Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
     bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
     bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

     bool appendData( const SNItem &name, quint64 quantity );
     bool appendOrAlterData( const SNItem &name, quint64 quantity );

     void clear();

     const static int ComponentRole = 15;

     QList< QPair<SNItem, quint64> > getItems() const;

signals:
    void componentsChanged( const SNItem &, quint64 diff );
    void statsChanged( int, quint64 );

 protected:
    QMap< SNItem, quint64 > m_hash;
    QList< QPair<SNItem, quint64> > m_data;
};

#endif // ITEMMODEL_H
