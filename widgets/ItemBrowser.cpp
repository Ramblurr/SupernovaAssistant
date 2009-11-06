#include "ItemBrowser.h"

#include "TurnParser.h"
#include "ui_ItemBrowser.h"

#include "data/SNItem.h"
#include "delegates/IntegerColumnDelegate.h"
#include "delegates/GenericDelegate.h"
#include "delegates/ItemDelegate.h"

#include "models/ComponentsModel.h"

#include "ItemConflictDialog.h"

#include <QtXml/QDomDocument>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QtAlgorithms>
#include <QModelIndex>
#include <QMapIterator>
#include <QMessageBox>
#include <QStyleOptionButton>
#include <QIntValidator>
#include <QPainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <QSettings>
#include <algorithm>

ItemBrowser::ItemBrowser(QWidget *parent) :
        QWidget(parent),
        m_ui(new Ui::ItemBrowser),
        m_fieldsChanged( false ),
        m_itemConflictResolution( -1 )
{
    m_ui->setupUi(this);

    m_itemModel = new ComponentsModel( /*SNItem::getItemsFromDatabase(),*/ this  );
    m_ui->treeView->setModel( m_itemModel);
    m_ui->treeView->setHeaderHidden( true );
    ItemDelegate* delegate = new ItemDelegate( this );
    m_ui->treeView->setItemDelegate( delegate );
    m_ui->treeView->setDragEnabled( true );
    m_ui->treeView->viewport()->setAcceptDrops(true);
    m_ui->treeView->installEventFilter( this );
    m_ui->treeView->setRootIsDecorated( false );
//    m_ui->treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );

    // Setup categories
    loadCategories();
    QStringList list = m_categories.uniqueKeys();
    list.removeAll("");
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

    /* you cannot adjust the stretch factor of layouts in
     * a splitter. So we set proportional sizes here*/
    QList<int> sizes;
    sizes.push_back(400);
    sizes.push_back(600);
    m_ui->splitter->setSizes(sizes);

    m_fieldsChanged = false;

    setWindowTitle("SN Assistant: Item Editor");

    connect( m_ui->treeView->selectionModel(),
             SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT( setCurrent( QModelIndex ) ) );
}

ItemBrowser::~ItemBrowser()
{
    delete m_ui;
}

void ItemBrowser::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool ItemBrowser::eventFilter( QObject * obj, QEvent * evt )
{
    if ( evt->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( evt );
        if ( keyEvent->key() == Qt::Key_Delete ) {
            if( m_ui->treeView->hasFocus() )
            {
                on_deleteItemBut_clicked();
                return true;
            }
        }
    }
    return QObject::eventFilter( obj, evt );
}

void ItemBrowser::currEmpireChangedSlot()
{
    m_ui->nameEdit->clear();
    m_ui->descEdit->clear();
    m_ui->tonsSpin->clear();
    m_ui->structureSpin->clear();

    m_ui->effectsTable->clear();
    m_ui->materialsTable->clear();

    m_itemModel->resetModel();

    m_fieldsChanged = false;
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
    SNItem item("New Item " + QString::number(num), "Edit me", "Unknown");
    m_itemModel->appendItem( item );
}

void ItemBrowser::on_treeView_clicked( QModelIndex index )
{
    int type = m_itemModel->data(index, SN::TypeRole ).toInt();
    if( type == SN::Category || type == SN::SubCategory )
    {
        if( m_ui->treeView->isExpanded( index ) )
            m_ui->treeView->collapse( index );
        else
            m_ui->treeView->expand( index );
    }
}

void ItemBrowser::setCurrent( QModelIndex index )
{
    if( int type = m_itemModel->data(index, SN::TypeRole ).toInt() == SN::Component )
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
            } else if( ret == QMessageBox::Discard )
                m_fieldsChanged = false;
        }
        QVariant data = m_itemModel->data( index, SN::ComponentRole );
        SNItem item = data.value<SNItem>();

        populateFields( item );
        m_fieldsChanged = false;

        m_selectedItem = QPersistentModelIndex( index );
    }
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
    QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( effect.value() ) );
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
    for( int i=m_ui->effectsTable->rowCount(); i >= 0 ; i-- )
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
    QFile file( ":/categories.xml" );
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

    if( parent.isEmpty() )
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
    sub_cats.removeAll("");

    m_ui->subcatCombo->clear();
    m_ui->subcatCombo->addItems( sub_cats );
}

void ItemBrowser::on_saveItemBut_clicked()
{
    if( !m_selectedItem.isValid() )
        return;
    SNItem item = itemFromFields();
    //    QModelIndex indx = m_itemModel->index(m_selectedItem.row(), m_selectedItem.column(), m_selectedItem.parent() );
    m_fieldsChanged = false;
    m_itemModel->setData( m_selectedItem, item );
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
    // We want to save the directory the user picks
    QSettings settings( "SN", "SNAssistant" );

    QVariant loc = settings.value("import-items-location", QDesktopServices::storageLocation(QDesktopServices::HomeLocation));

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Item XML File"), loc.toString(), tr("XML Files(*.xml)"));

    if( fileName.isEmpty() )
        return;

    QFileInfo info( fileName );
    settings.setValue("import-items-location", info.absoluteDir().absolutePath());

    qDebug() << " got file " << fileName;

    QList<SNItem> list = SNItem::getItemsFromXml( fileName );
    foreach( SNItem item, list )
    {
        appendItemToModel( item );
    }


}

void ItemBrowser::fieldsChangedSot()
{
    m_fieldsChanged = true;
}

void ItemBrowser::on_exportBut_clicked()
{
    // We want to save the directory the user picks
    QSettings settings( "SN", "SNAssistant" );
    QVariant loc = settings.value("export-items-location", QDesktopServices::storageLocation(QDesktopServices::HomeLocation));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose where to save the items"), loc.toString(), tr("XML Files(*.xml)"));
    if( fileName.isEmpty() )
        return;

    QFileInfo info( fileName );

    settings.setValue("export-items-location", info.absoluteDir().absolutePath());

    QList<SNItem> items = m_itemModel->getItems();
    qDebug() << "got " << items.size();
    SNItem::createXML(items, fileName);
}

void ItemBrowser::on_turnSheetBut_clicked()
{
    // We want to save the directory where the user
    // picks his turn sheets from
    QSettings settings( "SN", "SNAssistant" );
    QVariant loc = settings.value("turn-sheet-location", QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select turn sheet PDF"), loc.toString(), tr("PDF Files(*.pdf)"));
    QFileInfo info( fileName );
    if(!info.exists())
        return;
    settings.setValue("turn-sheet-location", info.absoluteDir().absolutePath());

    TurnParser* tp = new TurnParser( fileName, this );
//    tp.writeOut("anztest");
    connect(tp, SIGNAL(anzParsingComplete( const QList<SNItem> & )), this, SLOT(anzParsingFinishedSlot( const QList<SNItem> & )), Qt::QueuedConnection);
    tp->start();

}

void ItemBrowser::anzParsingFinishedSlot( const QList<SNItem> & items)
{
    qDebug() << "got " << items.size();
    foreach( SNItem item, items )
    {
        appendItemToModel( item );
    }
}

void ItemBrowser::appendItemToModel( const SNItem & item )
{
        SNItem existing = SNItem::getItem( item.name() );
        if( existing.isEmpty() ) // doesn't exist , no conflict
        {
            m_itemModel->appendItem( item );
            return;
        }
        if( existing == item ) // already exists, and is the same no conflict
            return ;

        if( m_itemConflictResolution == SN::KeepNewItemAndRepeat )
        {
            m_itemModel->removeItem( existing );
            m_itemModel->appendItem( item );
            return;
        } else if ( m_itemConflictResolution == SN::KeepOldItemAndRepeat )
        {
            m_itemModel->appendItem( item );
            return;
        }

        ItemConflictDialog conflictDialog( existing, item, this  );
        m_itemConflictResolution = conflictDialog.exec();

        if( m_itemConflictResolution == SN::KeepNewItem || m_itemConflictResolution == SN::KeepNewItemAndRepeat )
        {
           m_itemModel->removeItem( existing );
           m_itemModel->appendItem( item );
        }
}

void ItemBrowser::on_clearDbaseBut_clicked()
{
    QMessageBox msgBox;
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setText("Erasing Database");
    msgBox.setInformativeText("This will ERASE the item database. Do you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if( ret == QMessageBox::No )
        return;
    m_itemModel->eraseDatabase();


}
