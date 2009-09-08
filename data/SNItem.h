#ifndef SNITEM_H
#define SNITEM_H

#include <QString>
#include <QtCore/QMap>
#include <QtCore/QMetaType>

class SNItem
{
public:
    SNItem();
    SNItem( const SNItem &item );
    SNItem( const QString &name, const QString &desc, const QString &category  );
    SNItem( const QString &name, const QString &desc, const QString &category, int weight );

    SNItem& operator=(const SNItem &other);

    void addComponent(const QString &item, int quantity);

    QMap<QString, int> getComponents() const;
    QString name() const { return m_name; }
    QString desc() const { return m_desc; }
    QString category() const { return m_category; }
    int weight() const { return m_tons; }

    bool operator==(const SNItem &other) const;

private:
    QString m_name;
    QString m_desc;
    QString m_category;
    int m_tons;
    QMap<QString, int> m_components;
};
Q_DECLARE_METATYPE( SNItem )

inline bool operator<(const SNItem &i1, const SNItem &i2)
 {
     if ( i1.name() != i2.name() )
         return i1.name() < i2.name();
     return false;
 }
#endif // SNITEM_H
