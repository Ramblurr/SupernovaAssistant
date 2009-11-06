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
    ShipDesign( const QString &name, const QString &type, const QString &missionclass );

    ShipDesign& operator=( const ShipDesign &other );

    QString name() const { return m_name; }
    QString type() const { return m_type; }
    QString missionClass() const { return m_class; }
    QMap<QString, qint64> getComponents() const { return m_components; }

    void setName( const QString &name) { m_name = name; }
    void setType( const QString &type) { m_type = type; }
    void setMissionClass( const QString &missionClass) { m_class = missionClass; }
    void addComponent(const QString &item, qint64 quantity);

    bool saveDesign();
    bool deleteDesign();

    static QList<ShipDesign> getDesigns();
    static bool designExists( const QString &name );

private:
    QString m_name;
    QString m_type;
    QString m_class;

    QMap<QString, qint64> m_components;
};

Q_DECLARE_METATYPE( ShipDesign )

#endif // SHIPDESIGN_H
