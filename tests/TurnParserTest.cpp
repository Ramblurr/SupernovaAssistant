#include "TurnParserTest.h"

#include "TurnParser.h"

#include "SNItem.h"
#include "WarpPoint.h"
#include "Planet.h"

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
    parser = new TurnParser("29a6_0.pdf", 0);
//    parser->go();
}

void TurnParserTest::sss()
{
    DEBUG_BLOCK

    QFile f(":/tests/data/SS.txt");
    if (!f.open(QIODevice::ReadOnly))
        return;

    QByteArray array = f.readAll();
    QString data(array);

    System sys = parser->parseSS(data);

    QVERIFY( sys.name() == "Test1" );
    QVERIFY( sys.starType() == "S (Dim Red) 1 IV" );
    QVERIFY( sys.starSize() == "Single Star" );


    QVERIFY( sys.warpPointsCount() == 3);

    WarpPoint wp1(1234, "Test1", "C", 1594.1);
    WarpPoint wp2(12345, "Test1", "A", 2143.9);
    WarpPoint wp3(4321, "Test1", "C", 679.8);

    QVERIFY( sys.warpPoints().contains(wp1) );
    QVERIFY( sys.warpPoints().contains(wp2) );
    QVERIFY( sys.warpPoints().contains(wp3) );

    QVERIFY( sys.orbitsCount() == 8 );

    Planet p1("Test1-1","Test1", "1", "", "Hot Terrestrial", 0.4, 44733, "Chlorine", "");
    Planet p2("Test1-2","Test1", "2", "", "Hot Terrestrial", 0.75, 34089, "Chlorine", "");
    Planet p3("Test1-3","Test1", "3", "", "Asteroid Field", 1.1, 1, "Vacuum", "");
    Planet p4("Test1-4","Test1", "4", "", "Terrestrial", 1.8, 18468, "Carbon Dioxide", "");
    Planet p5("Test1-4a","Test1", "4", "a", "Moon", 0.0, 3233, "Nitrogen", "");
    Planet p6("Test1-4b","Test1", "4", "b", "Moon", 0.0, 2166, "Vacuum", "");
    Planet p7("Test1-4c","Test1", "4", "c", "Moon", 0.0, 3119, "Chlorine", "");
    Planet p8("Test1-5","Test1", "5", "", "Frozen Terrestrial", 3.2, 33430, "Vacuum", "");

    QVERIFY(sys.orbits().contains(p1));
    QVERIFY(sys.orbits().contains(p2));
    QVERIFY(sys.orbits().contains(p3));
    QVERIFY(sys.orbits().contains(p4));
    QVERIFY(sys.orbits().contains(p5));
    QVERIFY(sys.orbits().contains(p6));
    QVERIFY(sys.orbits().contains(p7));
    QVERIFY(sys.orbits().contains(p8));

}

void TurnParserTest::anzs()
{
    /*DEBUG_BLOCK
    QList<SNItem> items = parser->parseANZs();
    QVERIFY(items.size() == 1);

    SNItem item = items.front();
    QVERIFY( item.name() == "Iron");
    QVERIFY( item.structure() == 0);
    QVERIFY( item.category() == "Resource");
    QVERIFY( item.subcategory() == "Initial");
    QVERIFY( item.weight() == 1);*/

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
