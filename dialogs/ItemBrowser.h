#ifndef ITEMBROWSER_H
#define ITEMBROWSER_H



#include <QtGui/QDialog>
#include <QMultiHash>
#include <QTableWidgetItem>
#include <QItemDelegate>
namespace Ui {
    class ItemBrowser;
}



class QDomElement;
class ItemEffect;
class ComponentsModel;
class ItemModel;
class SNItem;
class ItemBrowser : public QDialog {
    Q_OBJECT
public:
    ItemBrowser(QWidget *parent = 0);
    ~ItemBrowser();

signals:

    void items_changed();

protected:
    void changeEvent(QEvent *e);

private:
    void populateFields( const SNItem &);
    SNItem itemFromFields() const;
    void addEffect( const ItemEffect &effect );
    void addMaterial( const QString &name, int amt );

    Ui::ItemBrowser *m_ui;
    ComponentsModel *m_itemModel;
    ItemModel* m_materialsModel;
    QMultiHash<QString, QString> m_categories;
    QPersistentModelIndex m_selectedItem;
    bool m_fieldsChanged;

    bool loadCategories();
    void loadCategoryAndChildren( const QDomElement & element, const QString & parent = "" );

private slots:
    void on_turnSheetBut_clicked();
    void on_exportBut_clicked();
    void fieldsChangedSot();
    void on_importBut_clicked();
    void on_treeView_pressed(QModelIndex index);
    void on_addMatBut_clicked();
    void on_deleteItemBut_clicked();
    void on_deleteEffectBut_clicked();
    void on_addEffectBut_clicked();
    void on_saveItemBut_clicked();
    void on_categoryCombo_currentIndexChanged(QString );
    void on_treeView_clicked(QModelIndex index);
    void on_addItemBut_clicked();
};

#endif // ITEMBROWSER_H
