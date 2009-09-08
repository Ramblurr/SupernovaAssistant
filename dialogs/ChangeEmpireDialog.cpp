#include "ChangeEmpireDialog.h"
#include "ui_ChangeEmpireDialog.h"

#include "models/EmpiresModel.h"
#include "data/Empire.h"

#include <QSettings>
#include <QList>
ChangeEmpireDialog::ChangeEmpireDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChangeEmpireDialog)
{
    m_ui->setupUi(this);

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
    m_empiresModel = new EmpiresModel( items, this );
    m_ui->empireCombo_3->setModel( m_empiresModel );
    
    connect(this, SIGNAL( currEmpireChanged( const Empire& ) ), parent, SLOT( currEmpireChangedSlot( const Empire & ) ) );
}

ChangeEmpireDialog::~ChangeEmpireDialog()
{
    delete m_ui;
}

void ChangeEmpireDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ChangeEmpireDialog::on_buttonBox_accepted()
{
    int id = m_ui->empireCombo_3->currentIndex();
    Empire emp = m_empiresModel->data( m_empiresModel->index( id, 0, QModelIndex() ), EmpiresModel::EmpireRole ).value<Empire>();
    if ( emp.name() == "" )
        return;
    emit( currEmpireChanged( emp ) );
}