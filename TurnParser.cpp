#include "TurnParser.h"

#include "SNConstants.h"
#include "data/SNItem.h"
#include "data/ItemEffect.h"
#include "data/System.h"
#include "Debug.h"

#include <poppler-qt4.h>
#include <QRectF>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QSharedPointer>

TurnParser::TurnParser( QObject * parent ): QThread( parent ), m_filename( "" ), m_text( "" ), m_parsed( false ), m_data()
{
}

TurnParser::TurnParser( const QString &filename, QObject * parent ) : QThread( parent ), m_filename( filename ), m_text( "" ), m_parsed( false ), m_data()
{

}
void TurnParser::run()
{
    go();
    exec();
    deleteLater();
}

void TurnParser::go()
{
    DEBUG_BLOCK
    debug() << "Turn Parser Loading:" << m_filename;
    Poppler::Document *doc = Poppler::Document::load( m_filename );
    int count = doc->numPages();

    QFile outfile(m_filename+".txt");
    if ( !outfile.open( QIODevice::WriteOnly ) )
    {
        debug() << "file open failed";
        return;
    }
    QTextStream outdebug(&outfile);

    QTextStream out( &m_text, QIODevice::WriteOnly );
    for(int i = 0; i < count; i ++)
    {
        QString text = doc->page(i)->text(QRectF());
        outdebug << text;
        // Cleanup text
        QStringList lines = text.split(QRegExp("\\n") );

        foreach(QString line, lines)
        {
            // Remove page dividers
            line = line.replace( QRegExp(".*Page\\s\\d+\\sof\\s\\d+"), "" );

            // Pad === dividers
            QRegExp rx("(={120,122})");
            rx.indexIn( line );
            if( rx.numCaptures() > 0 )
                line = line.replace(rx, "\n"+rx.cap()+"\n" );

            out << line << "\n";
//            debug() << line;
        }
    }
    outfile.close();
    out.flush();
    delete doc;

    //Some more cleanup

//    QRegExp rx;
//    rx.setPattern("-----([^-]+)-----\\s+(\\w+:)(.+)\\s\\s");
//    rx.setMinimal(true);
//    m_text.replace(rx, "\n-----" + rx.cap(1) + "-----\n" + rx.cap(2) + rx.cap(3) + "\n" );

//    rx.setPattern("------(-+)");
//    rx.setMinimal(false);
//    m_text.replace(rx, "\n------" + rx.cap(1) + "\n");

//    rx.setPattern("^([^:]+:)");
//    m_text.replace(rx, "\n" + rx.cap(1));

    parseData();
    m_parsed = true;
}

void TurnParser::parseData()
{
    DEBUG_BLOCK
    QStringList lines = m_text.split(QRegExp("\\n") );
    int mode = 0;
    QString cmdstore = "";
    QString header;
    QString command;

    foreach(QString line, lines)
    {
//         debug() << "mode="<<mode<<" : " << line;
        if( line.trimmed() == "" )
            continue;
        QRegExp rx( "" );
        if (mode == 0 || mode == 4 || mode == 5 || mode == 6)
        {
            if (mode == 6)
            {
                command = "WARPPOINTS";
                cmdstore = "-----WARPPOINTS (Warp Points Surveyed)-----\nWARPPOINTS:\n";
                debug() << "command: " << command;
                mode = 5;
            }

            rx.setPattern("\\s*Shipyard Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                cmdstore = "=========================================================================================================================\n";
                mode = 3;
            }
            rx.setPattern("\\s*Warp Points Surveyed\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                cmdstore = "=========================================================================================================================\n";
                mode = 6;
            }
            rx.setPattern("\\s*Character Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                cmdstore  = "=========================================================================================================================\n";
                mode = 3;
            }
            rx.setPattern("\\s*Imperial Stockpiles\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 4;
                command = "STOCK";
                cmdstore = "-----STOCK (Imperial Stockpile)-----\nSTOCK:\n";
                debug() << "command: " << command;
            }
            rx.setPattern("\\s*Colony Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 4;
                command = "COLONIES";
                cmdstore = "-----COLONY (Imperial Colonies)-----\nCOLONY:\n";
                debug() << "command: " << command;
            }
            rx.setPattern("\\s*Industrial Production\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                cmdstore  = "=========================================================================================================================\n";
                mode = 6;
            }
            rx.setPattern("\\s*Imperial Astronomical Report: Unsurveyed Warp Points\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 4;
                command = "UNSURVEYED";
                cmdstore = "-----UNSURVEYED (Unsurveyed Warp Points)-----\nUNSURVEYED:\n";
                debug() << "command: " << command;
            }
            rx.setPattern("\\s*Imperial Navy Report: Fleet Summary\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 5;
                command = "FLEETS";
                cmdstore = "-----FLEETS (Imperial Navy Report)-----\nFLEETS:\n";
                debug() << "command: " << command;
            }
            rx.setPattern("\\s*Fleet Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 5;
                command = "FLEETS2";
                cmdstore = "-----FLEETS2 (Fleet Report)-----\nFLEETS2:\n";
                debug() << "command: " << command;
            }
            rx.setPattern("\\s*Fleet Cargo Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 3;
                cmdstore  = "=========================================================================================================================\n";
                debug() << "completed command: " << command;
            }
        }

        if (mode == 1)
        {
            rx.setPattern("^\\s*(\\w+)");
            if ( rx.indexIn(line) > -1 )
            {
                mode = 2;
                command = rx.cap(0);
                debug() << "command: " << command;
                cmdstore = header +"\n";
            } else {
                debug() << "wrong header";
                mode = 0;
            }
        }

        rx.setPattern( "^[^-]*-----([^-\\]]+)-----\\s*$");
        if ( (mode == 0) && rx.indexIn(line) > -1 )
        {
            header = rx.cap(0);
            mode = 1;
//            debug() <<  "new header found: xxx" << header << "xxx";
        }

        rx.setPattern("^[^-]*-----([^-\\]]+)-----\\s+RTD$");
        if ( mode == 0 && rx.indexIn(line) > -1 )
        {
            header = "";
            mode = 2;
            command = "RTD";
            debug() << "command RTD: " << command;
            cmdstore = "-----RTD (Request Technology Descriptions)-----\nRTD:\n";
            continue;
        }

        rx.setPattern("={45}[\\s=]+\\n");
        line.replace(rx, "=========================================================================================================================\n");
        if( mode != 3 )
            cmdstore += line + "\n";

        rx.setPattern("={120}");
        if( rx.indexIn(line) > -1 || mode == 3 || mode == 6 )
        {
            if( mode != 6 )
                mode = 0;
            if( command != "" )
                m_data.insert(command.trimmed(), cmdstore);
        }

    }
    parseANZs();
    parseSS();
}

QString TurnParser::text() const
{
    return m_text;
}

void TurnParser::writeOut( const QString &filename )
{
    QFile outfile(filename+".txt");
    if ( !outfile.open( QIODevice::WriteOnly ) )
    {
        debug() << "file open failed";
        return;
    }
    QTextStream out(&outfile);
    out << m_text;
    outfile.close();
}

void TurnParser::debugData( const QString &directory )
{

    QHashIterator<QString, QString> i(m_data);

    while (i.hasNext()) {
        i.next();
        QFile outfile(directory + "/" +i.key()+".txt");
        if ( !outfile.open( QIODevice::Append ) )
        {
            debug() << "file open failed";
            return;
        }
        QTextStream out(&outfile);
        out << i.value();
        outfile.close();
    }
}

const System TurnParser::parseSS( const QString &data )
{
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::RegExp);
    rx.setPattern("-----(\\w+)\\s\\[([^\\]]+)\\]\\s\\[([^\\]]+)\\]-----");
    QString sysname;
    QString type;
    QString size;
    if( rx.indexIn( data ) > -1 )
    {
        sysname = rx.cap(1).trimmed();
        type = rx.cap(2).trimmed();
        size = rx.cap(3).trimmed();
    } else {
        warning() << "Found SS, but no details found.";
        return System();
    }

    debug() << "System Scan: " << sysname << type << size;

//    sysname = TurnParser::toProper(sysname);

    System system( sysname, type, size );


    //Parse orbits
    rx.setPattern("(\\d+)(\\w)\\s+(\\w+)\\s+(\\d+)\\s+(([^\\d]+|---)+)|(\\d+)\\s+(([^\\d\\s]+\\s)+)\\s*(\\d+\\.\\d+|\\d+)\\s+(\\d+|---)\\s+(([^\\d]+|---)+)");
    int pos = 0;
    while ((pos = rx.indexIn(data, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString orbit;
        QString moon;
        QString type;
        QString orbdist;
        QString diam;
        QString atmo;
        if( !rx.cap(7).isEmpty() ) // parsed a planet
        {
            orbit = rx.cap(7);
            type = rx.cap(8);
            orbdist = rx.cap(10);
            diam = rx.cap(11);
            atmo = rx.cap(13);
        } else { // parsed a moon
            orbit = rx.cap(1);
            moon = rx.cap(2);
            type  = rx.cap(3);
            diam = rx.cap(4);
            atmo = rx.cap(6);
        }
        QRegExp rx2;
        rx2.setPattern("(.+)\\s+$");
        if(type.contains(rx2))
        {
            rx2.indexIn(type);
            type = rx2.cap(1).trimmed();
        }

        // The last system will have the Wap Point ID line
        // it needs to be filtered out
        rx2.setPattern("(.+)\\sWarp Point ID\\s.*");
        rx2.setMinimal(true);
        if(atmo.contains(rx2))
        {
            rx2.indexIn(atmo.trimmed());
            atmo = rx2.cap(1).trimmed();
        } else
            atmo = atmo.trimmed();

        QString special = atmo;
        rx2.setPattern(".+\\s+(.*)");
        rx2.setMinimal(false);
        if( special.contains(rx2))
        {
            rx2.indexIn(special);
            special = rx2.cap(1).trimmed();
        }
        else
            special = "";

        rx2.setPattern("(.+)\\s+$");
        rx2.setMinimal(true);
        if(special.contains(rx2))
        {
            rx2.indexIn(special);
            special = rx2.cap(1);
        }

        rx2.setPattern("(.+)\\s+.*");
        if(atmo.contains(rx2))
        {
            rx2.indexIn(atmo);
            atmo = rx2.cap(1).trimmed();
        }

        rx2.setPattern("(.+)\\s+$");
        if(atmo.contains(rx2))
        {
            rx2.indexIn(atmo);
            atmo = rx2.cap(1);
        }

        if( special.contains(QRegExp("^\\w+$")) )
        {
            atmo += " " + special;
            special = "";
        }

        debug() << sysname << ":" << orbit+""+moon << type << orbdist << diam << atmo << special;
        QString name = sysname + "-" +orbit+moon;

        bool ok = false;
        float orb_distance = orbdist.toFloat(&ok);
        if(type != "Moon" && !ok)
            warning() << "Error converting orbital distance for planet" << name;

        int diameter = diam.toInt(&ok);
        if(!ok)
            warning() << "Error converting diameter for planet" << name;

        Planet planet(name, sysname, orbit, moon, type, orb_distance, diameter, atmo, special);
        system.addOrbit( planet );
    }

    //Parse Warppoints
    rx.setPattern("(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\w)");
    pos = 0;
    while ((pos = rx.indexIn(data, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString warpid = rx.cap(1);
        QString orbdist = rx.cap(2);
        QString warpclass = rx.cap(3);

        bool ok = false;
        float orbital_distance = orbdist.toFloat(&ok);
        if(!ok)
            warning() << "Error converting orbital distance for warppoint " << warpid << "in system" << sysname;

        int idnum = warpid.toInt(&ok);
        if(!ok)
            warning() << "Error converting id for warppoint " << warpid << "in system" << sysname;


        WarpPoint wp( idnum, sysname, warpclass, orbital_distance );
        debug() << sysname << ":" << warpid << warpclass << orbital_distance;
        system.addWP( wp );
    }
    return system;
}

const QList<System> TurnParser::parseSS()
{
//    DEBUG_BLOCK
    QStringList sss = m_data.values("SS");

    QList<System> systems;
    foreach( QString ss, sss)
    {
        systems << parseSS(ss);
    }

    emit( ssParsingComplete( systems ) );
    return systems;
}

const QList<Planet> TurnParser::parsePMAP()
{
    QStringList pmaps = m_data.values("PMAP");

    QList<Planet> planets;
    foreach( QString pmap, pmaps)
    {
        planets << parsePMAP(pmap);
    }

    emit( pmapParsingComplete( planets ) );
    return planets;
}

const Planet TurnParser::parsePMAP( const QString &data )
{
    DEBUG_BLOCK
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::RegExp);
    rx.setPattern("PMAP:\\s+\\d+,\\s+(\\w)");
    QString suborbit;

    if( rx.indexIn( data ) > -1 )
    {
        suborbit = rx.cap(1).trimmed();
        QChar firstchar = suborbit.at(0);
        if( firstchar < 'a' || firstchar > 'z')
            suborbit = "";
    }

    bool found_pmap = false;
    rx.setPattern("(detailed) Planet Map of (\\w+)-(\\d+)(\\w?)");
    if( rx.indexIn( data ) > -1 )
        found_pmap = true;
    else
    {
        rx.setPattern("(located) at (\\w+)-(\\d+)\\w?, conducts a detailed Planet Map of moon (\\w)");
        if( rx.indexIn( data ) > -1 )
            found_pmap = true;
        else
        {
        rx.setPattern("located (at) (\\w+)-(\\d+)\\w?, conducts a detailed Planet Map of the main planet");
        if( rx.indexIn( data ) > -1 )
            found_pmap = true;
        }
    }

    if(!found_pmap)
    {
        warning() << "Found PMAP, but no details found.";
        return Planet();
    }

    QString sysname = rx.cap(2).toUpper();
    QString orbit = rx.cap(3);
    QString suborbitwrong = rx.cap(4);
    if( rx.cap(1) == "located")
    {
        suborbit = suborbitwrong;
    }
    else if( rx.cap(1) == "at")
    {
        suborbit = "";
    }

    QString pname = (sysname+"-"+orbit).toUpper() + suborbit;
    pname = pname.replace(QRegExp("\\s-"), "-");

    debug() << "PMAP for " << pname;

    rx.setPattern("Pop Group \\#\\s*\\n?\\s*(\\d+):\\s+[^p]+\\spopulation units, governed by([^\\(]*)");
    int pos = 0;
    while ((pos = rx.indexIn(data, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString popgroup = rx.cap(1);
        QString empireid = rx.cap(2);
        debug() << "Pop group" << popgroup << "found for" << pname << "belonging to empire #" << empireid;
    }

    rx.setPattern("Pop Group \\#\\s*(\\d+):\\s+[^c]+\\scivilians, governed by([^\\(]*)");
    pos = 0;
    while ((pos = rx.indexIn(data, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString popgroup = rx.cap(1);
        QString empireid = rx.cap(2);
        debug() << "Pop group civilians" << popgroup << "found for" << pname << "belonging to empire #" << empireid;
    }

    rx.setPattern("Ocean\\s+-{121}\\s+(\\d+)\\s+(\\d+|-+)\\s+(\\d+\\.+\\d+)\\s+((?:[^\\d\\s]+\\s)+)\\s+([^\\d]+)+\\n");
    rx.indexIn(data);
    QString temp = rx.cap(1);
    QString tilt = rx.cap(2);
    QString grav = rx.cap(3);
    QString atmo = rx.cap(4).trimmed();
    QString ocean = rx.cap(5).trimmed();

    Planet p(pname, temp,tilt, grav, ocean, atmo, "", "", "");

    debug() << pname << temp << tilt << grav << atmo << ocean;

    // Parse the terrain
    rx.setPattern("Ocean\\s+-{121}\\s+.*\\n");
    rx.setMinimal(true);
    pos = rx.indexIn(data);
    pos += rx.matchedLength();
    rx.setPattern("((?:[^\\d\\s]+\\s)+)(\\d+)%");
    rx.setMinimal(false);
    while ((pos = rx.indexIn(data, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString terrain = rx.cap(1).replace(",", "").trimmed();
        QString percent = rx.cap(2);
        debug() << pname << terrain << percent << "%";
        p.addTerrain(terrain, percent);
    }

    return p;
}

const QList<SNItem> TurnParser::parseANZs()
{
    DEBUG_BLOCK
    debug() << "Parsing ANZs";
    QStringList anzs = m_data.values("ANZ");
    debug() << "found" << anzs.size() << "anzs";
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::RegExp);

    QList<SNItem> items;
    foreach( QString anz, anzs)
    {
        rx.setPattern("ANZ:\\s([^\\n]+)\\n");
        if( rx.indexIn( anz ) > -1 )
        {
            QString name = rx.cap(1).trimmed();
            debug() << "Got anz item: " << name;
            debug() << anz;

            // Parse Description
//            int desc_marker = rx.matchedLength();
//            debug() << anz.left(desc_marker);
            rx.setPattern("ANZ:\\s[^\\n]+\\n[^:]*:\\s+(.*)(?:\\(\\d+ ton|Classification)");
            rx.setMinimal( true );
            rx.indexIn( anz  );
            QString desc = rx.cap( 1 ).trimmed();
            rx.setMinimal( false );

            // Is technology?

            bool istech = false;
            if( anz.contains("This Item cannot be manufactured,") )
                istech = true;

            // Parse Weight
            rx.setPattern("\\((\\d*),?(\\d+),?(\\d*)\\s*\\n*\\s*tons?\\)");
            rx.indexIn( anz );
            QString tons = (rx.cap(1) + rx.cap(2) + rx.cap(3)).trimmed();

            // Parse classification, range and structure
            // note: this regexp assumes the values of classification/range won't
            // be longer more than three words
            rx.setPattern("(Classification|Range|Structural\\s+Integrity): (\\S+\\s?\\S+\\s?\\S+|\\d+)");

            QSharedPointer<QString> current = QSharedPointer<QString>(new QString);
            QSharedPointer<QString> classification = QSharedPointer<QString>(new QString);
            QSharedPointer<QString> integrity = QSharedPointer<QString>(new QString);
            QSharedPointer<QString> range = QSharedPointer<QString>(new QString);

            int pos=0;
            while ((pos = rx.indexIn(anz, pos)) != -1)
            {
                if ("Classification" == rx.cap(1) )
                    current = classification;
                else if ("Range" == rx.cap(1) )
                    current = range;
                else if ("Structural Integrity" == rx.cap(1) )
                    current = integrity;
                if (rx.cap(2) != "")
                    if (current != 0)
                        current->append( rx.cap(2) );
                pos += rx.matchedLength();
            }
            // the previous regex isn't 100% solid, sometimes it grabs the \n plus the ===== divider
            // so we need to parse that out.
            *integrity = integrity->left(integrity->indexOf("\n")).trimmed();
            *classification = classification->left(classification->indexOf("\n")).trimmed();
            *range = range->left(range->indexOf("\n")).trimmed();

            // Parse Components

            rx.setPattern("[.\\n]*ton(?:s)?\\)\\s+(.*)Classification:");
            rx.setMinimal(true);
            rx.indexIn(anz);
            rx.setMinimal(false);

            QString entry = rx.cap(1);
            entry = entry.simplified();
            QStringList comps  = entry.split(" - ");
            debug() << "Begin item components";
            QMap<QString, int> components;
            foreach( QString comp, comps )
            {
                comp = comp.trimmed();
                if( comp.isEmpty() )
                    continue;
                int div = comp.indexOf(" ");
                QString amt = comp.left(div).trimmed().replace(",","");
                QString resource = comp.right(comp.length()-div).trimmed();
                resource.replace(QRegExp(" {2,}"), " ");
                debug() << amt << resource;
                bool ok;
                components.insert( resource, amt.toInt(&ok) );
                if( !ok && !istech )
                    warning() << "resource amount toInt error while parsing item: " << name << " resource: " << resource << " amt:" << amt;
            }
            debug() << "End item components." << components.size() << " found.";

            // Parse Effects & counters

            rx.setPattern("Prerequisite Technologies:[^\\n]+\\n");
            rx.indexIn(anz);
            int prereqs_end = anz.indexOf(rx) + rx.matchedLength();

            rx.setPattern("([^\\:]+|Counters): (?:(\\S+) \\[\\D*(\\d+)\\]|([^\\n]+))");
            QList<ItemEffect*> effects;
            pos = prereqs_end;
            ItemEffect* effect = 0;
            bool pending = false;
            while ((pos = rx.indexIn(anz, pos)) != -1)
            {
                if( rx.cap(1).trimmed() != "Counters" )
                {

                    if( pending && effect != 0)
                    {
                        // There is no counter for the effect
                        effects << effect;
                        pending = false;
                    }
                    QString name = rx.cap(1).trimmed();
                    QString val = rx.cap(3).trimmed();
                    QString pval;

                    // The regex puts the pretty value in spot 2 IF there is a numeric value
                    //    e.g. Point Defense Accuracy: Fair [50]
                    // If there is no numeric value the pvalue is captured into spot 4
                    //    e.g. Warp Survey Gear: Fair
                    int val_numeric = 0;
                    if( !val.isEmpty() )
                    {
                        pval = rx.cap(2).trimmed();
                        bool ok;
                        val_numeric = val.toInt(&ok);
                        if( !ok )
                            warning() << "converting effect value to int error while parsing item " << name;
                    }
                    else
                        pval = rx.cap(4).trimmed();

                    effect = new ItemEffect( name, val_numeric, pval );
                    pending = true;

                } else {
                    if( pending  && effect != 0)
                    {
                        //Found the current effect's counter
                        effect->setCounter( rx.cap(4) );
                        effects << effect;
                        pending = false;
                    } else
                        warning() << "got Counter without effect while parsing item " << name;
                }
                pos += rx.matchedLength();
            }
            if( pending && effect != 0 )
                effects << effect;

            // convert list of ptrs
            QList<ItemEffect> effects_normal;
            foreach(ItemEffect* eff, effects)
            {
                effects_normal << ItemEffect( *eff );
            }
            qDeleteAll(effects);

//            debug() << "Effect: " << rx.cap(1) << rx.cap(2) << rx.cap(3);
//            debug() << "Counters: " << rx.cap(4);

            bool ok;
            int tons_numeric = tons.toInt(&ok);
            if( !ok && !istech )
                warning() << "got int conversion error for tonnage while parsing item " << name;


            int struct_numeric = integrity->toInt(&ok);
            if( !ok && !istech && classification->trimmed() != "Mass Destruction Device")
                warning() << "got int conversion error for integrity while parsing item " << name << " " << *integrity;

            // Get the actual category/subcategory
            CategoryPair pair = mapClassificationToCategory( name, classification->trimmed(), istech );

            // Fix numeric values for items that don't need them
            if( istech )
            {
                tons_numeric = 0;
                struct_numeric = 0;
            }
            SNItem item(name, QString(desc.simplified()), pair.first, pair.second, tons_numeric, struct_numeric);
            debug() << item << desc;
            item.addComponents( components );
            item.addEffects( effects_normal );

            items << item;
        }
    }
//    m_anzs = items;
    emit anzParsingComplete( items );
    return items;
}

CategoryPair TurnParser::mapClassificationToCategory(  const QString &name, const QString &classification, bool istech ) const
{
    // Is this really the best way to compare strings?

        // Technologies
    if( istech )
    {
        if( classification == "Horizon Technology")
            return CategoryPair(SN::Category::Technology, SN::Category::HorizonTechnology);
        else if( classification == "Ground Combat")
            return CategoryPair(SN::Category::Technology, SN::Category::GroundTechnology);
        else if( classification == "Cloaking Device")
            return CategoryPair(SN::Category::Technology, "");
        else
            return CategoryPair(SN::Category::Technology, "");
    }

    // Ship Components
    if( classification == "Bridge" )
        return CategoryPair(SN::Category::ShipComponent, SN::Category::Bridge);
    else if( classification == "Armor")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::Armor);
    else if( classification == "Diplomatic Facility")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::DiplomaticFacility);
    else if( classification == "Science Lab")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::ScienceLab);
    else if( classification == "Shield")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::Shield);
    else if( classification == "Defensive System")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::DefensiveSystem);
    else if( classification == "Drone Rack")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::Drone);
    else if( classification == "Fighter Bay")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::FighterBay);
    else if( classification == "Sensor")
        return CategoryPair(SN::Category::ShipComponent, SN::Category::Sensor);
    else if( classification == "Jump Drive" ) {
        if( name.contains("Engine") )
            return CategoryPair(SN::Category::ShipComponent, SN::Category::Engine);
        else
            return CategoryPair(SN::Category::ShipComponent, SN::Category::JumpDrive);
    }

    // Resources
    if( classification == SN::Category::Resource)
    {
        if( name.contains( QRegExp( "Improved.*") ) )
            return CategoryPair(SN::Category::Resource, SN::Category::ImprovedResource);
        else if( name.contains( QRegExp( "Advanced.*") ) )
            return CategoryPair(SN::Category::Resource, SN::Category::AdvancedResource);
        else
            return CategoryPair(SN::Category::Resource, SN::Category::InitialResource);

    }
    //Weapons
    if( classification == "Mass Destruction Device" )
        return CategoryPair(SN::Category::MDD, "" );
    else if( classification == "Weapon" )
        return CategoryPair( SN::Category::ShipComponent,  SN::Category::Weapon );


    debug() << "WARNING: Found uncatagorizable item; '"  << name << "' '"  << classification <<"'";
    return CategoryPair( SN::Category::Unknown , "" );
}

QString TurnParser::toProper( const QString &string )
{
    QString str = string.toLower();
    int idx = str.length() - 1;
    while( true )
    {
        QChar c = str.at(idx);
        if(idx == 0)
        {
            if( c.isLower() )
                str.replace(0, 1, c.toUpper());

            break;
        } else {
            if( c.isSpace() )
            {
                int dst = idx + 1;
                if( str.at(dst).isLower() )
                    str.replace(dst, 1, str.at(dst).toUpper() );
            }
            idx--;
        }
    }
    return str;
}
