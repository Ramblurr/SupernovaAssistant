#include <QtGui/QApplication>

#include "MainWindow.h"
#include "data/Empire.h"

#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    QString dataDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    QDir dataLoc( dataDir );
    if ( !dataLoc.exists() )
    {
        QMessageBox::critical( 0, "Data Storage Error", "The storage location could not be found.\n '" + dataDir + "'\nPlease create it." );
        exit( 1 );
    }
//    if( !dataLoc.mkdir("SNAssistant") )
//    {
//        QMessageBox::critical( 0, "Data Storage Error", "The storage location is not writeable.\n '" + dataDir);
//        exit(1);
//    }
    qRegisterMetaType<Empire>( "Empire" );
    qRegisterMetaTypeStreamOperators<Empire>( "Empire" );
    MainWindow w;
    w.show();
    return a.exec();
}
