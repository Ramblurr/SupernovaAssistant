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
    f.close();
    QString data(array);

    System sys = parser->parseSS(data);

    QVERIFY( sys.name() == "TEST1" );
    QVERIFY( sys.starType() == "S (Dim Red) 1 IV" );
    QVERIFY( sys.starSize() == "Single Star" );


    QVERIFY( sys.warpPointsCount() == 3);

    WarpPoint wp1(1234, "TEST1", "C", 1594.1);
    WarpPoint wp2(12345, "TEST1", "A", 2143.9);
    WarpPoint wp3(4321, "TEST1", "C", 679.8);

    QVERIFY( sys.warpPoints().contains(wp1) );
    QVERIFY( sys.warpPoints().contains(wp2) );
    QVERIFY( sys.warpPoints().contains(wp3) );

    QVERIFY( sys.orbitsCount() == 8 );

    Planet p1("TEST1-1","TEST1", "1", "", "Hot Terrestrial", 0.4, 44733, "Chlorine", "");
    Planet p2("TEST1-2","TEST1", "2", "", "Hot Terrestrial", 0.75, 34089, "Chlorine", "");
    Planet p3("TEST1-3","TEST1", "3", "", "Asteroid Field", 1.1, 1, "Vacuum", "");
    Planet p4("TEST1-4","TEST1", "4", "", "Terrestrial", 1.8, 18468, "Carbon Dioxide", "");
    Planet p5("TEST1-4a","TEST1", "4", "a", "Moon", 0.0, 3233, "Nitrogen", "");
    Planet p6("TEST1-4b","TEST1", "4", "b", "Moon", 0.0, 2166, "Vacuum", "");
    Planet p7("TEST1-4c","TEST1", "4", "c", "Moon", 0.0, 3119, "Chlorine", "");
    Planet p8("TEST1-5","TEST1", "5", "", "Frozen Terrestrial", 3.2, 33430, "Vacuum", "");

    QVERIFY(sys.orbits().contains(p1));
    QVERIFY(sys.orbits().contains(p2));
    QVERIFY(sys.orbits().contains(p3));
    QVERIFY(sys.orbits().contains(p4));
    QVERIFY(sys.orbits().contains(p5));
    QVERIFY(sys.orbits().contains(p6));
    QVERIFY(sys.orbits().contains(p7));
    QVERIFY(sys.orbits().contains(p8));

    QFile f2(":/tests/data/SS2.txt");
    if (!f2.open(QIODevice::ReadOnly))
        return;

    QByteArray array1 = f2.readAll();
    f2.close();
    QString data1(array1);

    sys = parser->parseSS(data1);

    QVERIFY( sys.name() == "TEST2" );
    QVERIFY( sys.starType() == "M (Red) 1 V" );
    QVERIFY( sys.starSize() == "Single Star" );


    QVERIFY( sys.warpPointsCount() == 3);

    WarpPoint test2wp1(12345, "TEST2", "D", 1.9);
    WarpPoint test2wp2(234567, "TEST2", "E", 596.7);
    WarpPoint test2wp3(345678, "TEST2", "D", 0.2);

    QVERIFY( sys.warpPoints().contains(test2wp1) );
    QVERIFY( sys.warpPoints().contains(test2wp2) );
    QVERIFY( sys.warpPoints().contains(test2wp3) );

    QVERIFY( sys.orbitsCount() == 6 );

    Planet test2p1("TEST2-1","TEST2", "1", "", "Terrestrial", 0.6, 87840, "Ammonia", "");
    Planet test2p2("TEST2-2","TEST2", "2", "", "Frozen Terrestrial", 1, 140120, "Oxygen", "");
    Planet test2p3("TEST2-3","TEST2", "3", "", "Gas Giant", 1.4, 0.0, "---", "Ion Storm");
    Planet test2p4("TEST2-3a","TEST2", "3", "a", "Moon", 0.0, 9315, "Vacuum", "");
    Planet test2p5("TEST2-3b","TEST2", "3", "b", "Moon", 0.0, 4911, "Vacuum", "");
    Planet test2p6("TEST2-4","TEST2", "4", "", "Frozen Rockball", 2.2, 85440, "Chlorine", "Ion Storm");

    QVERIFY(sys.orbits().contains(test2p1));
    QVERIFY(sys.orbits().contains(test2p2));
    QVERIFY(sys.orbits().contains(test2p3));
    QVERIFY(sys.orbits().contains(test2p4));
    QVERIFY(sys.orbits().contains(test2p5));
    QVERIFY(sys.orbits().contains(test2p6));

}

void TurnParserTest::pmaps()
{
    DEBUG_BLOCK

    QFile f(":/tests/data/PMAP.txt");
    if (!f.open(QIODevice::ReadOnly))
        return;

    QByteArray array = f.readAll();
    f.close();
    QString data(array);

    Planet planet = parser->parsePMAP(data);

    QVERIFY(planet.name() == "TEST1-2");
    QVERIFY(planet.temp() == "311");
    QVERIFY(planet.axialTilt() == "30");
    QVERIFY(planet.gravity() == "1.1");
    QVERIFY(planet.atmosphere() == "Chlorine");
    QVERIFY(planet.ocean() == "Water");

    QVERIFY(planet.terrain().size() == 7);

    QVERIFY(planet.terrain().contains(TerrainPair("Desert", "23")));
    QVERIFY(planet.terrain().contains(TerrainPair("Forested Mountains", "19")));
    QVERIFY(planet.terrain().contains(TerrainPair("Rain Forest", "16")));
    QVERIFY(planet.terrain().contains(TerrainPair("Deciduous Forest", "10")));
    QVERIFY(planet.terrain().contains(TerrainPair("Marsh", "10")));
    QVERIFY(planet.terrain().contains(TerrainPair("Salt Flats", "9")));


    QFile f2(":/tests/data/PMAP2.txt");
    if (!f2.open(QIODevice::ReadOnly))
        return;

    QByteArray array2 = f2.readAll();
    f2.close();
    QString data2(array2);

    Planet planet2 = parser->parsePMAP(data2);
    QVERIFY(planet2.atmosphere() == "Carbon Dioxide");
    QVERIFY(planet2.ocean() == "Liquid Ammonia");
    QVERIFY(planet2.terrain().contains(TerrainPair("Liquid Ammonia", "65")));

    QFile f3(":/tests/data/PMAP3.txt");
    if (!f3.open(QIODevice::ReadOnly))
        return;

    QByteArray array3 = f3.readAll();
    f3.close();
    QString data3(array3);

    Planet planet3 = parser->parsePMAP(data3);
    QVERIFY(planet3.atmosphere() == "Vacuum");
    QVERIFY(planet3.ocean() == "-None-");
    QVERIFY(planet3.axialTilt() == "----");
    QVERIFY(planet3.gravity() == "0.1");
    QVERIFY(planet3.terrain().contains(TerrainPair("Asteroids", "100")));
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
