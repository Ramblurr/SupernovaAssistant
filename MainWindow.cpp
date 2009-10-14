#include "MainWindow.h"
#include "ui/ui_MainWindow.h"
#include "ShipDesigner.h"
#include "dialogs/NewEmpireDialog.h"
#include "dialogs/ChangeEmpireDialog.h"
#include "dialogs/ItemBrowser.h"
#include "data/Empire.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QtSql>
#include <QStringListModel>
#include <QMessageBox>

MainWindow::MainWindow( QWidget *parent )
        : QMainWindow( parent ), ui( new Ui::MainWindowClass ), m_shipDesigner( 0 ), m_itemBrowser(0)
{
    ui->setupUi( this );
    setWindowTitle( tr( "Supernova Assistant" ) );
    setWindowIcon( QIcon("icons/supernova-16x16.png" ) );
    setupEmpiresModel();

}

MainWindow::~MainWindow()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    db.close();
    delete ui;
}
void MainWindow::setupEmpiresModel()
{
    ui->empireName->setText("No Empire Selected");
    ui->empireCombo->clear();
    QList<Empire> items;
    QSettings settings( "SN", "SNAssistant" );
    settings.beginGroup("empires");
    QStringList list = settings.childKeys();
    foreach( QString id, list )
    {
        Empire emp = settings.value(id).value<Empire>();
        if ( emp.name() == "" )
            continue;
        items << emp;
        ui->empireCombo->addItem( emp.name(), emp);

    }
    settings.endGroup();
//    int empireCount = settings.value( "empireCount", -1 ).toInt();
//    for ( int i = 0; i <= empireCount; i++ )
//    {
//        Empire blank_emp();
//        Empire emp = settings.value( "empires/" + QString::number( i ), blank_emp ).value<Empire>();
//        if ( emp.name() == "" )
//            continue;
//        items << emp;
//        ui->empireCombo->addItem( emp.name(), emp);
//    }
    if ( items.count() > 0 )
    {
        m_currEmpire = items.at( 0 );
        ui->empireName->setText( m_currEmpire.name() );
    }
}

bool MainWindow::checkDataPath()
{
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    if ( !dataLoc.cd( "SNAssistant" ) )
        if ( !dataLoc.mkdir( "SNAssistant" ) )
            return false; // can't make it
        else if ( !dataLoc.cd( "SNAssistant" ) )
            return false; // make it but can't cd to it
    return true;
}

void MainWindow::setupDatabase()
{
//    QSqlDatabase db = QSqlDatabase::addDatabase();
}

void MainWindow::on_actionCreate_an_Empire_triggered()
{
    if ( !checkDataPath() )
        return;
    NewEmpireDialog diag( this );
    int res = diag.exec();
    if ( res == QDialog::Accepted )
        setupEmpiresModel();

}
void MainWindow::on_empireCombo_currentIndexChanged( int id )
{
    Empire emp = ui->empireCombo->itemData( id ).value<Empire>();
    if ( emp.name() == "" )
        return;
    emit ( currEmpireChangedSlot( emp ) );
}

void MainWindow::on_actionSelect_Empire_triggered()
{
    ChangeEmpireDialog diag( this );
    diag.exec();
}

void MainWindow::currEmpireChangedSlot( const Empire & emp )
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        db.close();
    }
    QSqlDatabase::removeDatabase("CurrEmpire");
    m_currEmpire = emp;
    ui->empireName->setText( emp.name() );
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    dataLoc.cd( "SNAssistant" );
    db = QSqlDatabase::addDatabase( "QSQLITE", "CurrEmpire" );
    QString dbPath = dataLoc.absolutePath() + QDir::separator() + "empire_" + m_currEmpire.id() + ".sql";
    db.setDatabaseName( dbPath );
    bool ok = db.open();
    if( !ok )
        qDebug() << "Failed opening DB at " << dbPath << db.lastError();
}

void MainWindow::on_actionShip_Designer_triggered()
{
    if ( m_currEmpire.name() == "" )
    {
        QMessageBox::warning( 0, "No Empire Selected", "Please select an empire." );
        return;
    }
    if ( !m_shipDesigner )
    {
        m_shipDesigner = new ShipDesigner( m_currEmpire.id(), this );
    } else if ( m_shipDesigner->empireId() != m_currEmpire.id() )
    {
        m_shipDesigner = new ShipDesigner( m_currEmpire.id(), this );  // Qt should delete the old ship designer automatically
    }
    m_shipDesigner->show();
    m_shipDesigner->raise();
    m_shipDesigner->activateWindow();

}

void MainWindow::on_actionItem_Editor_triggered()
{
    if ( m_currEmpire.name() == "" )
    {
        QMessageBox::warning( 0, "No Empire Selected", "Please select an empire." );
        return;
    }
    if( !m_itemBrowser )
        m_itemBrowser = new ItemBrowser( this );

    m_itemBrowser->show();
    m_itemBrowser->raise();
    m_itemBrowser->activateWindow();

}

void MainWindow::on_deleteEmpireBut_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        db.close();
    }
    int idx = ui->empireCombo->currentIndex();
    Empire emp = ui->empireCombo->itemData( idx ).value<Empire>();
    QSettings settings( "SN", "SNAssistant" );

    settings.beginGroup("empires");
    settings.remove(emp.id());
    settings.endGroup();

    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    dataLoc.cd( "SNAssistant" );
    QString dbPath = dataLoc.absolutePath() + QDir::separator() + "empire_" + emp.id() + ".sql";
    QFile dbf( dbPath );
    dbf.remove();
    setupEmpiresModel();
}

void MainWindow::on_createEmpireBut_clicked()
{
     on_actionCreate_an_Empire_triggered();
}
