#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "data/Empire.h"

#include <QtGui/QWidget>

namespace Ui {
    class WelcomeWidget;
}

class WelcomeWidget : public QWidget {
    Q_OBJECT
public:
    WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

    Empire currentEmire() const { return m_currEmpire; }

signals:
    void currEmpireChanged( const Empire & );

public slots:
    void currEmpireChangedSlot( const Empire & );

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WelcomeWidget *m_ui;

    Empire m_currEmpire;
    bool checkDataPath();
    void setupDatabase();
    void setupEmpiresModel();
    //    void killDialogs();

private slots:
    void on_createEmpireBut_clicked();
    void on_deleteEmpireBut_clicked();
    void on_empireCombo_currentIndexChanged( int );
};

#endif // WELCOMEWIDGET_H
