#ifndef TURNPARSER_H
#define TURNPARSER_H

#include "data/SNItem.h"
#include "data/System.h"

#include <QString>
#include <QHash>
#include <QStringList>
#include <QPair>
#include <QThread>

typedef QPair<QString, QString> CategoryPair;
class TurnParserTest;
class TurnParser : public QThread
{
    Q_OBJECT
public:
    TurnParser(  QObject * parent = 0 );
    TurnParser( const QString &filename ,  QObject * parent = 0);

    QString text() const;
    void writeOut( const QString &filename );



signals:

    void anzParsingComplete( const QList<SNItem> & );
    void ssParsingComplete( const QList<System> & );

protected:
    void go();
    void run();

private:
    void parseData();

    //debug data
    void debugData( const QString &directory );

    // SS (System Scan)
    const QList<System> parseSS();
    const System parseSS( const QString &data );

    // ANZ
    const QList<SNItem> parseANZs();
    CategoryPair mapClassificationToCategory( const QString &name, const QString &classification, bool istech ) const;


    QString m_filename;
    QString m_text;
    bool m_parsed;

    QList<SNItem> m_anzs;
    QMultiHash<QString, QString> m_data;
    friend class TurnParserTest;


    //utility functions

    static QString toProper( const QString &str );


};

#endif // TURNPARSER_H
