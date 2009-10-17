#include "ItemBrowser.h"

#include "TurnParser.h"
#include "../ui/ui_ItemBrowser.h"

#include "../data/SNItem.h"
#include "../delegates/IntegerColumnDelegate.h"
#include "../delegates/GenericDelegate.h"
#include "../delegates/ItemDelegate.h"

#include "../models/ComponentsModel.h"

#include <QtXml/QDomDocument>
#include <QFile>
#include <QDebug>
#include <QtAlgorithms>
#include <QModelIndex>
#include <QMapIterator>
#include <QMessageBox>
#include <QStyleOptionButton>
#include <QIntValidator>
#include <QPainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <algorithm>
ItemBrowser::ItemBrowser(QWidget *parent) :
        QDialog(parent),
        m_ui(new Ui::ItemBrowser),
        m_fieldsChanged( false )
{
    m_ui->setupUi(this);
    this->setWindowTitle("SN Assistant: Item Editor");

    m_itemModel = new ComponentsModel( /*SNItem::getItemsFromDatabase(),*/ this  );
    m_ui->treeView->setModel( m_itemModel);
    m_ui->treeView->setHeaderHidden( true );
    ItemDelegate* delegate = new ItemDelegate( this );
    m_ui->treeView->setItemDelegate( delegate );
    m_ui->treeView->setDragEnabled( true );
    m_ui->treeView->viewport()->setAcceptDrops(true);

    // Setup categories
    loadCategories();
    QStringList list = m_categories.uniqueKeys();
    qSort( list );
    m_ui->categoryCombo->addItems( list );
    m_ui->subcatCombo->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    m_ui->categoryCombo->setSizeAdjustPolicy( QComboBox::AdjustToContents );

    // Setup spinners
    m_ui->tonsSpin->setMaximum( INT_MAX );
    m_ui->structureSpin->setMaximum( INT_MAX );

    // Setup effects table
    m_ui->effectsTable->setColumnCount(4);
    m_ui->effectsTable->verticalHeader()->hide();
    m_ui->effectsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->effectsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    m_ui->effectsTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Pretty Value"));
    m_ui->effectsTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Counters"));
    m_ui->effectsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->effectsTable->setAlternatingRowColors( true );

    GenericDelegate *generic = new GenericDelegate( this );
    IntegerColumnDelegate *valueDelegate = new IntegerColumnDelegate(0, INT_MAX);
    generic->insertColumnDelegate(1, valueDelegate );
    m_ui->effectsTable->setItemDelegate( generic );

    // Setup Component table
    m_ui->materialsTable->setColumnCount( 2 );
    m_ui->materialsTable->verticalHeader()->hide();
    m_ui->materialsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->materialsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Quantity"));
    m_ui->materialsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->materialsTable->viewport()->setAcceptDrops( true );

    generic = new GenericDelegate( this );
    IntegerColumnDelegate *quantityDelegate = new IntegerColumnDelegate(0, INT_MAX);
    generic->insertColumnDelegate(1, quantityDelegate );
    m_ui->materialsTable->setItemDelegate( generic );
    m_fieldsChanged = false;
}

ItemBrowser::~ItemBrowser()
{
    delete m_ui;
}

void ItemBrowser::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ItemBrowser::on_addItemBut_clicked()
{
    int num = 1;
    while( true )
    {
        SNItem i = SNItem::getItem("New Item " + QString::number(num) );
        if( i.name() == "" )
            break;
        num++;
    }
    SNItem item("New Item " + QString::number(num), "Edit me", "Misc");
    m_itemModel->appendItem( item );
}

void ItemBrowser::on_treeView_clicked( QModelIndex index )
{
    // Do not trigger the save message if clicking the same item
    if( index == m_selectedItem )
        return;
    if( m_fieldsChanged )
    {
        QMessageBox msgBox;
        msgBox.setText("The item has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if( ret == QMessageBox::Save )
            on_saveItemBut_clicked();
        else if( ret == QMessageBox::Cancel ) {
            m_ui->treeView->setCurrentIndex( m_selectedItem );
            return;
        }
    }
    QVariant data = m_itemModel->data( index, SN::ComponentRole );
    if( !data.canConvert<SNItem>() ) {
        if( m_ui->treeView->isExpanded( index ) )
            m_ui->treeView->collapse( index );
        else
            m_ui->treeView->expand( index );
        return;
    }
    SNItem item = data.value<SNItem>();

    populateFields( item );
    m_fieldsChanged = false;

    m_selectedItem = QPersistentModelIndex( index );
}

void ItemBrowser::on_addEffectBut_clicked()
{
    ItemEffect e("Name", 0, "Poor", "N/A");
    addEffect( e );
}

void ItemBrowser::addEffect( const ItemEffect &effect )
{
    int row = m_ui->effectsTable->rowCount();
    m_ui->effectsTable->insertRow( row );
    QTableWidgetItem *item0 = new QTableWidgetItem( effect.name() );
    QTableWidgetItem *item1 = new QTableWidgetItem( effect.value() );
    QTableWidgetItem *item2 = new QTableWidgetItem( effect.prettyValue() );
    QTableWidgetItem *item3 = new QTableWidgetItem( effect.counter() );
    m_ui->effectsTable->setItem( row, 0, item0 );
    m_ui->effectsTable->setItem( row, 1, item1 );
    m_ui->effectsTable->setItem( row, 2, item2 );
    m_ui->effectsTable->setItem( row, 3, item3 );
}

void ItemBrowser::addMaterial( const QString &name, int amt )
{
    int row = m_ui->materialsTable->rowCount();
    m_ui->materialsTable->insertRow( row );
    QTableWidgetItem *item0 = new QTableWidgetItem( name );
    QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( amt ) );
    m_ui->materialsTable->setItem( row, 0, item0 );
    m_ui->materialsTable->setItem( row, 1, item1 );
}

void ItemBrowser::on_deleteEffectBut_clicked()
{
    QModelIndexList idxs = m_ui->effectsTable->selectionModel()->selectedIndexes();
    QList<int> ints;
    foreach( QModelIndex idx, idxs )
    {
        ints << idx.row();
    }
    qSort( ints );
    std::list<int> list = ints.toStdList();
    list.unique();
    QList<int> result;
    std::reverse_copy( list.begin(), list.end(), std::back_inserter( result ) );
    foreach( int row, result )
    {
        m_ui->effectsTable->removeRow( row );
    }
}

void ItemBrowser::populateFields( const SNItem &item )
{
    m_ui->nameEdit->setText( item.name() );
    m_ui->descEdit->setPlainText( item.desc() );
    if( m_categories.contains( item.category() ) )
    {
        int idx = m_ui->categoryCombo->findText( item.category() );
        m_ui->categoryCombo->setCurrentIndex( idx );
        if( m_categories.values( item.category() ).contains( item.subcategory() ) )
        {
            idx = m_ui->subcatCombo->findText( item.subcategory() );
            m_ui->subcatCombo->setCurrentIndex( idx );
        }
    } else 
        qDebug() << "CAT not found: '" << item.category() <<"'";
    m_ui->tonsSpin->setValue( item.weight() );
    m_ui->structureSpin->setValue( item.structure() );


    // Reset effects table
    for( int i=m_ui->effectsTable->rowCount(); i >= 0 ; i--)
        m_ui->effectsTable->removeRow( i );

    foreach(ItemEffect effect, item.getEffects() )
    {
        addEffect( effect );
    }

    // Reset materials table
    for( int i=m_ui->materialsTable->rowCount(); i >= 0 ; i--)
        m_ui->materialsTable->removeRow( i );
    QMapIterator<QString, int> it( item.getComponents() );
    while( it.hasNext() )
    {
        it.next();
        addMaterial( it.key(), it.value() );
    }


}

SNItem ItemBrowser::itemFromFields() const
{
    QString name = m_ui->nameEdit->text().trimmed();
    QString desc = m_ui->descEdit->toPlainText();
    QString category = m_ui->categoryCombo->currentText().trimmed();
    QString sub_category = m_ui->subcatCombo->currentText().trimmed();
    int tons = m_ui->tonsSpin->value();
    int struc = m_ui->structureSpin->value();

    SNItem item( name, desc, category, sub_category, tons, struc );

    // Loop through table add the effects
    int rows = m_ui->effectsTable->rowCount();
    for( int i = 0; i < rows; i++ )
    {
        QString name = m_ui->effectsTable->takeItem(i, 0)->data(QTableWidgetItem::Type).toString();
        int value = m_ui->effectsTable->takeItem(i, 1)->data(QTableWidgetItem::Type).toInt();
        QString pvalue = m_ui->effectsTable->takeItem(i, 2)->data(QTableWidgetItem::Type).toString();
        QString counters = m_ui->effectsTable->takeItem(i, 3)->data(QTableWidgetItem::Type).toString();

        ItemEffect effect( name, value, pvalue, counters );
        item.addEffect( effect );
    }

    // Compile components
    rows = m_ui->materialsTable->rowCount();
    for( int i = 0; i < rows; i++ )
    {
        QString name = m_ui->materialsTable->takeItem(i, 0)->data(QTableWidgetItem::Type).toString();
        int amt = m_ui->materialsTable->takeItem(i, 1)->data(QTableWidgetItem::Type).toInt();
        item.addComponent( name, amt );
    }
    return item;
}


bool ItemBrowser::loadCategories()
{
    qDebug() << "loading cats";
    QFile file( "categories.xml" );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "file open failed";
        return false;
    }
    QDomDocument doc( "supernova" );
    QString msg;
    int line;
    if ( !doc.setContent( &file, false, &msg, &line ) )
    {
        qDebug() << "setting doc failed";
        qDebug() << msg;
        qDebug() << line;
        file.close();
        return false;
    }
    QDomElement root = doc.documentElement().firstChildElement();
    if ( root.tagName() != "categories" )
        return false;

    QDomNode n = root.firstChild();
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == "category" )
            loadCategoryAndChildren( e );
        n = n.nextSibling();
    }
    return true;
}
void ItemBrowser::loadCategoryAndChildren( const QDomElement & e, const QString & parent )
{

    if( parent == "" )
        m_categories.insertMulti(e.attribute("name" ).trimmed(), "");
    else
        m_categories.insertMulti(parent, e.attribute("name" ).trimmed());
    QDomElement child = e.firstChildElement( "category" );
    while ( !child.isNull() )
    {
        loadCategoryAndChildren( child,  e.attribute("name" ));
        child = child.nextSiblingElement();
    }
}

void ItemBrowser::on_categoryCombo_currentIndexChanged( QString category )
{
    QStringList sub_cats = m_categories.values( category );

    m_ui->subcatCombo->clear();
    m_ui->subcatCombo->addItems( sub_cats );
}

void ItemBrowser::on_saveItemBut_clicked()
{
    if( !m_selectedItem.isValid() )
        return;
    SNItem item = itemFromFields();
    //    QModelIndex indx = m_itemModel->index(m_selectedItem.row(), m_selectedItem.column(), m_selectedItem.parent() );
    m_itemModel->setData( m_selectedItem, item );
    m_fieldsChanged = false;
}

void ItemBrowser::on_deleteItemBut_clicked()
{
    if( !m_selectedItem.isValid() )
        return;
    if( m_itemModel->data( m_selectedItem, SN::TypeRole ) == SN::Category )
        return;
    QString name = m_itemModel->data( m_selectedItem, Qt::DisplayRole ).toString();
    QMessageBox msgBox;
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setText("Deleting item " + name);
    msgBox.setInformativeText("Are you sure you want to delete " + name + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if( ret == QMessageBox::No )
        return;
    m_itemModel->removeItem( m_selectedItem );
}


void ItemBrowser::on_addMatBut_clicked()
{
    addMaterial( "Item", 1 );
}

void ItemBrowser::on_treeView_pressed(QModelIndex index)
{
    Q_UNUSED(index)
}

void ItemBrowser::on_importBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Item XML File"), QDesktopServices::storageLocation(QDesktopServices::HomeLocation), tr("XML Files(*.xml)"));
    qDebug() << " got file " << fileName;

    QList<SNItem> list = SNItem::getItemsFromXml( fileName );
    foreach( SNItem item, list )
    {
        m_itemModel->appendItem( item );
    }


}

void ItemBrowser::fieldsChangedSot()
{
    m_fieldsChanged = true;
}

void ItemBrowser::on_exportBut_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose where to save the items"), QDesktopServices::storageLocation(QDesktopServices::HomeLocation), tr("XML Files(*.xml)"));
    QList<SNItem> items = m_itemModel->getItems();
    qDebug() << "got " << items.size();
    SNItem::createXML(items, fileName);
}

void ItemBrowser::on_turnSheetBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select turn sheet PDF"), QDesktopServices::storageLocation(QDesktopServices::HomeLocation), tr("XML Files(*.xml)"));
    qDebug() << " got file " << fileName;

    TurnParser tp( fileName );

//    tp.text();
}
