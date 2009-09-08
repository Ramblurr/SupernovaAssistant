#include "Empire.h"

Empire::Empire()
{
}

Empire::Empire( const Empire &empire )
{
    m_name = empire.name();
    m_id = empire.id();
    m_symbol = empire.symbol();
}

Empire::Empire( const QString &name, const QString &id, const QString &symbol )
        : m_name( name )
        , m_id( id )
        , m_symbol( symbol )
{
}
