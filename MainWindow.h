#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "data/Empire.h"

#include <QtGui/QMainWindow>

class ShipDesigner;
class EmpiresModel;

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void currEmpireChangedSlot( const Empire & );
private:
    Ui::MainWindowClass *ui;
    EmpiresModel *m_empiresModel;
    Empire m_currEmpire;
    ShipDesigner *m_shipDesigner;

    bool checkDataPath();
    void setupDatabase();
    void setupEmpiresModel();


private slots:
    void on_actionSelect_Empire_triggered();
    void on_empireCombo_currentIndexChanged( int );
    void on_actionCreate_an_Empire_triggered();
    void on_shipDesignerButton_clicked();
};

#endif // MAINWINDOW_H
