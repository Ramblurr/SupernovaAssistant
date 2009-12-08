#include "Order.h"
#include "Debug.h"

int Order::MAX_ARGS = 52;

Order::Order()
{
}

Order::Order( const QString &command ) : m_command( command )
{
}

QString Order::print() const
{
    QStringList finalArgs( m_args );

    //Pad empty fields
    int count = 1 + m_args.size();
    int diff = MAX_ARGS - count;
    if( diff > 0 )
    {
        for( int i=0; i < diff; i++ )
            finalArgs << " ";
    }

    QString args = "\"" + finalArgs.join("\",\"");
    args = m_command + "\"," + args;

    return args;
}
