#include "SNItem.h"
#include <QMap>
SNItem::SNItem()
{
}
SNItem::SNItem( const SNItem &other )
{
        m_name = other.name();
        m_category = other.category();
        m_desc = other.desc();
        m_tons = other.weight();
        m_components = other.getComponents();
}

SNItem::SNItem( const QString &name, const QString &desc, const QString &category ) : m_name( name ), m_desc( desc ), m_category( category ), m_components()
{
}

SNItem::SNItem( const QString &name, const QString &desc, const QString &category, int weight ): m_name( name ), m_desc( desc ), m_tons( weight ), m_category( category ), m_components()
{
}

void SNItem::addComponent( const QString &item, int quantity )
{
    if ( quantity <= 0 )
        return;
    m_components.insert( item, quantity );
}

QMap<QString, int> SNItem::getComponents() const
{
    return m_components;
}

bool SNItem::operator==(const SNItem &other) const
{
    return ( m_name == other.name() );
}

SNItem& SNItem::operator=( const SNItem &other )
{
    if ( this != &other )
    {
        m_name = other.name();
        m_category = other.category();
        m_desc = other.desc();
        m_tons = other.weight();
        m_components = other.getComponents();
    }
    return *this;
}
