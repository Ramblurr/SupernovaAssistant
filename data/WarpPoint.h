#ifndef WARPPOINT_H
#define WARPPOINT_H

#include <QString>

class WarpPoint
{
public:
    WarpPoint();
    WarpPoint( int idnum, const QString &sysname, const QString &classtype, const float orbdistance );

    int warpId() const { return m_id; }
    QString systemName() const { return m_sysname; }
    QString warpClass() const { return m_class; }
    QString destinationSystem() const { return m_destinationsys; }
    float orbDistance() const { return m_orbdistance; }

    void setDestinationSystem( const QString &destsys );

    bool operator==( const WarpPoint &other ) const;

    /**
      @brief Gets warp points from the database
      @param systemname Optionally specify the name of the system to retrieve wps from
      @return A list of all wps, or the ones in the requested system
      */
    static QList<WarpPoint> getWPs( const QString & systemname = "" );

private:
    int m_id;
    QString m_sysname;
    QString m_class;
    QString m_destinationsys;
    float m_orbdistance;
};

#endif // WARPPOINT_H
