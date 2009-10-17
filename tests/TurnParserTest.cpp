#include "TurnParserTest.h"

#include "../TurnParser.h"

#include <QFile>
TurnParserTest::TurnParserTest()
{
}

void TurnParserTest::parseANZs()
{
     QFile file("tests/anz_test.txt");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     QString anz = *in.string();

     QStringList anzs;
     anzs << anz;

     TurnParser tp;
     tp.parseANZs(anzs);
}

QTEST_MAIN(TurnParserTest)
//#include "tests/moc_TurnParserTest.cxx"
