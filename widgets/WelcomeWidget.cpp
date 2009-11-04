#include "WelcomeWidget.h"
#include "ui_WelcomeWidget.h"

#include "ShipDesigner.h"
#include "dialogs/NewEmpireDialog.h"
#include "dialogs/ChangeEmpireDialog.h"
#include "dialogs/ItemBrowser.h"
#include "data/Empire.h"
#include "TurnParser.h"
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QtSql>
#include <QStringListModel>
#include <QMessageBox>

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::WelcomeWidget)
{
    m_ui->setupUi(this);
    setupEmpiresModel();
    connect( this, SIGNAL(currEmpireChanged(Empire)), this, SLOT(currEmpireChangedSlot(Empire)));
}

WelcomeWidget::~WelcomeWidget()
{
    delete m_ui;
}

void WelcomeWidget::changeEvent(QEvent *e)
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


void WelcomeWidget::setupEmpiresModel()
{
    m_ui->empireName->setText("No Empire Selected");
    m_ui->empireCombo->clear();
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
        m_ui->empireCombo->addItem( emp.name(), emp);

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
//        m_ui->empireCombo->addItem( emp.name(), emp);
//    }
    if ( items.count() > 0 )
    {
        m_currEmpire = items.at( 0 );
        m_ui->empireName->setText( m_currEmpire.name() );
    }
    emit currEmpireChanged( m_currEmpire );
}

bool WelcomeWidget::checkDataPath()
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

void WelcomeWidget::on_empireCombo_currentIndexChanged( int id )
{
    Empire emp = m_ui->empireCombo->itemData( id ).value<Empire>();
    emit currEmpireChanged( emp );
}

void WelcomeWidget::currEmpireChangedSlot( const Empire & emp )
{
    m_ui->empireName->setText( emp.name() );
    int idx = m_ui->empireCombo->currentIndex();
    Empire emp_old = m_ui->empireCombo->itemData( idx ).value<Empire>();

    if( emp.id() != emp_old.id() )
    {
        int idx1 = m_ui->empireCombo->findData(emp);
        m_ui->empireCombo->setCurrentIndex( idx1 );
    }
}

void WelcomeWidget::on_deleteEmpireBut_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("CurrEmpire");
    if ( db.isOpen() )
    {
        db.close();
    }
    int idx = m_ui->empireCombo->currentIndex();
    Empire emp = m_ui->empireCombo->itemData( idx ).value<Empire>();
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

//    killDialogs();
}

void WelcomeWidget::on_createEmpireBut_clicked()
{
    if ( !checkDataPath() )
        return;
    NewEmpireDialog diag( this );
    int res = diag.exec();
    if ( res == QDialog::Accepted )
        setupEmpiresModel();
}

