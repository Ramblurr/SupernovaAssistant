#ifndef TURNPARSER_H
#define TURNPARSER_H

#include "data/SNItem.h"

#include <QString>
#include <QHash>
#include <QStringList>
#include <QPair>
#include <QThread>

typedef QPair<QString, QString> CategoryPair;
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

protected:
    void run();

private:
    void parseData();

    // SS (System Scan)
    void parseSS();

    // ANZ
    void parseANZs( const QStringList &anzs  );
    void parseANZs();
    CategoryPair mapClassificationToCategory( const QString &name, const QString &classification, bool istech ) const;


    QString m_filename;
    QString m_text;
    bool m_parsed;

    QList<SNItem> m_anzs;
    QMultiHash<QString, QString> m_data;
};

#endif // TURNPARSER_H
