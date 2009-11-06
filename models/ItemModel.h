#ifndef ITEMMODEL_H
#define ITEMMODEL_H
#include "SNItem.h"

#include "SNConstants.h"

#include <QAbstractTableModel>
#include <QStringList>


class ItemEntry
{
public:
    ItemEntry() : lockpercentage( false ) {}
    SNItem item;
    quint64 quantity;
    quint64 percentage;
    bool lockpercentage;
};

class ItemEntry;
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
     QStringList mimeTypes() const;
     bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
     Qt::DropActions supportedDropActions () const;
     bool appendData( const SNItem &name, quint64 quantity );
     bool appendOrAlterData( const SNItem &name, quint64 quantity );

     void clear();

     QList<ItemEntry> getItems() const;

     //! Returns a QPair<#items, tonnage> of the design represented in the model
     QPair<int, int> totalItemsTonnage() const;

     int actionPoints() const;
     double warpBubble() const;
     int warpRating() const;
     int thrust() const;
     int massThrustRatio() const;
     int firewpower() const;
     int integrity() const;
     int firecontrol() const;

signals:
    void componentsChanged( const SNItem &, quint64 diff );
    void statsChanged( int, quint64 );

 private:
    QMap< SNItem, quint64 > m_hash;
    QList<ItemEntry> m_data;

    int m_ap;
    int m_warp;
    int m_bubble;
    int m_thrust;
    int m_firewpower;
    int m_integrity;
    int m_firecontrol;


private slots:
    void refreshStats();
};

#endif // ITEMMODEL_H
