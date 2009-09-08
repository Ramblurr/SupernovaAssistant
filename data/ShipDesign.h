#ifndef SHIPDESIGN_H
#define SHIPDESIGN_H

#include <QString>
#include <QtCore/QMap>
#include <QtCore/QMetaType>

class QSqlDatabase;
class ShipDesign
{
public:
    ShipDesign();
    ShipDesign( const ShipDesign &other );
    ShipDesign( const QString &name, const QString &type, const QString &missionclass, int id = -1 );

    ShipDesign& operator=( const ShipDesign &other );

    QString name() const { return m_name; }
    QString type() const { return m_type; }
    QString missionClass() const { return m_class; }
    int id() const { return m_id; }
    QMap<QString, quint64> getComponents() const { return m_components; }

    void setName( const QString &name) { m_name = name; }
    void setType( const QString &type) { m_type = type; }
    void setMissionClass( const QString &missionClass) { m_class = missionClass; }
    void addComponent(const QString &item, quint64 quantity);

    bool saveDesign( const QString &empireId );
    bool deleteDesign( const QString &empireId );

    static QList<ShipDesign> getDesigns(  const QString &empireId  );
    static QSqlDatabase getDb(   const QString &empireId );

private:
    QString m_name;
    QString m_type;
    QString m_class;
    int m_id;

    QMap<QString, quint64> m_components;
};

Q_DECLARE_METATYPE( ShipDesign )

#endif // SHIPDESIGN_H
