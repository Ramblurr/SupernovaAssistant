#ifndef SHIPDESIGNER_H
#define SHIPDESIGNER_H

#include "data/SNItem.h"
#include "data/ShipDesign.h"
#include "models/ShipDesignsModel.h"
#include <QtGui/QWidget>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QModelIndex>
#include <QStringListModel>
#include <QLabel>

namespace Ui {
    class ShipDesigner;
}
class ItemModel;
class ComponentsModel;
class MaterialsModel;
class ComponentsProxyModel;
class ShipDesigner : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(ShipDesigner)
public:
    explicit ShipDesigner( QWidget *parent = 0);
    virtual ~ShipDesigner();

    SNItem getItem( const QString &name );

public slots:
    void currEmpireChangedSlot();
    void componentsFilterAccepted( const QModelIndex & );

protected:
    virtual void changeEvent(QEvent *e);
    bool eventFilter( QObject *, QEvent * );

private:
    Ui::ShipDesigner *m_ui;

    QString m_descHtmlTemplate;
    ShipDesignsModel *m_designsModel;

    QList<ShipDesign> m_designs;
    ShipDesign m_currentDesign;
    ItemModel *m_itemModel;
    MaterialsModel *m_detailedModel;
    ComponentsModel *m_componentsModel;
    ComponentsProxyModel *m_proxy_model;

    QMap<SNItem, qint64> m_materials;
    QList<SNItem> m_components;

    QMenu* m_detailedTableContextMenu;
    QMenu* m_itemListContextMenu;

    QList<QLabel*> m_materialsLabels;
    QList<QLabel*> m_effectLabels;

    void setupDesignsModel();
    void showDesc( const SNItem & item );

public slots:
    void statsChangedSlot(  int numitems, qint64 tons );
    void copyDetailedTableTriggered();
    void copyItemListTriggered();

private slots:
    void on_addEnginesBut_clicked();
    void on_componentFilterEdit_textChanged(QString );
    void on_itemList_customContextMenuRequested(QPoint pos);
    void on_detailedTable_customContextMenuRequested(QPoint pos);
    void on_desiredAPSpin_valueChanged(int );
    void on_generateBI_clicked();
    void on_generateSHIP_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_designsCombo_currentIndexChanged(int index);
    void on_saveButton_clicked();
    void on_componentList_clicked( QModelIndex index );
    void on_generateNUD_clicked();
    void showDescComponents( QModelIndex index );
    void showDescItems( QModelIndex index );

};

#endif // SHIPDESIGNER_H
