#ifndef ORDER_H
#define ORDER_H

#include <QStringList>
class Order
{
public:
    Order();
    Order( const QString &command );

    QString print() const;

    void addArg( const QString &arg ) { m_args << arg; }

    friend Order &operator<<(Order &out,  const QString &arg)
    {
        out.addArg( arg );
        return out;
    }

    friend Order &operator<<(Order &out,  int arg )
    {
        out.addArg( QString::number( arg ) );
        return out;
    }

    friend Order &operator<<(Order &out,  quint64 arg )
    {
        out.addArg( QString::number( arg ) );
        return out;
    }

private:

    QString m_command;

    QStringList m_args;

    static int MAX_ARGS;

//    static QString m_delim;
};



#endif // ORDER_H
