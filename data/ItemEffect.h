#ifndef ITEMEFFECT_H
#define ITEMEFFECT_H

#include <QString>
#include <QDataStream>
class ItemEffect
{
public:
    ItemEffect();
    ItemEffect( const QString &name, int value, const QString &prettyval, const QString &counter);
    ItemEffect(const ItemEffect &other);
    ItemEffect& operator=(const ItemEffect &other);
    bool operator==(const ItemEffect &other) const;

    QString name() const { return m_name; }
    int value() const { return m_value; }
    QString prettyValue() const { return m_prettyValue; }
    QString counter() const { return m_counter; }

    friend QDataStream &operator<<(QDataStream &out, const ItemEffect &e)
    {
        out << e.m_name << e.m_value << e.m_prettyValue << e.m_counter;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, ItemEffect &e)
    {
        in >> e.m_name >> e.m_value >> e.m_prettyValue >> e.m_counter;
        return in;
    }

private:
    QString m_name;
    int m_value;
    QString m_prettyValue;
    QString m_counter;
};

#endif // ITEMEFFECT_H
