#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ShipDesigner.h"
#include "dialogs/NewEmpireDialog.h"
#include "dialogs/ChangeEmpireDialog.h"
#include "data/Empire.h"
#include "models/EmpiresModel.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QtSql>
#include <QStringListModel>
#include <QMessageBox>

MainWindow::MainWindow( QWidget *parent )
        : QMainWindow( parent ), ui( new Ui::MainWindowClass ), m_empiresModel( 0 ), m_shipDesigner( 0 )
{
    ui->setupUi( this );
    setWindowTitle( tr( "Supernova Assistant" ) );
    setupEmpiresModel();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setupEmpiresModel()
{
    QList<Empire> items;
    QSettings settings( "SN", "SNAssistant" );
    int empireCount = settings.value( "empireCount", -1 ).toInt();
    for ( int i = 0; i <= empireCount; i++ )
    {
        Empire blank_emp();
        Empire emp = settings.value( "empires/" + QString::number( i ), blank_emp ).value<Empire>();
        if ( emp.name() == "" )
            continue;
        items << emp;
    }
    if ( m_empiresModel )
        delete m_empiresModel;
    m_empiresModel = new EmpiresModel( items, this );
    if ( items.count() > 0 )
    {
        m_currEmpire = items.at( 0 );
        ui->empireName->setText( m_currEmpire.name() );
    }
    ui->empireCombo->setModel( m_empiresModel );
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
void MainWindow::on_shipDesignerButton_clicked()
{
    qDebug() << "curr emp" << m_currEmpire.name();
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
    Empire emp = m_empiresModel->data( m_empiresModel->index( id, 0, QModelIndex() ), EmpiresModel::EmpireRole ).value<Empire>();
    if ( emp.name() == "" )
        return;
    m_currEmpire = emp;
    ui->empireName->setText( emp.name() );
}

void MainWindow::on_actionSelect_Empire_triggered()
{
    ChangeEmpireDialog diag( this );
    int res = diag.exec();
}

void MainWindow::currEmpireChangedSlot( const Empire & )
{

}
