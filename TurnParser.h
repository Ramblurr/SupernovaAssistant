#ifndef TURNPARSER_H
#define TURNPARSER_H

#include "data/SNItem.h"

#include <QString>
#include <QHash>
#include <QStringList>
#include <QPair>
typedef QPair<QString, QString> CategoryPair;
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
    CategoryPair mapClassificationToCategory( const QString &name, const QString &classification ) const;
    void parseData();
    QString m_filename;
    QString m_text;
    bool m_parsed;

    QMultiHash<QString, QString> m_data;
};

#endif // TURNPARSER_H
