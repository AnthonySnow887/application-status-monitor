#ifndef DRIVELOAD_H
#define DRIVELOAD_H

#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QString>

class DriveLoad;

//!
//! \file DriveLoad.h
//! \class DriveInfo
//! \brief Класс описывающий информацию по использованию дисков
//!
class DriveInfo
{
    friend class DriveLoad;

public:
    //!
    //! \brief Конструктор класса
    //!
    DriveInfo() {
        _totalSize = 0;
        _availableSize = 0;
        _freeSize = 0;
        _usageSize = 0;
    }

    //!
    //! \brief Деструктор класса
    //!
    virtual ~DriveInfo() {
        _device.clear();
        _mountPoint.clear();
        _fileSystem.clear();
        _readType.clear();
        _dummyValue_0.clear();
        _dummyValue_1.clear();

        _totalSize = 0;
        _availableSize = 0;
        _freeSize = 0;
        _usageSize = 0;
    }

    //!
    //! \brief Конструктор копирования
    //! \param info Класс-источник
    //!
    DriveInfo(const DriveInfo &info) {
        _device = info.device();
        _mountPoint = info.mountPoint();
        _fileSystem = info.fileSystem();
        _readType = info.readType();
        _dummyValue_0 = info.dummyValue_0();
        _dummyValue_1 = info.dummyValue_1();

        _totalSize = info.totalSize();
        _availableSize = info.availableSize();
        _freeSize = info.freeSize();
        _usageSize = info.usageSize();
    }

    //!
    //! \brief Оператор присваивания
    //! \param info Класс элемента
    //! \return
    //!
    DriveInfo& operator=(const DriveInfo &info) {
        //проверка на самоприсваивание
        if (this == &info)
            return *this;

        _device = info.device();
        _mountPoint = info.mountPoint();
        _fileSystem = info.fileSystem();
        _readType = info.readType();
        _dummyValue_0 = info.dummyValue_0();
        _dummyValue_1 = info.dummyValue_1();

        _totalSize = info.totalSize();
        _availableSize = info.availableSize();
        _freeSize = info.freeSize();
        _usageSize = info.usageSize();
        return *this;
    }

    // props

    //!
    //! \brief Название устройства
    //! \return Название устройства
    //!
    QString device() const { return _device; }

    //!
    //! \brief Точка монтирования
    //! \return Точка монтирования
    //!
    QString mountPoint() const { return _mountPoint; }

    //!
    //! \brief Файловая система
    //! \return Файловая система
    //!
    QString fileSystem() const { return _fileSystem; }

    //!
    //! \brief Тип чтения
    //! \return Тип чтения
    //!
    QString readType() const { return _readType; }

    //!
    //! \brief Дополнительное значение 0
    //! \return Дополнительное значение 0
    //!
    QString dummyValue_0() const { return _dummyValue_0; }

    //!
    //! \brief Дополнительное значение 1
    //! \return Дополнительное значение 1
    //!
    QString dummyValue_1() const { return _dummyValue_1; }


    // size

    //!
    //! \brief Общий размер (Kb)
    //! \return Общий размер (Kb)
    //!
    qlonglong totalSize() const { return _totalSize; }

    //!
    //! \brief Доступный размер (Kb)
    //! \return Доступный размер (Kb)
    //!
    qlonglong availableSize() const { return _availableSize; }

    //!
    //! \brief Свободный размер (Kb)
    //! \return Свободный размер (Kb)
    //!
    qlonglong freeSize() const { return _freeSize; }

    //!
    //! \brief Используемый размер (Kb)
    //! \return Используемый размер (Kb)
    //!
    qlonglong usageSize() const { return _usageSize; }

    //!
    //! \brief Используемый размер (%)
    //! \return Используемый размер (%)
    //!
    float usageSizePercentage() const {
        if(_usageSize == 0 && _availableSize == 0)
            return 0.0;

        return (float)(100.0 * _usageSize / (_usageSize + _availableSize));
    }


    // convert to...

    //!
    //! \brief Конвертирование из Kb в Gb
    //! \param value Значение в Kb
    //! \return Значение в Gb
    //!
    static float convertToGb(const float value) {
        return ((value / 1024.0) / 1024.0);
    }

    //!
    //! \brief Конвертирование из Kb в Mb
    //! \param value Значение в Kb
    //! \return Значение в Mb
    //!
    static float convertToMb(const float value) {
        return (value / 1024.0);
    }

    //!
    //! \brief Конвертирование из Kb в Kb
    //! \param value Значение в Kb
    //! \return Значение в Kb
    //!
    static float convertToKb(const float value) {
        return value;
    }

protected:
    QString _device;            //!< Название устройства
    QString _mountPoint;        //!< Точка монтирования
    QString _fileSystem;        //!< Файловая система
    QString _readType;          //!< Тип чтения
    QString _dummyValue_0;      //!< Дополнительное значение 0
    QString _dummyValue_1;      //!< Дополнительное значение 1

    qlonglong _totalSize;       //!< Общий размер (Kb)
    qlonglong _availableSize;   //!< Доступный размер (Kb)
    qlonglong _freeSize;        //!< Свободный размер (Kb)
    qlonglong _usageSize;       //!< Используемый размер (Kb)
};


//!
//! \file DriveLoad.h
//! \class DriveLoad
//! \brief Класс, формирующий информацию по использованию дисков
//!
class DriveLoad : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Конструктор класса
    //! \param parent Класс-родитель
    //!
    explicit DriveLoad(QObject *parent = 0x00);

    //!
    //! \brief Деструктор класса
    //!
    virtual ~DriveLoad();

    //!
    //! \brief Список дисков
    //! \return Список дисков
    //!
    static QVector<DriveInfo> driveValue(const QByteArray &proc_filesystems,
                                         const QByteArray &proc_mounts);
};

#endif // DRIVELOAD_H
