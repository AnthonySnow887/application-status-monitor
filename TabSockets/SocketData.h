#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include <QString>

class SocketData
{
public:
    enum Type {
        Type_Invalid = 0,
        Type_Tcp,
        Type_Udp
    };

    SocketData()
        : _type(Type_Invalid)
    {}
    SocketData(const Type &type,
               const QString &name)
        : _type(type)
        , _name(name.trimmed())
    {}
    virtual ~SocketData() {}

    SocketData(const SocketData &obj) {
        _type = obj.type();
        _name = obj.name();
    }


    SocketData& operator=(const SocketData &obj) {
        if (this == &obj)
            return *this;

        _type = obj.type();
        _name = obj.name();
        return *this;
    }

    Type type() const { return _type; }
    QString name() const { return _name; }

    static Type strToType(const QString &str) {
        if (str.trimmed().toLower() == "tcp")
            return Type_Tcp;
        if (str.trimmed().toLower() == "udp")
            return Type_Udp;

        return Type_Invalid;
    }

    static QString typeToStr(const Type &type) {
        switch (type) {
            case Type_Tcp:
                return QString("TCP");
            case Type_Udp:
                return QString("UDP");
            default:
                break;
        }
        return QString("???");
    }

private:
    Type _type;
    QString _name;
};

#endif // SOCKETDATA_H
