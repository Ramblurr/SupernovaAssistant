#ifndef TURNPARSERTEST_H
#define TURNPARSERTEST_H

#include <QObject>
#include <QtTest/QtTest>

class TurnParserTest : public QObject
{
    Q_OBJECT
public:
    TurnParserTest();

private slots:
    void parseANZs();
};

#endif // TURNPARSERTEST_H
