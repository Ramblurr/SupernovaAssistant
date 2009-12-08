#ifndef TURNPARSERTEST_H
#define TURNPARSERTEST_H

#include "TurnParser.h"
#include "System.h"
#include "SNItem.h"

#include <QObject>
#include <QtTest/QtTest>

class TurnParserTest : public QObject
{
    Q_OBJECT
public:
    TurnParserTest();

private slots:
    void anzs();
    void sss();

private:
    TurnParser* parser;
};

#endif // TURNPARSERTEST_H
