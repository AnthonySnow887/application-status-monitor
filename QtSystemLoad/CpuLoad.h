#ifndef CPULOAD_H
#define CPULOAD_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QByteArray>

class CpuLoad;

// This structure stores a frame's cpu tics used in history
// calculations.  It exists primarily for SMP support but serves
// all environments.

//!
//! \file CpuLoad.h
//! \class CpuInfo
//! \brief Класс описывающий информацию по использованию CPU
//!
class CpuInfo
{
    friend class CpuLoad;

public:
    //!
    //! \brief Конструктор класса
    //!
    CpuInfo() {
        _id = 0;
        _cpuUsage = 0;

        _user = 0;
        _nice = 0;
        _system = 0;
        _idle = 0;
        _iowait = 0;
        _irq = 0;
        _softing = 0;
        _total = 0;

        _userPrev = 0;
        _nicePrev = 0;
        _systemPrev = 0;
        _idlePrev = 0;
        _iowaitPrev = 0;
        _irqPrev = 0;
        _softingPrev = 0;
        _totalPrev = 0;
    }

    //!
    //! \brief Деструктор класса
    //!
    virtual ~CpuInfo() {}

    //!
    //! \brief Конструктор копирования
    //! \param info Класс-источник
    //!
    CpuInfo(const CpuInfo &info) {
        _id = info.cpuId();
        _cpuUsage = info.cpuUsage();
        // current
        _user = info.cpuUser();
        _nice = info.cpuNice();
        _system = info.cpuSystem();
        _idle = info.cpuIdle();
        _iowait = info.cpuIowait();
        _irq = info.cpuIrq();
        _softing = info.cpuSofting();
        _total = info.cpuTotal();
        // prev
        _userPrev = info.cpuUserPrev();
        _nicePrev = info.cpuNicePrev();
        _systemPrev = info.cpuSystemPrev();
        _idlePrev = info.cpuIdlePrev();
        _iowaitPrev = info.cpuIowaitPrev();
        _irqPrev = info.cpuIrqPrev();
        _softingPrev = info.cpuSoftingPrev();
        _totalPrev = info.cpuTotalPrev();
    }

    //!
    //! \brief Оператор присваивания
    //! \param info Класс элемента
    //! \return
    //!
    CpuInfo& operator=(const CpuInfo &info) {
        //проверка на самоприсваивание
        if (this == &info)
            return *this;

        _id = info.cpuId();
        _cpuUsage = info.cpuUsage();
        // current
        _user = info.cpuUser();
        _nice = info.cpuNice();
        _system = info.cpuSystem();
        _idle = info.cpuIdle();
        _iowait = info.cpuIowait();
        _irq = info.cpuIrq();
        _softing = info.cpuSofting();
        _total = info.cpuTotal();
        // prev
        _userPrev = info.cpuUserPrev();
        _nicePrev = info.cpuNicePrev();
        _systemPrev = info.cpuSystemPrev();
        _idlePrev = info.cpuIdlePrev();
        _iowaitPrev = info.cpuIowaitPrev();
        _irqPrev = info.cpuIrqPrev();
        _softingPrev = info.cpuSoftingPrev();
        _totalPrev = info.cpuTotalPrev();
        return *this;
    }

    //!
    //! \brief Номер ядра CPU
    //! \return Номер ядра
    //!
    //! Если -1 - то это средние данные по всем ядрам
    //!
    int cpuId() const { return _id; }

    //!
    //! \brief Использование ядра (%)
    //! \return Использование ядра (%)
    //!
    float cpuUsage() const { return _cpuUsage; }


    // current values

    //!
    //! \brief Количество тиков нормальных процессов в пользовательском режиме
    //! \return Количество тиков
    //!
    qlonglong cpuUser() const { return _user; }

    //!
    //! \brief Количество тиков хороших процессов в пользовательском режиме
    //! \return Количество тиков
    //!
    qlonglong cpuNice() const { return _nice; }

    //!
    //! \brief Количество тиков процессов в режиме ядра
    //! \return Количество тиков
    //!
    qlonglong cpuSystem() const { return _system; }

    //!
    //! \brief Количество тиков простоя
    //! \return Количество тиков
    //!
    qlonglong cpuIdle() const { return _idle; }

    //!
    //! \brief Количество тиков одидания завершения ввода/вывода
    //! \return Количество тиков
    //!
    qlonglong cpuIowait() const { return _iowait; }

    //!
    //! \brief Количество тиков обслуживания прерываний
    //! \return Количество тиков
    //!
    qlonglong cpuIrq() const { return _irq; }

    //!
    //! \brief Количество тиков обслуживания приложений
    //! \return Количество тиков
    //!
    qlonglong cpuSofting() const { return _softing; }

    //!
    //! \brief Количество тиков суммарного использования
    //! \return Количество тиков
    //!
    qlonglong cpuTotal() const { return _total; }


    // prev values

    //!
    //! \brief Количество тиков нормальных процессов в пользовательском режиме (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuUserPrev() const { return _userPrev; }

    //!
    //! \brief Количество тиков хороших процессов в пользовательском режиме (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuNicePrev() const { return _nicePrev; }

    //!
    //! \brief Количество тиков процессов в режиме ядра (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuSystemPrev() const { return _systemPrev; }

    //!
    //! \brief Количество тиков простоя (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuIdlePrev() const { return _idlePrev; }

    //!
    //! \brief Количество тиков одидания завершения ввода/вывода (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuIowaitPrev() const { return _iowaitPrev; }

    //!
    //! \brief Количество тиков обслуживания прерываний (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuIrqPrev() const { return _irqPrev; }

    //!
    //! \brief Количество тиков обслуживания приложений (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuSoftingPrev() const { return _softingPrev; }

    //!
    //! \brief Количество тиков суммарного использования (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuTotalPrev() const { return _totalPrev; }

protected:
    int _id;                //!< Номер ядра CPU

    float _cpuUsage;        //!< Использование ядра (%)

    // current cpu values
    qlonglong _user;        //!< Количество тиков нормальных процессов в пользовательском режиме
    qlonglong _nice;        //!< Количество тиков хороших процессов в пользовательском режиме
    qlonglong _system;      //!< Количество тиков процессов в режиме ядра
    qlonglong _idle;        //!< Количество тиков простоя
    qlonglong _iowait;      //!< Количество тиков одидания завершения ввода/вывода
    qlonglong _irq;         //!< Количество тиков обслуживания прерываний
    qlonglong _softing;     //!< Количество тиков обслуживания приложений
    qlonglong _total;       //!< Количество тиков суммарного использования

    // prev cpu values
    qlonglong _userPrev;    //!< Количество тиков нормальных процессов в пользовательском режиме (предыдущее значение)
    qlonglong _nicePrev;    //!< Количество тиков хороших процессов в пользовательском режиме (предыдущее значение)
    qlonglong _systemPrev;  //!< Количество тиков процессов в режиме ядра (предыдущее значение)
    qlonglong _idlePrev;    //!< Количество тиков простоя (предыдущее значение)
    qlonglong _iowaitPrev;  //!< Количество тиков одидания завершения ввода/вывода (предыдущее значение)
    qlonglong _irqPrev;     //!< Количество тиков обслуживания прерываний (предыдущее значение)
    qlonglong _softingPrev; //!< Количество тиков обслуживания приложений (предыдущее значение)
    qlonglong _totalPrev;   //!< Количество тиков суммарного использования (предыдущее значение)
};


//!
//! \file CpuLoad.h
//! \class CpuLoad
//! \brief Класс, формирующий информацию по использованию CPU
//!
class CpuLoad : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Конструктор класса
    //! \param parent Класс-родитель
    //!
    explicit CpuLoad(QObject *parent = 0x00);

    //!
    //! \brief Деструктор класса
    //!
    virtual ~CpuLoad();

    //!
    //! \brief Запросить данные загрузки процессоров
    //! \param cpu Данные предыдущего запроса
    //! \return Список данных
    //!
    //! Данные предыдущего запроса нужны для корректного рассчета использования процессора.
    //!
    static QVector<CpuInfo> cpuValue(const QByteArray &proc_stat,
                                     QVector<CpuInfo> cpu = QVector<CpuInfo>());
};

#endif // CPULOAD_H
