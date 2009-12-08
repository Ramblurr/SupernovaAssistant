#include <QApplication>

//#include "Global.h"
#include "Debug.h"
#include "MainWindow.h"
#include "data/Empire.h"
#include "data/SNItem.h"
#include "data/System.h"


#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QMutex>
#include <QBuffer>
#include <QIODevice>

void myMessageOutput(QtMsgType type, const char *msg)
 {
     switch (type) {
     case QtDebugMsg:
         Debug::debug_log.write(msg);
         fprintf(stderr, "%s\n", msg);
         break;
     case QtWarningMsg:
         Debug::debug_log.write(msg);
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         Debug::debug_log.write(msg);
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         Debug::debug_log.write(msg);
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }

int main( int argc, char *argv[] )
{
    Debug::debug_log.open(QBuffer::ReadWrite);
    qInstallMsgHandler(myMessageOutput);
    QApplication a( argc, argv );
    QApplication::setApplicationName( "SNAssistant" );
//    #if defined( Q_WS_WIN )
//        a.setStyle( "windowsmodernstyle" );
//    #endif

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
    qRegisterMetaType<SNItem>( "SNItem" );
    qRegisterMetaType<SNItem>( "System" );
    qRegisterMetaType<QList<SNItem> >( "QList<SNItem>" );
    qRegisterMetaType<QList<System> >( "QList<System>" );
    qRegisterMetaTypeStreamOperators<Empire>( "Empire" );
    MainWindow w;
    w.show();
    return a.exec();
}
