#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "data/Empire.h"



class ShipDesigner;
class ItemBrowser;
class WelcomeWidget;
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
    Empire m_currEmpire;
    ShipDesigner *m_shipDesigner;
    ItemBrowser *m_itemBrowser;
    WelcomeWidget *m_welcomeWidget;

    QMap<QWidget *, int> m_indexes;

    bool checkDataPath();
    void setupDatabase();
    void setupEmpiresModel();
//    void killDialogs();


private slots:

    void on_actionSelect_Empire_triggered();
    void on_actionQuit_triggered();
    void on_actionItem_Editor_triggered();
    void on_actionShip_Designer_triggered();
    /*
//    void on_createEmpireBut_clicked();
//    void on_deleteEmpireBut_clicked();
//    void on_empireCombo_currentIndexChanged( int );

    void on_actionCreate_an_Empire_triggered();*/

};

#endif // MAINWINDOW_H
