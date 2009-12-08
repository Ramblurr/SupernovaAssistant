#include "TurnParserTest.h"

#include "TurnParser.h"

#include "data/SNItem.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QList>
TurnParserTest::TurnParserTest()
{
    DEBUG_BLOCK
            qRegisterMetaType<SNItem>( "SNItem" );
    qRegisterMetaType<SNItem>( "System" );
    qRegisterMetaType<QList<SNItem> >( "QList<SNItem>" );
    qRegisterMetaType<QList<System> >( "QList<System>" );
    parser = new TurnParser("296_0.pdf", 0);
    parser->go();
}

void TurnParserTest::sss()
{
    DEBUG_BLOCK

    QList<System> items = parser->parseSS();
    QVERIFY(items.size() == 1);
    System sys = items.front();

    QVERIFY( sys.name() == "SOEBESLAV" );
    QVERIFY( sys.starType() == "S (Dim Red) 1 IV" );

    QVERIFY( sys.starSize() == "Single Star" );
}

void TurnParserTest::anzs()
{
    DEBUG_BLOCK
    QList<SNItem> items = parser->parseANZs();
    QVERIFY(items.size() == 1);

    SNItem item = items.front();
    QVERIFY( item.name() == "Iron");
    QVERIFY( item.structure() == 0);
    QVERIFY( item.category() == "Resource");
    QVERIFY( item.subcategory() == "Initial");
    QVERIFY( item.weight() == 1);

    //     qDebug() << " effects# "  <<item.getEffects().size();
    //     foreach(ItemEffect e, item.getEffects() )
    //     {
    //         qDebug() << "(" << e.name() << ", val: " << e.value() << ", pval: " << e.prettyValue() << ", counters: " << e.counter() << ")";
    //     }
//    QVERIFY( item.getEffects().size() == 16);
//    QVERIFY( item.getComponents().size() == 4);

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
