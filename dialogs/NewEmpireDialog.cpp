#include "NewEmpireDialog.h"
#include "ui_NewEmpireDialog.h"
#include "data/Empire.h"

#include <QDesktopServices>
#include <QtSql>
#include <QSettings>
NewEmpireDialog::NewEmpireDialog( QWidget *parent ) :
        QDialog( parent ),
        m_ui( new Ui::NewEmpireDialog )
{
    m_ui->setupUi( this );
}

NewEmpireDialog::~NewEmpireDialog()
{
    delete m_ui;
}

void NewEmpireDialog::changeEvent( QEvent *e )
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

void NewEmpireDialog::on_acceptButton_clicked()
{
    QString id = m_ui->empireIDEdit->text().trimmed();
    QString name = m_ui->empireNameEdit->text().trimmed();
    QSettings settings( "SN", "SNAssistant" );
    int empireCount = settings.value( "empireCount", -1 ).toInt();
    empireCount++;
    Empire emp( name, id );

    bool ok = setupNewDatabase( name, id );
    if ( ok )
    {
        qDebug() << "Databse created succesfully";
        settings.setValue( "empireCount", empireCount );
        settings.setValue( "empires/" + QString::number( empireCount ), emp );
    }
    else
        qDebug() << "Error when creating dbase";

}


void NewEmpireDialog::on_empireIDEdit_textEdited( QString text )
{
    QSettings settings( "SN", "SNAssistant" );
    int empireCount = settings.value( "empireCount", -1 ).toInt();
    for ( int i = 0; i <= empireCount; i++ )
    {
        Empire blank_emp();
        Empire emp = settings.value( "empires/" + QString::number( i ), blank_emp ).value<Empire>();
        if ( emp.name() == "" )
            continue;
        if ( emp.id() == text.trimmed() )
        {
            m_ui->errorLabel->setText( "Empire ID already exists" );
            m_ui->acceptButton->setEnabled( false );
            return;
        }
        else
        {
            m_ui->errorLabel->setText( "" );
            m_ui->acceptButton->setEnabled( true );
        }
    }
}

bool NewEmpireDialog::setupNewDatabase( const QString &name, const QString &id )
{
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    dataLoc.cd( "SNAssistant" );
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString dbPath = dataLoc.absolutePath() + QDir::separator() + "empire_" + m_ui->empireIDEdit->text().trimmed() + ".sql";
    db.setDatabaseName( dbPath );
    if ( db.open() )
    {
        QString sql = "CREATE TABLE shipdesigns( design_id INTEGER PRIMARY KEY, name VARCHAR, type VARCHAR, class VARCHAR )";
        db.exec( sql );
        sql = "CREATE TABLE shipdesigns_lines( line_id INTEGER PRIMARY KEY, design_id INTEGER, item VARCHAR, quantity INTEGER )";
        db.exec( sql );
        db.close();
        return true;
    }
    return false;
}
