#ifndef SHIPDESIGNER_H
#define SHIPDESIGNER_H

#include "data/SNItem.h"
#include "data/ShipDesign.h"
#include "models/ShipDesignsModel.h"
#include <QtGui/QDialog>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QModelIndex>
#include <QStringListModel>
namespace Ui {
    class ShipDesigner;
}
class ItemModel;
class ComponentsModel;
class MaterialsModel;
class ShipDesigner : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ShipDesigner)
public:
    explicit ShipDesigner( QString empId, QWidget *parent = 0);
    virtual ~ShipDesigner();
    static QList<SNItem> m_components;

    static SNItem getItem( const QString &name );

    QString empireId() const { return m_empireId; }

protected:
    virtual void changeEvent(QEvent *e);
    bool eventFilter( QObject *, QEvent * );

private:
    Ui::ShipDesigner *m_ui;

    QString m_empireId;
    QString m_descHtmlTemplate;
    ShipDesignsModel *m_designsModel;

    QList<ShipDesign> m_designs;
    ShipDesign m_currentDesign;
    ItemModel *m_itemModel;
    MaterialsModel *m_detailedModel;
    ComponentsModel *m_componentsModel;
    QMap<SNItem, quint64> m_materials;

    static bool loadItems();
    void setupDesignsModel();

public slots:
    void statsChangedSlot( int, quint64 );

private slots:
    void on_generateBI_clicked();
    void on_generateSHIP_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_designsCombo_currentIndexChanged(int index);
    void on_saveButton_clicked();
    void on_componentList_clicked( QModelIndex index );
    void on_generateNUD_clicked();

};

#endif // SHIPDESIGNER_H