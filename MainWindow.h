#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "data/Empire.h"



class ShipDesigner;
class ItemBrowser;

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

    bool checkDataPath();
    void setupDatabase();
    void setupEmpiresModel();


private slots:
    void on_createEmpireBut_clicked();
    void on_deleteEmpireBut_clicked();
    void on_actionItem_Editor_triggered();
    void on_actionSelect_Empire_triggered();
    void on_empireCombo_currentIndexChanged( int );
    void on_actionCreate_an_Empire_triggered();
    void on_actionShip_Designer_triggered();
};

#endif // MAINWINDOW_H
