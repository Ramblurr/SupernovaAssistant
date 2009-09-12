#ifndef MATERIALSWIDGET_H
#define MATERIALSWIDGET_H

#include <QTableWidget>

#include <QStringList>
#include <QMimeData>
#include <QModelIndex>

class MaterialsWidget : public QTableWidget
{
public:
    MaterialsWidget(  QWidget * parent = 0) : QTableWidget( parent ) {}

    QStringList mimeTypes() const;
//    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool dropMimeData ( int row, int column, const QMimeData * data, Qt::DropAction action);
    Qt::DropActions supportedDropActions () const;

};

#endif // MATERIALSWIDGET_H
