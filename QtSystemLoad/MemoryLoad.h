#ifndef MEMORYLOAD_H
#define MEMORYLOAD_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>

class MemoryLoad;

//!
//! \file MemoryLoad.h
//! \class MemoryInfo
//! \brief Класс описывающий информацию по использованию памяти
//!
class MemoryInfo
{
    friend class MemoryLoad;

public:
    //!
    //! \brief Конструктор класса
    //!
    MemoryInfo() {
        _memoryTotal = 0;
        _memoryFree = 0;
        _memoryBuffers = 0;
        _memoryCached = 0;
        _swapTotal = 0;
        _swapFree = 0;
    }

    //!
    //! \brief Деструктор класса
    //!
    virtual ~MemoryInfo() {}

    //!
    //! \brief Конструктор копирования
    //! \param info Класс-источник
    //!
    MemoryInfo(const MemoryInfo &info) {
        _memoryTotal = info.memoryTotal();
        _memoryFree = info.memoryFree();
        _memoryBuffers = info.memoryBuffers();
        _memoryCached = info.memoryCached();
        _swapTotal = info.swapTotal();
        _swapFree = info.swapFree();
    }

    //!
    //! \brief Оператор присваивания
    //! \param info Класс элемента
    //! \return
    //!
    MemoryInfo& operator=(const MemoryInfo &info) {
        //проверка на самоприсваивание
        if (this == &info)
            return *this;

        _memoryTotal = info.memoryTotal();
        _memoryFree = info.memoryFree();
        _memoryBuffers = info.memoryBuffers();
        _memoryCached = info.memoryCached();
        _swapTotal = info.swapTotal();
        _swapFree = info.swapFree();
        return *this;
    }

    //!
    //! \brief Общее количество памяти (Kb)
    //! \return Общее количество памяти (Kb)
    //!
    int memoryTotal() const { return _memoryTotal; }

    //!
    //! \brief Количество свободной памяти (Kb)
    //! \return Количество свободной памяти (Kb)
    //!
    int memoryFree() const { return _memoryFree; }

    //!
    //! \brief Размер буффера (Kb)
    //! \return Размер буффера (Kb)
    //!
    int memoryBuffers() const { return _memoryBuffers; }

    //!
    //! \brief Размер кэша (Kb)
    //! \return Размер кэша (Kb)
    //!
    int memoryCached() const { return _memoryCached; }

    //!
    //! \brief Количество используемой памяти (Kb)
    //! \return Количество используемой памяти (Kb)
    //!
    int memoryUsage() const { return (_memoryTotal - _memoryFree - _memoryBuffers - _memoryCached); }

    //!
    //! \brief Количество используемой памяти (%)
    //! \return Количество используемой памяти (%)
    //!
    float memoryUsagePercentage() const {
        if(_memoryTotal == 0)
            return 0.0;

        return ((float)(this->memoryUsage())/(float)_memoryTotal * 100.0);
    }

    //!
    //! \brief Общее количество памяти файла подкачки (Kb)
    //! \return Общее количество памяти файла подкачки (Kb)
    //!
    int swapTotal() const { return _swapTotal; }

    //!
    //! \brief Количество свободной памяти файла подкачки (Kb)
    //! \return Количество свободной памяти файла подкачки (Kb)
    //!
    int swapFree() const { return _swapFree; }

    //!
    //! \brief Количество используемой памяти файла подкачки (Kb)
    //! \return Количество используемой памяти файла подкачки (Kb)
    //!
    int swapUsage() const { return _swapTotal - _swapFree; }

    //!
    //! \brief Количество используемой памяти файла подкачки (%)
    //! \return Количество используемой памяти файла подкачки (%)
    //!
    float swapUsagePercentage() const {
        return ((float)(_swapTotal - _swapFree)/(float)_swapTotal * 100.0);
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
    int     _memoryTotal;   //!< Общее количество памяти (Kb)
    int     _memoryFree;    //!< Количество свободной памяти (Kb)
    int     _memoryBuffers; //!< Размер буффера (Kb)
    int     _memoryCached;  //!< Размер кэша (Kb)

    int     _swapTotal;     //!< Общее количество памяти файла подкачки (Kb)
    int     _swapFree;      //!< Количество свободной памяти файла подкачки (Kb)
};


//!
//! \file MemoryLoad.h
//! \class MemoryLoad
//! \brief Класс, формирующий информацию по использованию памяти
//!
class MemoryLoad : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Конструктор класса
    //! \param parent Класс-родитель
    //!
    explicit MemoryLoad(QObject *parent = 0);

    //!
    //! \brief Деструктор класса
    //!
    virtual ~MemoryLoad();

    //!
    //! \brief Информация по использованию памяти
    //! \return Информация по использованию памяти
    //!
    static MemoryInfo memoryValue(const QByteArray &proc_meminfo);
};


#endif // MEMORYLOAD_H
