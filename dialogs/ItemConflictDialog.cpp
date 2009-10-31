#include "ItemConflictDialog.h"
#include "ui_ItemConflictDialog.h"

#include "SNItem.h"
#include "GenericDelegate.h"
#include "IntegerColumnDelegate.h"

#include "diff_match_patch.h"

#include <QDebug>

ItemConflictDialog::ItemConflictDialog( const SNItem &olditem, const SNItem &newitem, QWidget *parent ) :
    QDialog(parent),
    m_ui(new Ui::ItemConflictDialog),
    m_oldItem( olditem ),
    m_newItem( newitem )
{
    m_ui->setupUi(this);
    setWindowTitle("Existing Item Found");

    m_ui->oldeffectsTable->setColumnCount(4);
    m_ui->oldeffectsTable->verticalHeader()->hide();
    m_ui->oldeffectsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->oldeffectsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    m_ui->oldeffectsTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Pretty Value"));
    m_ui->oldeffectsTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Counters"));
    m_ui->oldeffectsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->oldeffectsTable->setAlternatingRowColors( true );

//    GenericDelegate *generic = new GenericDelegate( this );
//    IntegerColumnDelegate *valueDelegate = new IntegerColumnDelegate(0, INT_MAX);
//    generic->insertColumnDelegate(1, valueDelegate );
//    m_ui->oldeffectsTable->setItemDelegate( generic );

    // Setup Component table
    m_ui->oldmaterialsTable->setColumnCount( 2 );
    m_ui->oldmaterialsTable->verticalHeader()->hide();
    m_ui->oldmaterialsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->oldmaterialsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Quantity"));
    m_ui->oldmaterialsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->oldmaterialsTable->viewport()->setAcceptDrops( true );

//    generic = new GenericDelegate( this );
//    IntegerColumnDelegate *quantityDelegate = new IntegerColumnDelegate(0, INT_MAX);
//    generic->insertColumnDelegate(1, quantityDelegate );
//    m_ui->oldmaterialsTable->setItemDelegate( generic );

    m_ui->neweffectsTable->setColumnCount(4);
    m_ui->neweffectsTable->verticalHeader()->hide();
    m_ui->neweffectsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->neweffectsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    m_ui->neweffectsTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Pretty Value"));
    m_ui->neweffectsTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Counters"));
    m_ui->neweffectsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->neweffectsTable->setAlternatingRowColors( true );

//    GenericDelegate *generic_new = new GenericDelegate( this );
//    IntegerColumnDelegate *valueDelegate_new = new IntegerColumnDelegate(0, INT_MAX);
//    generic_new->insertColumnDelegate(1, valueDelegate_new );
//    m_ui->neweffectsTable->setItemDelegate( generic_new );

    // Setup Component table
    m_ui->newmaterialsTable->setColumnCount( 2 );
    m_ui->newmaterialsTable->verticalHeader()->hide();
    m_ui->newmaterialsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    m_ui->newmaterialsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Quantity"));
    m_ui->newmaterialsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_ui->newmaterialsTable->viewport()->setAcceptDrops( true );

//    generic = new GenericDelegate( this );
//    IntegerColumnDelegate *quantityDelegate_new = new IntegerColumnDelegate(0, INT_MAX);
//    generic->insertColumnDelegate(1, quantityDelegate_new );
//    m_ui->newmaterialsTable->setItemDelegate( generic );

    m_ui->olddescEdit->setAcceptRichText(true);
    m_ui->newdescEdit->setAcceptRichText( true );

    populateFields();
    showDiffs();
}

ItemConflictDialog::~ItemConflictDialog()
{
    delete m_ui;
}

void ItemConflictDialog::changeEvent(QEvent *e)
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

void ItemConflictDialog::on_saveOldBut_clicked()
{
    done( SN::KeepOldItem );
}

void ItemConflictDialog::on_saveNewBut_clicked()
{
    done( SN::KeepNewItem );
}

void ItemConflictDialog::showDiffs()
{

//    if( m_oldItem.name() != m_newItem.name() )
//    {
//        QPalette pold = m_ui->oldnameEdit->palette();
//        QPalette pnew = m_ui->newnameEdit->palette();
//        pold.setColor(QPalette::Text, Qt::red);
//        pnew.setColor(QPalette::Text, Qt::red);
//        m_ui->oldnameEdit->setPalette(pold);
//        m_ui->newnameEdit->setPalette(pnew);
//    }

    if( m_oldItem.desc() != m_newItem.desc() )
    {
        diff_match_patch dmp;
        QList<Diff> diffs = dmp.diff_main(m_oldItem.desc(), m_newItem.desc() );
        dmp.diff_cleanupSemantic(diffs);
        QString diff = dmp.diff_prettyHtml(diffs);
        m_ui->newdescEdit->setHtml( diff );
    }

    if( m_oldItem.category() != m_newItem.category() )
    {
        QPalette pold = m_ui->oldcategoryCombo->palette();
        QPalette pnew = m_ui->newcategoryCombo->palette();
        pold.setColor(QPalette::Text, Qt::red);
        pnew.setColor(QPalette::Text, Qt::red);
        m_ui->oldcategoryCombo->setPalette(pold);
        m_ui->newcategoryCombo->setPalette(pnew);
    }

    if( m_oldItem.subcategory() != m_newItem.subcategory() )
    {
        QPalette pold = m_ui->oldsubcatCombo->palette();
        QPalette pnew = m_ui->newsubcatCombo->palette();
        pold.setColor(QPalette::Text, Qt::red);
        pnew.setColor(QPalette::Text, Qt::red);
        m_ui->oldsubcatCombo->setPalette(pold);
        m_ui->newsubcatCombo->setPalette(pnew);
    }

    if( m_oldItem.weight() != m_newItem.weight() )
    {
        QPalette pold = m_ui->oldtonsSpin->palette();
        QPalette pnew = m_ui->newtonsSpin->palette();
        pold.setColor(QPalette::Text, Qt::red);
        pnew.setColor(QPalette::Text, Qt::red);
        m_ui->oldtonsSpin->setPalette(pold);
        m_ui->newtonsSpin->setPalette(pnew);
    }

    if( m_oldItem.structure() != m_newItem.structure() )
    {
        QPalette pold = m_ui->oldstructureSpin->palette();
        QPalette pnew = m_ui->newstructureSpin->palette();
        pold.setColor(QPalette::Text, Qt::red);
        pnew.setColor(QPalette::Text, Qt::red);
        m_ui->oldstructureSpin->setPalette(pold);
        m_ui->newstructureSpin->setPalette(pnew);
    }

}

void ItemConflictDialog::populateFields()
{
    //OLD Item
    m_ui->oldnameEdit->setText( m_oldItem.name() );
    m_ui->olddescEdit->setPlainText( m_oldItem.desc() );
    m_ui->oldcategoryCombo->addItem(m_oldItem.category());
    m_ui->oldsubcatCombo->addItem(m_oldItem.subcategory());
    m_ui->oldtonsSpin->setValue( m_oldItem.weight() );
    m_ui->oldstructureSpin->setValue( m_oldItem.structure() );


    // Reset effects table
    for( int i=m_ui->oldeffectsTable->rowCount(); i >= 0 ; i-- )
        m_ui->oldeffectsTable->removeRow( i );

    foreach(ItemEffect effect, m_oldItem.getEffects() )
    {
        int row = m_ui->oldeffectsTable->rowCount();
        m_ui->oldeffectsTable->insertRow( row );
        QTableWidgetItem *item0 = new QTableWidgetItem( effect.name() );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( effect.value() ) );
        QTableWidgetItem *item2 = new QTableWidgetItem( effect.prettyValue() );
        QTableWidgetItem *item3 = new QTableWidgetItem( effect.counter() );
        m_ui->oldeffectsTable->setItem( row, 0, item0 );
        m_ui->oldeffectsTable->setItem( row, 1, item1 );
        m_ui->oldeffectsTable->setItem( row, 2, item2 );
        m_ui->oldeffectsTable->setItem( row, 3, item3 );
    }

    // Reset materials table
    for( int i=m_ui->oldmaterialsTable->rowCount(); i >= 0 ; i--)
        m_ui->oldmaterialsTable->removeRow( i );
    QMapIterator<QString, int> it( m_oldItem.getComponents() );
    while( it.hasNext() )
    {
        it.next();
        int row = m_ui->oldmaterialsTable->rowCount();
        m_ui->oldmaterialsTable->insertRow( row );
        QTableWidgetItem *item0 = new QTableWidgetItem( it.key() );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( it.value() ) );
        m_ui->oldmaterialsTable->setItem( row, 0, item0 );
        m_ui->oldmaterialsTable->setItem( row, 1, item1 );
    }

    m_ui->newnameEdit->setText( m_newItem.name() );
    m_ui->newdescEdit->setPlainText( m_newItem.desc() );
    m_ui->newcategoryCombo->addItem(m_newItem.category());
    m_ui->newsubcatCombo->addItem(m_newItem.subcategory());
    m_ui->newtonsSpin->setValue( m_newItem.weight() );
    m_ui->newstructureSpin->setValue( m_newItem.structure() );


    // Reset effects table
    for( int i=m_ui->neweffectsTable->rowCount(); i >= 0 ; i-- )
        m_ui->neweffectsTable->removeRow( i );

    foreach(ItemEffect effect, m_newItem.getEffects() )
    {
        int row = m_ui->neweffectsTable->rowCount();
        m_ui->neweffectsTable->insertRow( row );
        QTableWidgetItem *item0 = new QTableWidgetItem( effect.name() );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( effect.value() ) );
        QTableWidgetItem *item2 = new QTableWidgetItem( effect.prettyValue() );
        QTableWidgetItem *item3 = new QTableWidgetItem( effect.counter() );
        m_ui->neweffectsTable->setItem( row, 0, item0 );
        m_ui->neweffectsTable->setItem( row, 1, item1 );
        m_ui->neweffectsTable->setItem( row, 2, item2 );
        m_ui->neweffectsTable->setItem( row, 3, item3 );
    }

    // Reset materials table
    for( int i=m_ui->newmaterialsTable->rowCount(); i >= 0 ; i--)
        m_ui->newmaterialsTable->removeRow( i );
    QMapIterator<QString, int> it_new( m_newItem.getComponents() );
    while( it_new.hasNext() )
    {
        it_new.next();
        int row = m_ui->newmaterialsTable->rowCount();
        m_ui->newmaterialsTable->insertRow( row );
        QTableWidgetItem *item0 = new QTableWidgetItem( it_new.key() );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::number( it_new.value() ) );
        m_ui->newmaterialsTable->setItem( row, 0, item0 );
        m_ui->newmaterialsTable->setItem( row, 1, item1 );
    }
}
