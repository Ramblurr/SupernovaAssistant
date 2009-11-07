#ifndef SYSTEMSOVERVIEWWIDGET_H
#define SYSTEMSOVERVIEWWIDGET_H

#include <QWidget>

namespace Ui {
    class SystemsOverviewWidget;
}

class SystemsOverviewWidget : public QWidget {
    Q_OBJECT
public:
    SystemsOverviewWidget(QWidget *parent = 0);
    ~SystemsOverviewWidget();\

public slots:
    void currEmpireChangedSlot();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SystemsOverviewWidget *ui;
};

#endif // SYSTEMSOVERVIEWWIDGET_H
