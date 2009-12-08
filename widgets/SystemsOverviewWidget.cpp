#include "SystemsOverviewWidget.h"
#include "ui_SystemsOverviewWidget.h"

#include "Debug.h"
SystemsOverviewWidget::SystemsOverviewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemsOverviewWidget)
{
    ui->setupUi(this);
    debug() << "HI!";
}

SystemsOverviewWidget::~SystemsOverviewWidget()
{
    delete ui;
}

void SystemsOverviewWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SystemsOverviewWidget::currEmpireChangedSlot()
{

}
