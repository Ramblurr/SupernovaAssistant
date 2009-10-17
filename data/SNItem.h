#ifndef SNITEM_H
#define SNITEM_H

#include "ItemEffect.h"

#include <QString>
#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QVariant>
#include <QDataStream>
class SNItem
{
public:
    SNItem();
    SNItem( const SNItem &item );
    SNItem( const QString &name, const QString &desc, const QString &category  );
    SNItem( const QString &name, const QString &desc, const QString &category, int weight );
    SNItem( const QString &name, const QString &desc, const QString &category, const QString & subcat, int weight, int structure = 0 );

    SNItem& operator=(const SNItem &other);

    void addComponent(const QString &item, int quantity);
    void addComponents( QMap<QString, int> comps );
    void addEffect( const ItemEffect &effect );
    void addEffects( QList<ItemEffect> );

    QMap<QString, int> getComponents() const;
    QList<ItemEffect> getEffects() const;
    QString name() const { return m_name; }
    QString desc() const { return m_desc; }
    QString category() const { return m_category; }
    QString subcategory() const { return m_subcategory; }
    int weight() const { return m_tons; }
    int structure() const { return m_structure; }

    bool saveItem() const;
    bool updateItem( const SNItem &item ) const;
    bool deleteItem() const;

    bool operator==(const SNItem &other) const;

    static QList<SNItem> getItemsFromDatabase();
    static QList<SNItem> getItemsFromXml( const QString &filename);
    static SNItem getItem( const QString &name );

    //Warning clears existing items table
    static void writeToDatabase( const QList<SNItem> &list);

    static bool createXML( const QList<SNItem> & items, const QString &filename );

    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

    friend QDataStream &operator<<(QDataStream &out, const SNItem &item)
    {
        out << item.name() << item.desc() << item.category() << item.subcategory();
        out << item.weight() << item.structure() << item.getEffects() << item.getComponents();
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, SNItem &item)
    {
        in >> item.m_name >> item.m_desc >> item.m_category >> item.m_structure;
        in >> item.m_tons >> item.m_structure >> item.m_effects >> item.m_components;
        return in;
    }

private:
    QString m_name;
    QString m_desc;
    QString m_category;
    QString m_subcategory;
    QMap<QString, int> m_components;
    int m_tons;
    int m_structure;
    QList<ItemEffect> m_effects;

};
Q_DECLARE_METATYPE( SNItem )

inline bool operator<(const SNItem &i1, const SNItem &i2)
 {
     if ( i1.name() != i2.name() )
         return i1.name() < i2.name();
     return false;
 }
#endif // SNITEM_H
