#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ShipDesigner.h"
#include "NewEmpireDialog.h"
#include "ChangeEmpireDialog.h"
#include "ItemBrowser.h"
#include "Empire.h"
#include "TurnParser.h"
#include "WelcomeWidget.h"
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
    setWindowIcon( QIcon(":/icons/supernova-16x16.png" ) );
//    setupEmpiresModel();

    m_welcomeWidget = new WelcomeWidget(this);
    int i = ui->stackedWidget->addWidget( m_welcomeWidget );
    ui->stackedWidget->setCurrentIndex(i);
    m_indexes.insert(m_welcomeWidget, i);

    connect(m_welcomeWidget, SIGNAL(currEmpireChanged(Empire)), this, SLOT(currEmpireChangedSlot(Empire)));

    currEmpireChangedSlot( m_welcomeWidget->currentEmire() );

}

MainWindow::~MainWindow()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    db.close();
    delete ui;
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

void MainWindow::currEmpireChangedSlot( const Empire & emp )
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
        db.close();
    QSqlDatabase::removeDatabase("CurrEmpire");

    m_currEmpire = emp;
    if( emp.name() == "" )
        return;

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
    if ( m_shipDesigner == 0)
    {
        m_shipDesigner = new ShipDesigner( this );
        int i = ui->stackedWidget->addWidget(m_shipDesigner);
        m_indexes.insert(m_shipDesigner, i);
        connect(m_welcomeWidget, SIGNAL(currEmpireChanged(Empire)), m_shipDesigner, SLOT(currEmpireChangedSlot()));
    }
    ui->stackedWidget->setCurrentIndex( m_indexes.value( m_shipDesigner ) );

}

void MainWindow::on_actionItem_Editor_triggered()
{
    if ( m_currEmpire.name() == "" )
    {
        QMessageBox::warning( 0, "No Empire Selected", "Please select an empire." );
        return;
    }
    if( m_itemBrowser == 0 )
    {
        m_itemBrowser = new ItemBrowser( this );
        int i = ui->stackedWidget->addWidget(m_itemBrowser);
        m_indexes.insert(m_itemBrowser, i);
        connect(m_welcomeWidget, SIGNAL(currEmpireChanged(Empire)), m_itemBrowser, SLOT(currEmpireChangedSlot()));
    }
    ui->stackedWidget->setCurrentIndex( m_indexes.value( m_itemBrowser ) );
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionSelect_Empire_triggered()
{
    ui->stackedWidget->setCurrentIndex( m_indexes.value(m_welcomeWidget) );
}
