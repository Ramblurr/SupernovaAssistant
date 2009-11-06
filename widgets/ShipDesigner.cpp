#include "ShipDesigner.h"

#include "data/SNItem.h"
#include "data/ShipDesign.h"
#include "data/Order.h"
#include "ui_ShipDesigner.h"
#include "models/ComponentsModel.h"
#include "models/ItemModel.h"
#include "models/MaterialsModel.h"
#include "models/ComponentsProxyModel.h"
#include "delegates/GenericDelegate.h"
#include "delegates/IntegerColumnDelegate.h"
#include "delegates/ItemDelegate.h"
#include "tests/modeltest.h"
#include "SNConstants.h"

#include <QtAlgorithms>
#include <QAction>
#include <QDebug>
#include <QFile>
#include <QtCore/QLocale>
#include <QtCore/QList>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMenu>
#include <QClipboard>
#include <QList>
#include <QPair>


ShipDesigner::ShipDesigner( QWidget *parent ) :
        QWidget( parent ),
        m_ui( new Ui::ShipDesigner )
{
    m_ui->setupUi( this );

    m_itemModel = new ItemModel( this );
    m_ui->itemList->setModel( m_itemModel );
    m_ui->itemList->installEventFilter( this );

    GenericDelegate *generic = new GenericDelegate( this );
    IntegerColumnDelegate *delegate = new IntegerColumnDelegate(0, INT_MAX);
    generic->insertColumnDelegate(1, delegate );

    delegate = new IntegerColumnDelegate(0, INT_MAX);
    generic->insertColumnDelegate(2, delegate );

    m_ui->itemList->setItemDelegate( generic );

//    m_ui->itemList->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    m_itemListContextMenu = new QMenu(this);
    QAction *act = m_itemListContextMenu ->addAction("Copy");
    connect(act, SIGNAL(triggered()), this, SLOT(copyItemListTriggered()));
    act->setShortcut(QKeySequence::Copy);
    act->setShortcutContext(Qt::WidgetShortcut);
    m_ui->itemList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->itemList->addAction(act);

    m_detailedModel = new MaterialsModel( this );
    new ModelTest(m_detailedModel, this);
    m_ui->detailedTable->setModel( m_detailedModel );
    m_detailedTableContextMenu = new QMenu(this);
    act = m_detailedTableContextMenu ->addAction("Copy");
    connect(act, SIGNAL(triggered()), this, SLOT(copyDetailedTableTriggered()));
    act->setShortcut(QKeySequence::Copy);
    act->setShortcutContext(Qt::WidgetShortcut);
    m_ui->detailedTable->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->detailedTable->addAction(act);

    m_components = SNItem::getItemsFromDatabase();

    QList<SNItem> items;

    for( int i=0; i<ShipDesigner::m_components.size(); i++)
        if( ShipDesigner::m_components.at(i).category() == "Ship Component" )
            items << ShipDesigner::m_components.at(i);
    qDebug() << "items cnt: " << items.size();
    m_componentsModel = new ComponentsModel( this, "Ship Component");
    m_proxy_model = new ComponentsProxyModel(this);
    m_proxy_model->setSourceModel( m_componentsModel );

    m_ui->componentList->setModel( m_proxy_model );
    m_ui->componentList->setRootIsDecorated( false );
    ItemDelegate* idelegate = new ItemDelegate( this );
    m_ui->componentList->setItemDelegate( idelegate );
    m_ui->componentList->setHeaderHidden( true );
    m_ui->componentList->installEventFilter( this );

    QFile file( ":/item_desc_format.html" );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qDebug() << "error opening file. Error: " << file.error();
    }
    m_descHtmlTemplate = file.readAll();

    this->setWindowTitle( "Supernova Assistant: Ship Designer" );

    connect( m_itemModel, SIGNAL( componentsChanged( const SNItem &, quint64 ) ), m_detailedModel, SLOT( item_changed( const SNItem&,quint64) ));
    connect( m_itemModel, SIGNAL( statsChanged( int, quint64 ) ), this, SLOT( statsChangedSlot( int, quint64 ) ) );
    connect( m_ui->componentList->selectionModel(),
             SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT( showDescComponents( QModelIndex ) ) );

    connect( m_ui->itemList->selectionModel(),
             SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT( showDescItems( QModelIndex ) ) );


    setupDesignsModel();

    /* you cannot adjust the stretch factor of layouts in
     * a splitter. So we set proportional sizes here*/
    QList<int> sizes;
    sizes.push_back(700);
    sizes.push_back(300);
    m_ui->splitter_2->setSizes(sizes);

    QList<int> sizes2;
    sizes2.push_back(600);
    sizes2.push_back(400);
    m_ui->splitter->setSizes(sizes);
}

ShipDesigner::~ShipDesigner()
{
    delete m_ui;
}

void ShipDesigner::currEmpireChangedSlot()
{
    setupDesignsModel();
    m_itemModel->clear();
    m_detailedModel->clear();
    m_ui->tonnageLabel->setText("0");
    m_ui->countLabel->setText("0");
    m_ui->warpLabel->setText("0");
    m_ui->bubbleLabel->setText("0");
    m_ui->massvthrustLabel->setText("0");
    m_ui->apsLabel->setText("0");
    m_ui->fcLabel->setText("0");
    m_ui->integrityLabel->setText("0");
    m_ui->thrustLabel->setText("0");
    m_ui->reqEnginesEdit->setText("0");
    m_ui->desiredAPSpin->clear();
    m_componentsModel->resetModel();
    m_ui->designNameEdit->clear();
    m_ui->quantityEdit->clear();
    m_ui->popGroupEdit->clear();
    m_ui->priorityEdit->clear();
    m_ui->fleetEdit->clear();
    m_ui->componentFilterEdit->clear();
}

void ShipDesigner::componentsFilterAccepted( const QModelIndex & idx)
{
    m_ui->componentList->expand( idx );
}

void ShipDesigner::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

bool ShipDesigner::eventFilter( QObject * obj, QEvent * evt )
{
    if ( evt->type() != QEvent::KeyPress )
        return QObject::eventFilter( obj, evt );
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>( evt );
    if ( keyEvent->key() == Qt::Key_Delete  && m_ui->itemList->hasFocus() )
    {
        QModelIndexList idxs = m_ui->itemList->selectionModel()->selectedIndexes();
        foreach( QModelIndex idx, idxs )
        {
            if ( idx.isValid() ) {
                m_itemModel->removeRow( idx.row() );
                return true;
            }
        }
    }
    if( (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return )&& m_ui->componentList->hasFocus() )
    {
        on_addButton_clicked();
        return true;
    }
    return QObject::eventFilter( obj, evt );
}

void ShipDesigner::on_componentList_clicked( QModelIndex index )
{
    if ( m_componentsModel->data( index, SN::TypeRole ) != SN::Component )
    {
        if( m_ui->componentList->isExpanded( index ) )
            m_ui->componentList->collapse( index );
        else
            m_ui->componentList->expand( index );
        return;
    }
}

void ShipDesigner::showDesc( const SNItem & item )
{
    if( item.isEmpty() )
        return;
    QLocale locale;
    QString format = m_descHtmlTemplate;
    format.replace( "{CAT}", item.category());
    format.replace( "{SUBCAT}", item.subcategory());
    format.replace( "{DESC}", item.desc() );
    format.replace( "{TONS}", locale.toString( item.weight() ) );
    format.replace( "{INTEG}", locale.toString( item.structure() ) );

    m_ui->componentName->setText( item.name() );

    QString materials = "";
    QMapIterator<QString, int> i( item.getComponents() );
    while ( i.hasNext() )
    {
        i.next();
        materials.append( QString::number( i.value() ) + " " + i.key() );
        materials.append( ", " );
    }
    materials.chop( 2 );
    format.replace( "{MATS}", materials );

    QString effects = "";
    QList<ItemEffect> effs = item.getEffects();
    foreach(ItemEffect eff, effs)
    {
        effects.append( eff.name() + ": " + eff.prettyValue());
        if( eff.value() != 0 )
            effects.append( "["+ locale.toString( eff.value() ) +"]" );
        effects.append("<br />");
    }
    format.replace( "{EFFS}", effects );

    m_ui->componentDesc->setHtml( format );
}

void ShipDesigner::showDescComponents( QModelIndex index )
{
    if ( m_descHtmlTemplate == "" )
        return;
    QModelIndex real_index = m_proxy_model->mapToSource( index );
    int type = m_componentsModel->data(real_index, SN::TypeRole ).toInt();
    if(  type == SN::Component )
    {
        QVariant data = m_componentsModel->data( real_index, SN::ComponentRole );
        SNItem item = data.value<SNItem>();
        showDesc( item );
    }
}

void ShipDesigner::showDescItems( QModelIndex index )
{
    if ( m_descHtmlTemplate == "" )
        return;
    QVariant data = m_itemModel->data( index, SN::ComponentRole );
    SNItem item = data.value<SNItem>();
    showDesc( item );
}

void ShipDesigner::on_saveButton_clicked()
{
    // TODO UPDATE design on save
    QString name = m_ui->designNameEdit->text().trimmed();
    if ( name.length() == 0)
    {
        QMessageBox::warning( 0, "Save Error", "Design name cannot be blank." );
        return;
    }
    if( ShipDesign::designExists( name )  )
    {
        QMessageBox::warning( 0, "Save Error", "Design already exists. The name must be unique." );
        return;
    }
    QString type = m_ui->typeCombo->currentText();
    QString mclass = m_ui->missionCombo->currentText().left( 1 );
    ShipDesign design( name, type, mclass );

    QList< ItemEntry > items = m_itemModel->getItems();

    for( int i = 0; i < items.size(); i++ )
    {
        ItemEntry p = items.at(i);
        qDebug() << "appending: " << p.item.name();
        if( p.quantity > 0 )
            design.addComponent( p.item.name(), p.quantity );
    }

    design.saveDesign();
    setupDesignsModel();

    m_currentDesign = design;
    for ( int i = 0; i < m_ui->designsCombo->count(); i++ )
    {
        QString name ( m_designs.at( i ).name() );
        if ( m_currentDesign.name() == name )
        {
            m_ui->designsCombo->setCurrentIndex( i );
            break;
        }
    }
}

void ShipDesigner::setupDesignsModel()
{
//    if( m_designsModel )
//        delete m_designsModel;
    m_designs = ShipDesign::getDesigns();
    m_designsModel = new ShipDesignsModel( m_designs, this );
    m_ui->designsCombo->setModel( m_designsModel );
    if ( m_designs.size() > 0 )
    {
        m_currentDesign = m_designs.at( 0 );
    }
}

void ShipDesigner::on_designsCombo_currentIndexChanged( int index )
{
    if ( index < 0 )
        return;
    m_itemModel->clear();
    m_detailedModel->clear();
    m_ui->tonnageLabel->setText("0");
    m_ui->countLabel->setText("0");
    m_currentDesign = m_designs.at( index );
    m_ui->designNameEdit->setText( m_currentDesign.name() );

    // Set the correct Design Type
    for ( int i = 0; i < m_ui->typeCombo->count(); i++ )
    {
        QString type = m_ui->typeCombo->itemData( i, Qt::DisplayRole ).toString();
        if ( type == m_currentDesign.type() )
        {
            m_ui->typeCombo->setCurrentIndex( i );
            break;
        }
    }

    // Set the correct Mission Class
    for ( int i = 0; i < m_ui->missionCombo->count(); i++ )
    {
        QString mclass = m_ui->missionCombo->itemData( i, Qt::DisplayRole ).toString().left( 1 );
        if ( mclass == m_currentDesign.missionClass() )
        {
            m_ui->missionCombo->setCurrentIndex( i );
            break;
        }
    }
    QMapIterator< QString, quint64 > it( m_currentDesign.getComponents() );
    qDebug() << "num comps " << m_currentDesign.getComponents().size();
    while( it.hasNext() )
    {
        it.next();
        SNItem item = getItem( it.key() );
        if( item.name() != "" )
            m_itemModel->appendData(item, it.value());
    }
}

void ShipDesigner::on_deleteButton_clicked()
{
    int curr = m_ui->designsCombo->currentIndex();
    if ( curr < 0 )
        return;
    ShipDesign des = m_ui->designsCombo->itemData( curr, ShipDesignsModel::ShipDesignRole ).value<ShipDesign>();
    des.deleteDesign();
    setupDesignsModel();
}

void ShipDesigner::on_addButton_clicked()
{
//   QMap< QString, int > list = m_itemModel->getList();

    QItemSelectionModel *selectionModel = m_ui->componentList->selectionModel();
    QModelIndexList idxs = selectionModel->selectedIndexes();
    for ( int i = 0; i < idxs.size(); i++ )
    {
        QModelIndex idx = idxs.at( i );
        SNItem item = idx.data( SN::ComponentRole ).value<SNItem>();
        m_itemModel->appendData(item, 0);
    }
}

void ShipDesigner::statsChangedSlot( int numitems, quint64 tons )
{
    m_ui->itemList->resizeColumnToContents(0);
    m_ui->itemList->resizeColumnToContents(1);
    m_ui->itemList->resizeColumnToContents(2);
    m_ui->itemList->resizeColumnToContents(3);
    // Set item count total
    int curr = m_ui->countLabel->text().toInt();

    QLocale locale;
    // set tonnage

    QString tmp = m_ui->tonnageLabel->text();
    quint64 tonnage = locale.toUInt(tmp);
    m_ui->tonnageLabel->setText( locale.toString( tonnage + tons ) );
    m_ui->countLabel->setText( locale.toString( curr + numitems ) );

    // set AP
    m_ui->apsLabel->setText( locale.toString( m_itemModel->actionPoints() ) );

    //set Thrust
    m_ui->thrustLabel->setText( locale.toString( m_itemModel->thrust() ) );

    //set warp info
    m_ui->warpLabel->setText( locale.toString( m_itemModel->warpRating() ) );

    m_ui->bubbleLabel->setText( locale.toString( m_itemModel->warpBubble(), 'f', 2) );

    // mass v thrust ratio
    m_ui->massvthrustLabel->setText( locale.toString( m_itemModel->massThrustRatio() ) );


}
void ShipDesigner::copyItemListTriggered()
{
    QItemSelectionModel * selection = m_ui->itemList->selectionModel();
    QString selected_text;
    // Case 1: Full rows seelected
    QModelIndexList rows = selection->selectedRows();
    if( rows.size() > 0)
    {
        qSort(rows);
        foreach(QModelIndex row, rows)
        {
            QVariant data = m_itemModel->data(m_itemModel->index(row.row(), 0), Qt::DisplayRole);
            QString text = data.toString();
            selected_text.append( text );
            data = m_itemModel->data(m_itemModel->index(row.row(), 1), Qt::DisplayRole);
            text = data.toString();
            selected_text.append("\t");
            selected_text.append(text);
            selected_text.append("\n");

        }
    } else { // Case 2: everything else
        QModelIndexList indxs = selection->selectedIndexes();
        qSort(indxs);
        foreach(QModelIndex idx, indxs)
        {
            QVariant data = m_itemModel->data(idx, Qt::DisplayRole);
            QString text = data.toString();
            selected_text.append( text );
            selected_text.append("\n");
        }
    }
    //remove the last \n
    selected_text = selected_text.left( selected_text.length() -1 );
    QApplication::clipboard()->setText(selected_text);
}

void ShipDesigner::copyDetailedTableTriggered()
{
    QItemSelectionModel * selection = m_ui->detailedTable->selectionModel();

    QString selected_text;
    // Case 1: Full rows seelected
    QModelIndexList rows = selection->selectedRows();
    if( rows.size() > 0)
    {
        qSort(rows);
        foreach(QModelIndex row, rows)
        {
            QVariant data = m_detailedModel->data(m_detailedModel->index(row.row(), 0), Qt::DisplayRole);
            QString text = data.toString();
            selected_text.append( text );
            data = m_detailedModel->data(m_detailedModel->index(row.row(), 1), Qt::DisplayRole);
            text = data.toString();
            selected_text.append("\t");
            selected_text.append(text);
            selected_text.append("\n");

        }
    } else { // Case 2: everything else
        QModelIndexList indxs = selection->selectedIndexes();
        qSort(indxs);
        foreach(QModelIndex idx, indxs)
        {
            QVariant data = m_detailedModel->data(idx, Qt::DisplayRole);
            QString text = data.toString();
            selected_text.append( text );
            selected_text.append("\n");
        }
    }
    //remove the last \n
    selected_text = selected_text.left( selected_text.length() -1 );
    QApplication::clipboard()->setText(selected_text);
}

SNItem ShipDesigner::getItem( const QString &name )
{
    for (int i = 0; i < m_components.size(); ++i)
    {
        if (m_components.at(i).name() == name)
            return m_components.at(i);
    }
    return SNItem();
}

/**
 * Generates NUD Order
 * NUD, Design Type, Design Name, Mission Class, Item #1, Quantity #1, …, Item # 24, Quantity # 24
 */

void ShipDesigner::on_generateNUD_clicked()
{
    //"NUD","Starship","test","A","Blah","1"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "
    if( m_currentDesign.name() == "" )
        return;
    Order order( "NUD" );

    order << m_currentDesign.type() << m_currentDesign.name() << m_currentDesign.missionClass();
    QMapIterator< QString, quint64 > it( m_currentDesign.getComponents() );
    while( it.hasNext() )
    {
        it.next();
        order << it.key() << it.value();
    }

    qDebug() << order.print();
}

void ShipDesigner::on_generateSHIP_clicked()
{
    if( m_currentDesign.name() == "" )
        return;
    Order order( "SHIP" );

    order << m_ui->quantityEdit->text().trimmed() << m_currentDesign.name() << m_ui->popGroupEdit->text().trimmed();
    order << m_ui->priorityEdit->text().trimmed() << m_ui->fleetEdit->text().trimmed();

    qDebug() << order.print();
}

void ShipDesigner::on_generateBI_clicked()
{

}

void ShipDesigner::on_desiredAPSpin_valueChanged(int ap)
{
    //  (#E) = ( AP * tonnage ) / ( thrust – 100AP )
    quint64 total_tonnage = m_itemModel->totalItemsTonnage().second;

    quint64 engine_tonnage = 0;
    int thrust_value = 0;
    QList<ItemEntry> items = m_itemModel->getItems();
    foreach(ItemEntry pair, items)
    {
        if(pair.item.subcategory() == SN::Category::Engine)
        {
            engine_tonnage += pair.quantity* pair.item.weight();
            foreach( ItemEffect effect, pair.item.getEffects() )
            {
                    if( effect.name() == "Maneuverability" )
                        thrust_value += effect.value();
            }
        }
    }

    int numerator = (ap * (total_tonnage - engine_tonnage ) );
    int denominator = (thrust_value - (100*ap));
    int num_engines = -1;
    if( denominator != 0 )
        num_engines = (numerator / denominator)+1;

//    qDebug() << "num" << numerator << "dem" << denominator;

    if( num_engines != -1)
        m_ui->reqEnginesEdit->setText( QString::number(num_engines) );
    else
        m_ui->reqEnginesEdit->setText( "Not possible" );
}

void ShipDesigner::on_detailedTable_customContextMenuRequested(QPoint pos)
{
    m_detailedTableContextMenu->exec(m_ui->detailedTable->viewport()->mapToGlobal(pos));
}

void ShipDesigner::on_itemList_customContextMenuRequested(QPoint pos)
{
    m_itemListContextMenu->exec(m_ui->itemList->viewport()->mapToGlobal(pos));
}

void ShipDesigner::on_componentFilterEdit_textChanged(QString filter)
{
    m_proxy_model->setFilterWildcard( filter );
}

void ShipDesigner::on_addEnginesBut_clicked()
{

}
