#ifndef PROCESSLOAD_H
#define PROCESSLOAD_H

#include <QtCore/QObject>
#include <QtCore/QVector>

//
// /proc/<PID>/stat
//
// Table 1-3: Contents of the stat files (as of 2.6.22-rc3)
// ..............................................................................
//  Field          Content
//   pid           process id
//   tcomm         filename of the executable
//   state         state (R is running, S is sleeping, D is sleeping in an
//                 uninterruptible wait, Z is zombie, T is traced or stopped)
//   ppid          process id of the parent process
//   pgrp          process group ID of the process.
//   sid           session id
//   tty_nr        tty the process uses
//   tty_pgrp      pgrp of the tty
//   flags         task flags
//   min_flt       number of minor faults
//   cmin_flt      number of minor faults with child's
//   maj_flt       number of major faults
//   cmaj_flt      number of major faults with child's
//   utime         user mode jiffies
//   stime         kernel mode jiffies
//   cutime        user mode jiffies with child's
//   cstime        kernel mode jiffies with child's
//
// You're probably after utime and/or stime
//
// Read both utime and stime for the process you're interested in, and read time_total from /proc/stat.
// Then sleep for a second or so, and read them all again. You can now calculate the CPU usage of the process over the sampling time, with:
//
// user_util = 100 * (utime_after - utime_before) / (time_total_after - time_total_before);
// sys_util = 100 * (stime_after - stime_before) / (time_total_after - time_total_before);
//
// Process name:
// /proc/<PID>/cmdline - содержит полное имя процесса, но может быть пустым!
//


class ProcessLoad;

//!
//! \file ProcessLoad.h
//! \class ProcessInfo
//! \brief Класс описывающий информацию по запущенному в системе процессу
//!
class ProcessInfo
{
    friend class ProcessLoad;

public:
    //!
    //! \brief Конструктор класса
    //!
    ProcessInfo() {
        _pid = -1;
        _tcomm = "";
        _tcommPath = "";
        _state = "";
        _ppid = 0;
        _pgrp = 0;
        _sid = 0;
        _tty_nr = 0;
        _tty_pgrp = 0;
        _flags = 0;
        _min_flt = 0;
        _cmin_flt = 0;
        _maj_flt = 0;
        _cmaj_flt = 0;
        _utime = 0;
        _stime = 0;
        _cutime = 0;
        _csutime = 0;

        _virtualMemoryUsage = 0;
        _physicalMemoryUsage = 0;
        _libraryMemoryUsage = 0;

        _userCpuUsage = 0;
        _sysCpuUsage = 0;

        _utimePrev = 0;
        _stimePrev = 0;
        _cpuTotal = 0;
        _cpuTotalPrev = 0;
    }

    //!
    //! \brief Деструктор класса
    //!
    virtual ~ProcessInfo() {
        _tcomm.clear();
        _tcommPath.clear();
        _state.clear();
    }

    //!
    //! \brief Конструктор копирования
    //! \param info Класс-источник
    //!
    ProcessInfo(const ProcessInfo &info) {
        _pid = info.pid();
        _tcomm = info.tcomm();
        _tcommPath = info.tcommPath();
        _state = info.state();
        _ppid = info.ppid();
        _pgrp = info.pgrp();
        _sid = info.sid();
        _tty_nr = info.tty_nr();
        _tty_pgrp = info.tty_pgrp();
        _flags = info.flags();
        _min_flt = info.min_flt();
        _cmin_flt = info.cmin_flt();
        _maj_flt = info.maj_flt();
        _cmaj_flt = info.cmaj_flt();
        _utime = info.utime();
        _stime = info.stime();
        _cutime = info.cutime();
        _csutime = info.csutime();

        _virtualMemoryUsage = info.virtualMemoryUsage();
        _physicalMemoryUsage = info.physicalMemoryUsage();
        _libraryMemoryUsage = info.libraryMemoryUsage();

        _userCpuUsage = info.userCpuUsage();
        _sysCpuUsage = info.sysCpuUsage();

        _utimePrev = info.utimePrev();
        _stimePrev = info.stimePrev();
        _cpuTotal = info.cpuTotal();
        _cpuTotalPrev = info.cpuTotalPrev();
    }

    //!
    //! \brief Оператор присваивания
    //! \param info Класс элемента
    //! \return
    //!
    ProcessInfo& operator=(const ProcessInfo &info) {
        //проверка на самоприсваивание
        if (this == &info)
            return *this;

        _pid = info.pid();
        _tcomm = info.tcomm();
        _tcommPath = info.tcommPath();
        _state = info.state();
        _ppid = info.ppid();
        _pgrp = info.pgrp();
        _sid = info.sid();
        _tty_nr = info.tty_nr();
        _tty_pgrp = info.tty_pgrp();
        _flags = info.flags();
        _min_flt = info.min_flt();
        _cmin_flt = info.cmin_flt();
        _maj_flt = info.maj_flt();
        _cmaj_flt = info.cmaj_flt();
        _utime = info.utime();
        _stime = info.stime();
        _cutime = info.cutime();
        _csutime = info.csutime();

        _virtualMemoryUsage = info.virtualMemoryUsage();
        _physicalMemoryUsage = info.physicalMemoryUsage();
        _libraryMemoryUsage = info.libraryMemoryUsage();

        _userCpuUsage = info.userCpuUsage();
        _sysCpuUsage = info.sysCpuUsage();

        _utimePrev = info.utimePrev();
        _stimePrev = info.stimePrev();
        _cpuTotal = info.cpuTotal();
        _cpuTotalPrev = info.cpuTotalPrev();
        return *this;
    }

    // current values
    //!
    //! \brief PID процесса
    //! \return PID
    //!
    uint    pid() const { return _pid; }

    //!
    //! \brief Имя процесса
    //! \return Имя
    //!
    QString tcomm() const { return _tcomm; }

    //!
    //! \brief Полный путь до исполняемого файла
    //! \return Полный путь
    //!
    //! Может быть пустым, если процесс системный и запущен ядром.
    //!
    QString tcommPath() const { return _tcommPath; }

    //!
    //! \brief Состояние процесса
    //! \return Состояние
    //!
    //! Возможные значения:
    //! R - работает
    //! S - спит
    //! D - спит в режиме ожидания,
    //! Z - "зомби" процесс,
    //! T - прослеживается или остановлен
    //!
    QString state() const { return _state; }

    //!
    //! \brief PID родительского процесса
    //! \return PID
    //!
    uint    ppid() const { return _ppid; }

    //!
    //! \brief ID группы процесса
    //! \return ID группы
    //!
    qint64  pgrp() const { return _pgrp; }

    //!
    //! \brief ID сессии
    //! \return ID сессии
    //!
    uint    sid() const { return _sid; }

    //!
    //! \brief Терминал использующий процесс
    //! \return Терминал
    //!
    qint64  tty_nr() const { return _tty_nr; }

    //!
    //! \brief ID группы терминалов
    //! \return ID группы
    //!
    qint64  tty_pgrp() const { return _tty_pgrp; }

    //!
    //! \brief Флаги
    //! \return Флаги
    //!
    qint64  flags() const { return _flags; }

    //!
    //! \brief Количество незначительных сбоев
    //! \return Количество незначительных сбоев
    //!
    qint64  min_flt() const { return _min_flt; }

    //!
    //! \brief Количество незначительных сбоев с дочерними процессами
    //! \return Количество незначительных сбоев с дочерними процессами
    //!
    qint64  cmin_flt() const { return _cmin_flt; }

    //!
    //! \brief Количество значительных сбоев
    //! \return Количество значительных сбоев
    //!
    qint64  maj_flt() const { return _maj_flt; }

    //!
    //! \brief Количество значительных сбоев с дочерними процессами
    //! \return Количество значительных сбоев с дочерними процессами
    //!
    qint64  cmaj_flt() const { return _cmaj_flt; }

    //!
    //! \brief Количество тиков в пользовательском режиме
    //! \return Количество тиков
    //!
    qint64  utime() const { return _utime; }

    //!
    //! \brief Количество тиков в режиме ядра
    //! \return Количество тиков
    //!
    qint64  stime() const { return _stime; }

    //!
    //! \brief Количество тиков в пользовательском режиме с дочерними процессами
    //! \return Количество тиков
    //!
    qint64  cutime() const { return _cutime; }

    //!
    //! \brief Количество тиков в режиме ядра с дочерними процессами
    //! \return Количество тиков
    //!
    qint64  csutime() const { return _csutime; }

    //!
    //! \brief Общее использование CPU
    //! \return Количество тиков
    //!
    qlonglong cpuTotal() const { return _cpuTotal; }

    //!
    //! \brief Использование виртуальной памяти в kB
    //! \return Использование виртуальной памяти
    //!
    quint64 virtualMemoryUsage() const { return _virtualMemoryUsage; }

    //!
    //! \brief Использование физической памяти в kB (RAM)
    //! \return Использование физической памяти
    //!
    quint64 physicalMemoryUsage() const { return _physicalMemoryUsage; }

    //!
    //! \brief Использование памяти зависимыми библиотеками kB (RAM)
    //! \return Использование памяти зависимыми библиотеками
    //!
    quint64 libraryMemoryUsage() const { return _libraryMemoryUsage; }

    //!
    //! \brief Использование процессора в пользовательском режиме (%)
    //! \return Использование процессора (%)
    //!
    float userCpuUsage() const { return _userCpuUsage; }

    //!
    //! \brief Использование процессора в режиме ядра (%)
    //! \return Использование процессора (%)
    //!
    float sysCpuUsage() const { return _sysCpuUsage; }


    // prev values

    //!
    //! \brief Количество тиков в пользовательском режиме (предыдущее значение)
    //! \return Количество тиков
    //!
    qint64  utimePrev() const { return _utimePrev; }

    //!
    //! \brief Количество тиков в режиме ядра (предыдущее значение)
    //! \return Количество тиков
    //!
    qint64  stimePrev() const { return _stimePrev; }

    //!
    //! \brief Общее использование CPU (предыдущее значение)
    //! \return Количество тиков
    //!
    qlonglong cpuTotalPrev() const { return _cpuTotalPrev; }


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
    uint    _pid;                   //!< PID процесса
    QString _tcomm;                 //!< Имя процесса
    QString _tcommPath;             //!< Полный путь до исполняемого файла
    QString _state;                 //!< Состояние процесса
    uint    _ppid;                  //!< PID родительского процесса
    qint64  _pgrp;                  //!< ID группы процесса
    uint    _sid;                   //!< ID сессии
    qint64  _tty_nr;                //!< Терминал использующий процесс
    qint64  _tty_pgrp;              //!< ID группы терминалов
    qint64  _flags;                 //!< Флаги
    qint64  _min_flt;               //!< Количество незначительных сбоев
    qint64  _cmin_flt;              //!< Количество незначительных сбоев с дочерними процессами
    qint64  _maj_flt;               //!< Количество значительных сбоев
    qint64  _cmaj_flt;              //!< Количество значительных сбоев с дочерними процессами
    qint64  _utime;                 //!< Количество тиков в пользовательском режиме
    qint64  _stime;                 //!< Количество тиков в режиме ядра
    qint64  _cutime;                //!< Количество тиков в пользовательском режиме с дочерними процессами
    qint64  _csutime;               //!< Количество тиков в режиме ядра с дочерними процессами

    qlonglong _cpuTotal;            //!< Общее использование CPU (тики)

    quint64 _virtualMemoryUsage;    //!< Использование виртуальной памяти в kB
    quint64 _physicalMemoryUsage;   //!< Использование физической памяти в kB (RAM)
    quint64 _libraryMemoryUsage;    //!< Использование памяти зависимыми библиотеками kB (RAM)

    float  _userCpuUsage;           //!< Использование процессора в пользовательском режиме (%)
    float  _sysCpuUsage;            //!< Использование процессора в режиме ядра (%)

    qint64  _utimePrev;             //!< Количество тиков в пользовательском режиме (предыдущее значение)
    qint64  _stimePrev;             //!< Количество тиков в режиме ядра (предыдущее значение)

    qlonglong _cpuTotalPrev;        //!< Общее использование CPU (предыдущее значение) (тики)
};


//!
//! \file ProcessLoad.h
//! \class ProcessLoad
//! \brief Класс, формирующий информацию о запущенных процессах
//!
class ProcessLoad : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Конструктор класса
    //! \param parent Класс-родитель
    //!
    explicit ProcessLoad(QObject *parent = 0x00);

    //!
    //! \brief Деструктор класса
    //!
    virtual ~ProcessLoad();

    //!
    //! \brief Список запущенных процессов
    //! \param process Список запущенных процессов (предыдущее значение)
    //! \return Список запущенных процессов
    //!
    //! Список запущенных процессов на входе метода необходим для
    //! корректного рассчета использования CPU.
    //!
    static ProcessInfo processValue(const QByteArray &proc_stat,
                                    const QByteArray &proc_pid_stat,
                                    const QByteArray &proc_pid_status,
                                    const ProcessInfo &process = ProcessInfo());
};

#endif // PROCESSLOAD_H
