#include "NewEmpireDialog.h"
#include "ui_NewEmpireDialog.h"
#include "data/Empire.h"

#include <QDesktopServices>
#include <QtSql>
#include <QFile>
#include <QSettings>

NewEmpireDialog::NewEmpireDialog( QWidget *parent ) :
        QDialog( parent ),
        m_ui( new Ui::NewEmpireDialog )
{
    m_ui->setupUi( this );
    m_ui->acceptButton->setEnabled( false );
    setWindowTitle("Create New Empire");
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
    Empire emp( name, id );

    bool ok = setupNewDatabase( name, id );

    if ( ok )
    {
        qDebug() << "Databse created succesfully";
//        settings.setValue( "empireCount", empireCount );
//        settings.setValue( "empires/" + QString::number( empireCount ), emp );
        settings.beginGroup("empires");
        settings.setValue( emp.id(), emp );
        settings.endGroup();
    }
    else
        qDebug() << "Error when creating dbase";

}


void NewEmpireDialog::on_empireIDEdit_textEdited( QString text )
{
    Q_UNUSED( text )
    m_ui->acceptButton->setEnabled( fieldsValidated() );
}
QStringList NewEmpireDialog::loadSchema( const QString &filename )
{
    QStringList queries;
    QFile file( filename );
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString query;
        bool trigger = false;
        while (!file.atEnd())
        {
            QString tmp = file.readLine().replace("\n", " ");
            if( tmp.startsWith("-- ") )
                continue;
            if( tmp.startsWith("CREATE TRIGGER", Qt::CaseInsensitive) )
                trigger = true;
            if( !trigger && tmp.trimmed().right(1) == ";"  )
            {
                query.append(tmp);
                queries << query;
//                qDebug() << "added: " << query;
                query = "";
            }
            else if( trigger && tmp.trimmed().endsWith("END;", Qt::CaseInsensitive ) )
            {
                trigger = false;
                query.append(tmp);
                queries << query;
//                qDebug() << "added: " << query;
                query = "";
            } else {
                query.append(tmp);
            }
        }
    }
    file.close();
    return queries;
}
bool NewEmpireDialog::setupNewDatabase( const QString &name, const QString &id )
{
    Q_UNUSED( name )
    Q_UNUSED( id )
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    dataLoc.cd( "SNAssistant" );
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString dbPath = dataLoc.absolutePath() + QDir::separator() + "empire_" + m_ui->empireIDEdit->text().trimmed() + ".sql";
    db.setDatabaseName( dbPath );
    if ( db.open() )
    {

        QStringList queries( loadSchema( ":/supernova.sql" ) );
        db.transaction();
        foreach(QString statement, queries)
        {
            QSqlQuery query( db );
            query.prepare( statement );
            if( !query.exec() )
                qDebug() << query.executedQuery() << "\nerror: " << query.lastError();
        }
        db.commit();
        db.close();
        return true;
    }
    db.close();
    return false;
}

void NewEmpireDialog::on_empireNameEdit_textEdited(QString )
{
    m_ui->acceptButton->setEnabled( fieldsValidated() );
}

bool NewEmpireDialog::fieldsValidated()
{
    bool ok = true;
    if( m_ui->empireNameEdit->text().trimmed().length() == 0 )
        ok = false;
    if( m_ui->empireIDEdit->text().trimmed().length() == 0 )
        ok = false;

    QSettings settings( "SN", "SNAssistant" );
//    int empireCount = settings.value( "empireCount", -1 ).toInt();

    settings.beginGroup("empires");
    QStringList list = settings.childKeys();
    foreach(QString id, list)
    {
        if( id == m_ui->empireIDEdit->text().trimmed() )
        {
            m_ui->errorLabel->setText( "Empire ID already exists" );
            ok = false;
        }
        else
            m_ui->errorLabel->setText( "" );
    }
    settings.endGroup();
//    for ( int i = 0; i <= empireCount; i++ )
//    {
//        Empire blank_emp();
//        Empire emp = settings.value( "empires/" + QString::number( i ), blank_emp ).value<Empire>();
//        if ( emp.name() == "" )
//            continue;
//        if ( emp.id() ==  m_ui->empireIDEdit->text().trimmed())
//        {
//            m_ui->errorLabel->setText( "Empire ID already exists" );
//            ok = false;
//        }
//        else
//            m_ui->errorLabel->setText( "" );
//    }

    return ok;
}
