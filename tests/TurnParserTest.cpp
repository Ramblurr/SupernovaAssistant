#include "TurnParserTest.h"

#include "TurnParser.h"

#include "data/SNItem.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QList>
TurnParserTest::TurnParserTest()
{
}

void TurnParserTest::parseANZs()
{
    QVERIFY( 1 == 1 );
     QFile file(":/tests/anz_test.txt");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     QString anz = in.readAll();

     QStringList anzs;
     anzs << anz;

     TurnParser tp;
     QList<SNItem> items = tp.parseANZs(anzs);

     QVERIFY(items.size() == 1);

     SNItem item = items.front();
     QVERIFY( item.name() == "Mk I Fusion Engine");
     QVERIFY( item.structure() == 80);
     QVERIFY( item.category() == "Jump Drive");
     QVERIFY( item.weight() == 100);

//     qDebug() << " effects# "  <<item.getEffects().size();
//     foreach(ItemEffect e, item.getEffects() )
//     {
//         qDebug() << "(" << e.name() << ", val: " << e.value() << ", pval: " << e.prettyValue() << ", counters: " << e.counter() << ")";
//     }
     QVERIFY( item.getEffects().size() == 16);
     QVERIFY( item.getComponents().size() == 4);

//     QFile file2(":/tests/296_2.pdf");
//     if (!file2.open(QIODevice::ReadOnly))
//         return;
//
//     QByteArray array = file2.readAll();
//     TurnParser tp2(file2.absoluteFilePath());
////
////     items = tp2.parseANZs();
////
////     QVERIFY( items.size() == 22);

}

QTEST_MAIN(TurnParserTest)
