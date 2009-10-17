#ifndef TURNPARSER_H
#define TURNPARSER_H

#include "data/SNItem.h"

#include <QString>
#include <QHash>
#include <QStringList>

class TurnParser
{
public:
    TurnParser();
    TurnParser( const QString &filename );

    QString text() const;
    void writeOut( const QString &filename );

    QList<SNItem> parseANZs( const QStringList &anzs  ) const;
    QList<SNItem> parseANZs() const;

private:
    void parseData();
    QString m_filename;
    QString m_text;
    bool m_parsed;

    QMultiHash<QString, QString> m_data;
};

#endif // TURNPARSER_H
