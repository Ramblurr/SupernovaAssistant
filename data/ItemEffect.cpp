#include "ItemEffect.h"

ItemEffect::ItemEffect()
{
}

ItemEffect::ItemEffect( const QString &name, int value, const QString &prettyval )
        : m_name( name )
        , m_value( value )
        , m_prettyValue( prettyval )
        , m_counter( "" )
{
}

ItemEffect::ItemEffect( const QString &name, int value, const QString &prettyval, const QString &counter)
        : m_name( name )
        , m_value( value )
        , m_prettyValue( prettyval )
        , m_counter( counter )

{}

ItemEffect::ItemEffect(const ItemEffect &other)
{
    m_name = other.name();
    m_value = other.value();
    m_prettyValue = other.prettyValue();
    m_counter = other.counter();
}

ItemEffect& ItemEffect::operator=(const ItemEffect &other)
{
    if ( this != &other )
    {
        m_name = other.name();
        m_value = other.value();
        m_prettyValue = other.prettyValue();
        m_counter = other.counter();
    }
    return *this;
}

bool ItemEffect::operator==(const ItemEffect &other) const
{
    return m_name == other.name() && m_value == other.value() && m_prettyValue == other.prettyValue();
}
