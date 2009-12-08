#ifndef EMPIRE_H
#define EMPIRE_H

#include "Debug.h"

#include <QString>
#include <QtCore/QMetaType>
#include <QDataStream>
#include <QVariant>

class Empire
{
public:
    Empire();
    Empire( const Empire &empire );
    Empire( const QString &name, const QString &id, const QString &symbol = "" );

    QString name() const { return m_name; }
    QString id() const { return m_id; }
    QString symbol() const { return m_symbol; }


    friend QDataStream &operator<<(QDataStream &out, const Empire &myObj)
    {
        out << myObj.m_name << myObj.m_id; //<< myObj.m_symbol;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, Empire &myObj)
    {
        in >> myObj.m_name >> myObj.m_id;// >> myObj.m_symbol;
//        debug() << "YO!"<< myObj.m_name << " " << myObj.m_id;
        return in;
    }

    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

    private:
    QString m_name;
    QString m_id;
    QString m_symbol;

};

Q_DECLARE_METATYPE( Empire )

#endif // EMPIRE_H

