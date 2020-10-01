#ifndef NETWORKLOAD_H
#define NETWORKLOAD_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QByteArray>

class NetworkLoad;

//!
//! \file NetworkLoad.h
//! \class NetworkInfo
//! \brief Класс описывающий информацию по использованию сетевых интерфейсов
//!
class NetworkInfo
{
    friend class NetworkLoad;

public:
    //!
    //! \brief Конструктор класса
    //!
    NetworkInfo() {
        _name = "";

        // current
        _inBytes = 0;
        _inPackets = 0;
        _outBytes = 0;
        _outPackets = 0;

        // prev
        _inBytesPrev = 0;
        _inPacketsPrev = 0;
        _outBytesPrev = 0;
        _outPacketsPrev = 0;
    }

    //!
    //! \brief Деструктор класса
    //!
    virtual ~NetworkInfo() {
        _name.clear();
    }

    //!
    //! \brief Конструктор копирования
    //! \param info Класс-источник
    //!
    NetworkInfo(const NetworkInfo &info) {
        _name = info.name();
        // current
        _inBytes = info.inBytes();
        _inPackets = info.inPackets();
        _outBytes = info.outBytes();
        _outPackets = info.outPackets();
        // prev
        _inBytesPrev = info.inBytesPrev();
        _inPacketsPrev = info.inPacketsPrev();
        _outBytesPrev = info.outBytesPrev();
        _outPacketsPrev = info.outPacketsPrev();
    }

    //!
    //! \brief Оператор присваивания
    //! \param info Класс элемента
    //! \return
    //!
    NetworkInfo& operator=(const NetworkInfo &info) {
        //проверка на самоприсваивание
        if (this == &info)
            return *this;

        _name = info.name();
        // current
        _inBytes = info.inBytes();
        _inPackets = info.inPackets();
        _outBytes = info.outBytes();
        _outPackets = info.outPackets();
        // prev
        _inBytesPrev = info.inBytesPrev();
        _inPacketsPrev = info.inPacketsPrev();
        _outBytesPrev = info.outBytesPrev();
        _outPacketsPrev = info.outPacketsPrev();
        return *this;
    }

    //!
    //! \brief Название сетевого интерфейса
    //! \return Название интерфейса
    //!
    QString name() const { return _name; }

    //!
    //! \brief Количество байт принятых с момента предыдущего запроса
    //! \return Количество байт
    //!
    quint64 inBytesNow() const {
        if(_inBytesPrev == 0)
            return 0;

        return (_inBytes - _inBytesPrev);
    }

    //!
    //! \brief Количество пакетов принятых с момента предыдущего запроса
    //! \return Количество пакетов
    //!
    quint64 inPacketsNow() const {
        if(_inPacketsPrev == 0)
            return 0;

        return (_inPackets - _inPacketsPrev);
    }

    //!
    //! \brief Количество байт отданных с момента предыдущего запроса
    //! \return Количество байт
    //!
    quint64 outBytesNow() const {
        if(_outBytesPrev == 0)
            return 0;

        return (_outBytes - _outBytesPrev);
    }

    //!
    //! \brief Количество пакетов отданных с момента предыдущего запроса
    //! \return Количество пакетов
    //!
    quint64 outPacketsNow() const {
        if(_outPacketsPrev == 0)
            return 0;

        return (_outPackets - _outPacketsPrev);
    }


    // current

    //!
    //! \brief Количество принятых байт
    //! \return Количество байт
    //!
    quint64 inBytes() const { return _inBytes; }

    //!
    //! \brief Количество принятых пакетов
    //! \return Количество пакетов
    //!
    quint64 inPackets() const { return _inPackets; }

    //!
    //! \brief Количество отданных байт
    //! \return Количество байт
    //!
    quint64 outBytes() const { return _outBytes; }

    //!
    //! \brief Количество отданных пакетов
    //! \return Количество пакетов
    //!
    quint64 outPackets() const { return _outPackets; }


    // prev

    //!
    //! \brief Количество принятых байт (предыдущее значение)
    //! \return Количество байт
    //!
    quint64 inBytesPrev() const { return _inBytesPrev; }

    //!
    //! \brief Количество принятых пакетов (предыдущее значение)
    //! \return Количество пакетов
    //!
    quint64 inPacketsPrev() const { return _inPacketsPrev; }

    //!
    //! \brief Количество отданных байт (предыдущее значение)
    //! \return Количество байт
    //!
    quint64 outBytesPrev() const { return _outBytesPrev; }

    //!
    //! \brief Количество отданных пакетов (предыдущее значение)
    //! \return Количество пакетов
    //!
    quint64 outPacketsPrev() const { return _outPacketsPrev; }


    // convert to...

    //!
    //! \brief Конвертирование из Bytes в Gb
    //! \param value Значение в Bytes
    //! \return Значение в Gb
    //!
    static float convertToGb(const float value) {
        return (((value / 1024.0) / 1024.0) / 1024.0);
    }

    //!
    //! \brief Конвертирование из Bytes в Mb
    //! \param value Значение в Bytes
    //! \return Значение в Mb
    //!
    static float convertToMb(const float value) {
        return ((value / 1024.0) / 1024.0);
    }

    //!
    //! \brief Конвертирование из Bytes в Kb
    //! \param value Значение в Bytes
    //! \return Значение в Kb
    //!
    static float convertToKb(const float value) {
        return (value / 1024.0);
    }

    //!
    //! \brief Конвертирование из Bytes в Bytes
    //! \param value Значение в Bytes
    //! \return Значение в Bytes
    //!
    static float convertToBytes(const float value) {
        return value;
    }

protected:
    QString _name;              //!< Название сетевого интерфейса

    // current
    quint64 _inBytes;           //!< Количество принятых байт
    quint64 _inPackets;         //!< Количество принятых пакетов

    quint64 _outBytes;          //!< Количество отданных байт
    quint64 _outPackets;        //!< Количество отданных пакетов

    // prev
    quint64 _inBytesPrev;       //!< Количество принятых байт (предыдущее значение)
    quint64 _inPacketsPrev;     //!< Количество принятых пакетов (предыдущее значение)

    quint64 _outBytesPrev;      //!< Количество отданных байт (предыдущее значение)
    quint64 _outPacketsPrev;    //!< Количество отданных пакетов (предыдущее значение)
};


//!
//! \file NetworkLoad.h
//! \class NetworkLoad
//! \brief Класс, формирующий информацию по использованию сетевых интерфейсов
//!
class NetworkLoad : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Конструктор класса
    //! \param parent Класс-родитель
    //!
    explicit NetworkLoad(QObject *parent = 0x00);

    //!
    //! \brief Деструктор класса
    //!
    virtual ~NetworkLoad();

    //!
    //! \brief Список сетевых интерфейсов со значениями
    //! \param network Список сетевых интерфейсов (предыдущее значение)
    //! \return Список сетевых интерфейсов
    //!
    //! Список сетевых интерфейсов на входе метода необходим для
    //! корректного рассчета загруженности.
    //!
    static QVector<NetworkInfo> networkValue(const QByteArray &proc_net_dev,
                                             QVector<NetworkInfo> network = QVector<NetworkInfo>());
};

#endif // NETWORKLOAD_H
