#ifndef DEBUG_H
#define DEBUG_H

#include "Global.h"

#include <QDebug>
#include <QApplication>
#include <QMutex>
#include <QObject>
#include <QBuffer>

#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>

#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#ifndef APP_PREFIX
#define APP_PREFIX "SNAssistant"
#endif

#ifdef GLOBAL
#define Global
#else
#define Global extern
#endif



/**
 * @namespace Debug
 * @short qDebug with indentation functionality and convenience macros
 * @author Max Howell <max.howell@methylblue.com>
 *
 * Usage:
 *
 *     #define DEBUG_PREFIX "Blah"
 *     #include "debug.h"
 *
 *     void function()
 *     {
 *        Debug::Block myBlock( __PRETTY_FUNCTION__ );
 *
 *        debug() << "output1" << endl;
 *        debug() << "output2" << endl;
 *     }
 *
 * Will output:
 *
 * app: BEGIN: void function()
 * app:   [Blah] output1
 * app:   [Blah] output2
 * app: END: void function(): Took 0.1s
 *
 * @see Block
 * @see CrashHelper
 * @see ListStream
 */

namespace Debug
{
    Global QMutex mutex;
    Global QBuffer debug_log;
    Global NoDebugStream devnull;

    // we can't use a statically instantiated QString for the indent, because
    // static namespaces are unique to each dlopened library. So we piggy back
    // the QString on the KApplication instance

    #define qOApp reinterpret_cast<QObject*>(qApp)
    class Indent : QObject
    {
        friend QString &modifieableIndent();
        Indent() : QObject( qOApp ) { setObjectName( "DEBUG_indent" ); }
        QString m_string;
    };

    inline QString &modifieableIndent()
    {
        QObject* o = qOApp ? qOApp->findChild<QObject*>( "DEBUG_indent" ) : 0;
        QString &ret = (o ? static_cast<Indent*>( o ) : new Indent)->m_string;
        return ret;
    }

    inline QString indent()
    {
        return modifieableIndent();
    }

    inline bool debugEnabled()
    {
//        KConfigGroup config = KGlobal::config()->group( "General" );
//        const bool debug = config.readEntry( "Debug Enabled", false );
//
//        return debug;
        return true;
    }

    inline QDebug dbgstream()
    {
        return debugEnabled ? QDebug( QtDebugMsg ) : QDebug( &devnull );
//        return QDebug( &debug_log );
    }

    #undef qOApp

    #ifndef DEBUG_PREFIX
    #define AMK_PREFIX ""
    #else
    #define AMK_PREFIX "[" DEBUG_PREFIX "]"
    #endif

    //from qDebug.h
    enum DebugLevels {
        qDebug_INFO  = 0,
        qDebug_WARN  = 1,
        qDebug_ERROR = 2,
        qDebug_FATAL = 3
    };


    static inline QDebug debug()   { mutex.lock(); QString ind = indent(); mutex.unlock(); return dbgstream() << endl << qPrintable( APP_PREFIX ": " + ind + AMK_PREFIX ); }
    static inline QDebug warning() { mutex.lock(); QString ind = indent(); mutex.unlock(); return dbgstream() << endl << qPrintable( APP_PREFIX ": " + ind + AMK_PREFIX + " [WARNING!]" ); }
    static inline QDebug error()   { mutex.lock(); QString ind = indent(); mutex.unlock(); return dbgstream() << endl << qPrintable( APP_PREFIX ": " + ind + AMK_PREFIX + " [ERROR!]" ); }
    static inline QDebug fatal()   { mutex.lock(); QString ind = indent(); mutex.unlock(); return dbgstream() << endl << qPrintable( APP_PREFIX ": " + ind + AMK_PREFIX ); }

    #undef AMK_PREFIX

    static inline void perfLog( const QString &message, const QString &func )
    {
#ifdef Q_OS_UNIX
        if( !debugEnabled() )
        {
            return;
        }
        QString str = QString( "MARK: %1: %2 %3" ).arg( QApplication::applicationName(), func, message );
        access( str.toLocal8Bit().data(), F_OK );
#endif
    }
}

using Debug::debug;
using Debug::warning;
using Debug::error;
using Debug::fatal;

/// Standard function announcer
#define DEBUG_FUNC_INFO { Debug::mutex.lock(); qDebug() << Debug::indent() ; Debug::mutex.unlock(); }

/// Announce a line
#define DEBUG_LINE_INFO { Debug::mutex.lock(); qDebug() << Debug::indent() << "Line: " << __LINE__; Debug::mutex.unlock(); }

#ifdef __SUNPRO_CC
#define __PRETTY_FUNCTION__ __FILE__
#endif

/// Convenience macro for making a standard Debug::Block
#define DEBUG_BLOCK Debug::Block uniquelyNamedStackAllocatedStandardBlock( __PRETTY_FUNCTION__ );

/// Use this to remind yourself to finish the implementation of a function
#define AMAROK_NOTIMPLEMENTED warning() << "NOT-IMPLEMENTED: " << __PRETTY_FUNCTION__ << endl;

/// Use this to alert other developers to stop using a function
#define AMAROK_DEPRECATED warning() << "DEPRECATED: " << __PRETTY_FUNCTION__ << endl;

/// Performance logging
#define PERF_LOG( msg ) { Debug::perfLog( msg, __PRETTY_FUNCTION__ ); }

namespace Debug
{
    /**
     * @class Debug::Block
     * @short Use this to label sections of your code
     *
     * Usage:
     *
     *     void function()
     *     {
     *         Debug::Block myBlock( "section" );
     *
     *         debug() << "output1" << endl;
     *         debug() << "output2" << endl;
     *     }
     *
     * Will output:
     *
     *     app: BEGIN: section
     *     app:  [prefix] output1
     *     app:  [prefix] output2
     *     app: END: section - Took 0.1s
     *
     */

    class Block
    {
        timeval     m_start;
        const char *m_label;

    public:
        Block( const char *label )
                : m_label( label )
        {
            if ( gettimeofday( &m_start, 0 ) == -1 )
                dbgstream() << endl << APP_PREFIX ": Block - gettimeofday failed with "
                            << strerror(errno);

            if( !debugEnabled() ) return;

            mutex.lock();

            dbgstream() << qPrintable( APP_PREFIX ": " + indent() + "BEGIN: " + label );
            Debug::modifieableIndent() += "  ";
            mutex.unlock();
        }

        ~Block()
        {
            if( !debugEnabled() ) return;

            mutex.lock();
            timeval end;
            int result = gettimeofday( &end, 0 );
            if( result == -1 )
            {
                mutex.unlock();
                return;
            }

            end.tv_sec -= m_start.tv_sec;
            if( end.tv_usec < m_start.tv_usec) {
                // Manually carry a one from the seconds field.
                end.tv_usec += 1000000;
                end.tv_sec--;
            }
            end.tv_usec -= m_start.tv_usec;

            double duration = double(end.tv_sec) + (double(end.tv_usec) / 1000000.0);

            Debug::modifieableIndent().truncate( Debug::indent().length() - 2 );

            // Print timing information, and a special message (DELAY) if the method took longer than 5s
            if( duration < 5.0 )
                dbgstream() << endl << qPrintable( APP_PREFIX ": " + indent() + "END__: " + m_label + " - Took " + QString::number( duration, 'g', 2 ) + "s" );
            else
                dbgstream() << endl << qPrintable( APP_PREFIX ": " + indent() + "END__: " + m_label + " - DELAY Took (quite long) " + QString::number( duration, 'g', 2 ) + "s" );

            mutex.unlock();
        }
    };


    /**
     * @name Debug::stamp()
     * @short To facilitate crash/freeze bugs, by making it easy to mark code that has been processed
     *
     * Usage:
     *
     *     {
     *         Debug::stamp();
     *         function1();
     *         Debug::stamp();
     *         function2();
     *         Debug::stamp();
     *     }
     *
     * Will output (assuming the crash occurs in function2()
     *
     *     app: Stamp: 1
     *     app: Stamp: 2
     *
     */

    inline void stamp()
    {
        static int n = 0;
        debug() << "| Stamp: " << ++n << endl;
    }
}


#include <QVariant>

namespace Debug
{
    /**
     * @class Debug::List
     * @short You can pass anything to this and it will output it as a list
     *
     *     debug() << (Debug::List() << anInt << aString << aQStringList << aDouble) << endl;
     */

    typedef QList<QVariant> List;
}

#endif // DEBUG_H

