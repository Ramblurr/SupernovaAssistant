#include "ShipDesigner.h"

#include "data/SNItem.h"
#include "data/ShipDesign.h"
#include "data/Order.h"
#include "ui/ui_ShipDesigner.h"
#include "models/ComponentsModel.h"
#include "models/ItemModel.h"
#include "models/MaterialsModel.h"

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QDebug>
#include <QtCore/QLocale>
#include <QtCore/QList>
#include <QEvent>
#include <QKeyEvent>

QList<SNItem>  ShipDesigner::m_components;// = new QList<SNItem>();

ShipDesigner::ShipDesigner( QString empid, QWidget *parent ) :
        QDialog( parent ),
        m_ui( new Ui::ShipDesigner ),
        m_empireId( empid )
{
    m_ui->setupUi( this );

    m_itemModel = new ItemModel( this );
    m_ui->itemList->setModel( m_itemModel );
    m_ui->itemList->installEventFilter( this );

    m_detailedModel = new MaterialsModel( this );
    m_ui->detailedTable->setModel( m_detailedModel );

    ShipDesigner::loadItems();

    QList<SNItem> items;

    for( int i=0; i<ShipDesigner::m_components.size(); i++)
        if( ShipDesigner::m_components.at(i).category() == "Ship Component" )
            items << ShipDesigner::m_components.at(i);
    m_componentsModel = new ComponentsModel( items );
    m_ui->componentList->setModel( m_componentsModel );

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
            if ( idx.isValid() )
                m_itemModel->removeRow( idx.row() );
        }
    }
}

void ShipDesigner::on_componentList_clicked( QModelIndex index )
{
    if ( m_descHtmlTemplate == "" )
        return;
    ComponentsModel *model = dynamic_cast<ComponentsModel*>( m_ui->componentList->model() );
    QVariant data = model->data( index, ComponentsModel::ComponentRole );
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
    }

}

bool ShipDesigner::loadItems()
{
    qDebug() << "loading items";
    QFile file( "items.xml" );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "file open failed";
        return false;
    }
    QDomDocument doc( "items" );
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
    QDomElement root = doc.documentElement();
    if ( root.tagName() != "items" )
        return false;

    QDomNode n = root.firstChild();
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == "item" )
        {
            QString name = n.firstChildElement( "name" ).text();
            QString desc = n.firstChildElement( "desc" ).text();
            QString weight = n.firstChildElement( "weight" ).text();
            QString category = n.firstChildElement( "category" ).text();
            if( weight == "" )
            {
                n = n.nextSibling();
                continue;
            }
            bool ok;
            SNItem item = SNItem( name, desc, category, weight.toInt( &ok ) );
            if ( !ok )
            {
                n = n.nextSibling();
                continue;
            }

            QDomNode n_comps = n.firstChildElement( "components" ).firstChild();
            while ( !n_comps.isNull() )
            {
                QDomElement e = n_comps.toElement();
                if ( !e.isNull() && e.tagName() == "component" )
                {
                    QString compname = e.text();
                    int quantity = e.attribute( "quantity", "0" ).toInt( &ok );
                    if ( ok )
                    {

                        item.addComponent( compname, quantity );
                    }
                    else
                        qWarning() << "Error parsing component '" << quantity << "' for item '" << name << "'";
                }
                n_comps = n_comps.nextSibling();
            }

            if ( ok )
                ShipDesigner::m_components.append( item );
        }
        n = n.nextSibling();
    }
    return true;
}

void ShipDesigner::on_saveButton_clicked()
{
    QString name = m_ui->designNameEdit->text().trimmed();
    if ( name.length() == 0 )
        return;
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

    bool ok = design.saveDesign( m_empireId );
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
    m_designs = ShipDesign::getDesigns( m_empireId );
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
    m_ui->tonnageLabel->setText("0 tons");
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
        SNItem item = ShipDesigner::getItem( it.key() );
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
    des.deleteDesign( m_empireId );
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
        SNItem item = idx.data( ComponentsModel::ComponentRole ).value<SNItem>();
        m_itemModel->appendData(item, 0);
    }
}

void ShipDesigner::statsChangedSlot(int numitems, quint64 tons )
{
    int curr = m_ui->countLabel->text().toInt();
    QString tmp = m_ui->tonnageLabel->text();
    tmp.chop(5);
    quint64 tonnage = tmp.toUInt();
    m_ui->tonnageLabel->setText( QString::number( tonnage + tons ) + " tons" );
    m_ui->countLabel->setText( QString::number( curr + numitems ) );
}

SNItem ShipDesigner::getItem( const QString &name )
{
    for (int i = 0; i < ShipDesigner::m_components.size(); ++i)
    {
        if (ShipDesigner::m_components.at(i).name() == name)
            return ShipDesigner::m_components.at(i);
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
