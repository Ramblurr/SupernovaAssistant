#include "ShipDesigner.h"

#include "data/SNItem.h"
#include "data/ShipDesign.h"
#include "data/Order.h"
#include "ui/ui_ShipDesigner.h"
#include "models/ComponentsModel.h"
#include "models/ItemModel.h"
#include "models/MaterialsModel.h"
#include "delegates/GenericDelegate.h"
#include "delegates/IntegerColumnDelegate.h"
#include "delegates/ItemDelegate.h"
#include <QDebug>
#include <QFile>
#include <QtCore/QLocale>
#include <QtCore/QList>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>


ShipDesigner::ShipDesigner( QString empid, QWidget *parent ) :
        QDialog( parent ),
        m_ui( new Ui::ShipDesigner ),
        m_empireId( empid )
{
    m_ui->setupUi( this );

    m_itemModel = new ItemModel( this );
    m_ui->itemList->setModel( m_itemModel );
    m_ui->itemList->installEventFilter( this );
        GenericDelegate *generic = new GenericDelegate( this );
    IntegerColumnDelegate *delegate = new IntegerColumnDelegate(0, INT_MAX);
    generic->insertColumnDelegate(1, delegate );
    m_ui->itemList->setItemDelegate( generic );

    m_detailedModel = new MaterialsModel( this );
    m_ui->detailedTable->setModel( m_detailedModel );

    m_components = SNItem::getItemsFromDatabase();

    QList<SNItem> items;

    for( int i=0; i<ShipDesigner::m_components.size(); i++)
        if( ShipDesigner::m_components.at(i).category() == "Ship Component" )
            items << ShipDesigner::m_components.at(i);
    qDebug() << "items cnt: " << items.size();
    m_componentsModel = new ComponentsModel( this, "Ship Component");
    m_ui->componentList->setModel( m_componentsModel );
    ItemDelegate* idelegate = new ItemDelegate( this );
    m_ui->componentList->setItemDelegate( idelegate );
    m_ui->componentList->setHeaderHidden( true );

    QFile file( "item_desc_format.html" );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qDebug() << "error opening file. Error: " << file.error();
    }
    m_descHtmlTemplate = file.readAll();

    this->setWindowTitle( "Supernova Assistant: Ship Designer" );

    connect( m_itemModel, SIGNAL( componentsChanged( const SNItem &, quint64 ) ), m_detailedModel, SLOT( item_changed( const SNItem&,quint64) ));
    connect( m_itemModel, SIGNAL( statsChanged( int, quint64 ) ), this, SLOT( statsChangedSlot( int, quint64 ) ) );

    setupDesignsModel();

    /* you cannot adjust the stretch factor of layouts in
     * a splitter. So we set proportional sizes here*/
    QList<int> sizes;
    sizes.push_back(800);
    sizes.push_back(200);
    m_ui->splitter->setSizes(sizes);
}

ShipDesigner::~ShipDesigner()
{
    delete m_ui;
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
    return QObject::eventFilter( obj, evt );
}

void ShipDesigner::on_componentList_clicked( QModelIndex index )
{
    if ( m_descHtmlTemplate == "" )
        return;
    ComponentsModel *model = dynamic_cast<ComponentsModel*>( m_ui->componentList->model() );
    QVariant data = model->data( index, SN::ComponentRole );
    if ( data.canConvert<SNItem>() )
    {
        SNItem item = data.value<SNItem>();
        QLocale locale;
        QString format = m_descHtmlTemplate;
        format.replace( "{DESC}", item.desc() );
        format.replace( "{TONS}", locale.toString( item.weight() ) );
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
        m_ui->componentDesc->setHtml( format );
    } else {
        if( m_ui->componentList->isExpanded( index ) )
            m_ui->componentList->collapse( index );
        else
            m_ui->componentList->expand( index );
        return;
    }
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

    QList< QPair<SNItem, quint64> > items = m_itemModel->getItems();

    for( int i = 0; i < items.size(); i++ )
    {
        QPair<SNItem, quint64> p = items.at(i);
        qDebug() << "appending: " << p.first.name();
        if( p.second > 0 )
            design.addComponent( p.first.name(), p.second );
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
        on_designsCombo_currentIndexChanged( 0 );
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
    // Set item count total
    int curr = m_ui->countLabel->text().toInt();

    // set tonnage
    QString tmp = m_ui->tonnageLabel->text();
    tmp.chop(5);
    quint64 tonnage = tmp.toUInt();
    m_ui->tonnageLabel->setText( QString::number( tonnage + tons ) );
    m_ui->countLabel->setText( QString::number( curr + numitems ) );

    // set AP
    m_ui->apsLabel->setText( QString::number( m_itemModel->actionPoints() ) );
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

void ShipDesigner::on_desiredAPSpin_valueChanged(int desired_ap)
{

}
