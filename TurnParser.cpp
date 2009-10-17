#include "TurnParser.h"

#include "data/SNItem.h"
#include "data/ItemEffect.h"
#include <poppler-qt4.h>
#include <QRectF>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegExp>

TurnParser::TurnParser(): m_filename( "" ), m_text( "" ), m_parsed( false ), m_data()
{
}

TurnParser::TurnParser( const QString &filename ) : m_filename( filename ), m_text( "" ), m_parsed( false ), m_data()
{
    Poppler::Document *doc = Poppler::Document::load(filename);
    int count = doc->numPages();

    QTextStream out( &m_text, QIODevice::WriteOnly );
    for(int i = 0; i < count; i ++)
    {
        QString text = doc->page(i)->text(QRectF());

        // Cleanup text
        QStringList lines = text.split(QRegExp("\\n") );
        foreach(QString line, lines)
        {
            // Remove page dividers
            line = line.replace( QRegExp(".*Page\\s\\d+\\sof\\s\\d+"), "" );

            // Pad === dividers
            QRegExp rx("(={121})");
            rx.indexIn( line );
            if( rx.numCaptures() > 0 )
                line = line.replace(rx, "\n"+rx.cap()+"\n" );

            out << line << "\n";
//            qDebug() << line;
        }
    }
    parseData();
    m_parsed = true;
}

void TurnParser::parseData()
{
    QStringList lines = m_text.split(QRegExp("\\n") );
    int mode = 0;
    QString cmdstore = "";
    QString header;
    QString command;

    foreach(QString line, lines)
    {
//         qDebug() << "mode="<<mode<<" : " << line;
        if( line.trimmed() == "" )
            continue;
        QRegExp rx( "" );
        if (mode == 0 || mode == 4 || mode == 5 || mode == 6)
        {
            if (mode == 6)
            {
                command = "WARPPOINTS";
                cmdstore = "-----WARPPOINTS (Warp Points Surveyed)-----\nWARPPOINTS:\n";
                qDebug() << "command: " << command;
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
                qDebug() << "command: " << command;
            }
            rx.setPattern("\\s*Colony Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 4;
                command = "COLONIES";
                cmdstore = "-----COLONY (Imperial Colonies)-----\nCOLONY:\n";
                qDebug() << "command: " << command;
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
                qDebug() << "command: " << command;
            }
            rx.setPattern("\\s*Imperial Navy Report: Fleet Summary\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 5;
                command = "FLEETS";
                cmdstore = "-----FLEETS (Imperial Navy Report)-----\nFLEETS:\n";
                qDebug() << "command: " << command;
            }
            rx.setPattern("\\s*Fleet Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 5;
                command = "FLEETS2";
                cmdstore = "-----FLEETS2 (Fleet Report)-----\nFLEETS2:\n";
                qDebug() << "command: " << command;
            }
            rx.setPattern("\\s*Fleet Cargo Report\\s*");
            if ( rx.indexIn( line ) > -1)
            {
                mode = 3;
                cmdstore  = "=========================================================================================================================\n";
                qDebug() << "completed command: " << command;
            }
        }

        if (mode == 1)
        {
            rx.setPattern("^\\s*(\\w+)");
            if ( rx.indexIn(line) > -1 )
            {
                mode = 2;
                command = rx.cap(0);
                qDebug() << "command: " << command;
                cmdstore = header +"\n";
            } else {
                qDebug() << "wrong header";
                mode = 0;
            }
        }

        rx.setPattern( "^[^-]*-----([^-\\]]+)-----\\s*$");
        if ( (mode == 0) && rx.indexIn(line) > -1 )
        {
            header = rx.cap(0);
            mode = 1;
//            qDebug() <<  "new header found: xxx" << header << "xxx";
        }

        rx.setPattern("^[^-]*-----([^-\\]]+)-----\\s+RTD$");
        if ( mode == 0 && rx.indexIn(line) > -1 )
        {
            header = "";
            mode = 2;
            command = "RTD";
            qDebug() << "command RTD: " << command;
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
        qDebug() << "file open failed";
        return;
    }
    QTextStream out(&outfile);
    out << m_text;
    outfile.close();
}

SNItem TurnParser::parseANZs() const
{
    QStringList anzs = m_data.values("ANZ");
    return parseANZs(anzs);
}
SNItem TurnParser::parseANZs( const QStringList &anzs ) const
{
    qDebug() << "anzs: " << anzs.size();
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::RegExp);

    foreach( QString anz, anzs)
    {
        rx.setPattern("ANZ:\\s([^\\n]+)\\n");
        if( rx.indexIn( anz ) > -1 )
        {
            QString name = rx.cap(1);
//            qDebug() << anz;
//            qDebug() << "Got anz item: " << name;

            // Parse Description
            int desc_marker = rx.matchedLength();
//            qDebug() << anz.left(desc_marker);
            rx.setPattern("ANZ:\\s[^\\n]+\\n([^:]*):\\s+(.*)(?:(\\([\\d,]*\\s+ton(?:s)?\\)))");
            rx.setMinimal( true );
            rx.indexIn( anz, desc_marker );
            rx.setMinimal( false );
            QString desc = rx.cap( 2 );

            // Parse Weight
            rx.setPattern("\\((\\d*),?(\\d+),?(\\d*)\\s*\\n*\\s*tons?\\)");
            rx.indexIn( anz );
            QString tons = rx.cap(1) + rx.cap(2) + rx.cap(3);

            // Parse classification, range and structure
            rx.setPattern("(Classification|Range|Structural\\s+Integrity): (\\S+\\s?\\S+)");
            QString *classification = new QString();
            QString *range = new QString();
            QString *integrity = new QString();

            QString* current = 0;

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
//            qDebug() << "classification" << *classification;
//            qDebug() << "range" << *range;
//            qDebug() << "integrity" << *integrity;

            // Parse Components

//            QString anz_line = anz.replace("\n", " ");
//            rx.setPattern("[.\\n]*tons?\\)\\s(?:\\d+.*-\\s)+(?:\\d+.*)");
//            pos=0;
//            while ((pos = rx.indexIn(anz, pos)) != -1)
//            {
//            }
            rx.setPattern("[.\\n]*tons?\\)\\s((?:\\d+.*-\\s)+(?:\\d+.*))\n(.*:)");
            rx.setMinimal(true);
            rx.indexIn(anz);
            rx.setMinimal(false);

            QString entry = rx.cap(1);
            QString rest = rx.cap(2);
            entry = entry.replace("\n", "").trimmed();
            QStringList comps  = entry.split(" - ");
//            qDebug() << "composed of";
            foreach( QString comp, comps )
            {
                comp = comp.trimmed();
                int div = comp.indexOf(" ");
                QString amt = comp.left(div).trimmed();
                QString resource = comp.right(comp.length()-div).trimmed();
                resource.replace(QRegExp(" {2,}"), " ");
//                qDebug() << amt << resource;
            }

            // Parse Effects

            rx.setPattern("Prerequisite Technologies:[^\\n]+\\n");
            rx.indexIn(anz);
            int prereqs_end = anz.indexOf(rx) + rx.matchedLength();

            rx.setPattern("([^\\:]+|Counters): (?:(\\S+) \[\\D*(\\d+)\\]|([^\\n]+))");
            QList<ItemEffect*> effects;
            pos = prereqs_end;
            ItemEffect* effect = 0;
            bool pending = false;
            while ((pos = rx.indexIn(anz, pos)) != -1)
            {
                if( rx.cap(1) != "Counters" )
                {
                    if( pending && effect != 0)
                    {
                        // There is no counter for the effect
                        effects << effect;
                    }
                    QString name = rx.cap(1).trimmed();
                    QString val = rx.cap(3).trimmed();
                    QString pval;

                    // The regex puts the pretty value in spot 2 IF there is a numeric value
                    //    e.g. Point Defense Accuracy: Fair [50]
                    // If there is no numeric value the pvalue is captured into spot 4
                    //    e.g. Warp Survey Gear: Fair
                    if( val != "" )
                        pval = rx.cap(2).trimmed();
                    else
                        pval = rx.cap(4).trimmed();

                    effect = new ItemEffect( name, val.toInt(), pval );
                    pending = true;
                } else {
                    if( pending  && effect != 0)
                    {
                        //Found the current effect's counter
                        effect->setCounter( rx.cap(4) );
                        effects << effect;
                    } else {
                        qDebug() << "Got Counter without Effect";
                    }
                }
                pos += rx.matchedLength();
            }

            rx.setPattern( "([^\\:]+):\\s(\\S+)(?:\\s\\[(?:[^\\:]+:\\s)?(\\d+)\\])?\\s+(?:Counters:\\s(.*)\\n)" );
            rx.setMinimal(true);
            rx.indexIn(anz, prereqs_end);
            rx.setMinimal(false);
            qDebug() << rx.capturedTexts();

//            qDebug() << "Effect: " << rx.cap(1) << rx.cap(2) << rx.cap(3);
//            qDebug() << "Counters: " << rx.cap(4);


            // Parse Counters
        }
    }
    return SNItem();
}
