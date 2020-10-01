#include "ProcessLoad.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>

ProcessLoad::ProcessLoad(QObject *parent)
    : QObject(parent)
{
}

ProcessLoad::~ProcessLoad()
{
}

ProcessInfo ProcessLoad::processValue(const QByteArray &proc_stat,
                                      const QByteArray &proc_pid_stat,
                                      const QByteArray &proc_pid_status,
                                      const ProcessInfo &process)
{
    qlonglong cpuTotal = 0;

    QTextStream processOut(proc_stat);
    while(!processOut.atEnd()) {
        QString readLine = processOut.readLine();
        if(readLine.indexOf("cpu") == 0) {
            readLine = readLine.replace("  ", " ");
            QStringList lineList = readLine.split(" ");
            QString cpuIdStr = lineList[0].replace("cpu", "");

            if(lineList.size() < 9)
                continue;
            if(!cpuIdStr.isEmpty())
                continue;

            qlonglong user = lineList[1].toLongLong();
            qlonglong nice = lineList[2].toLongLong();
            qlonglong system = lineList[3].toLongLong();
            qlonglong idle = lineList[4].toLongLong();
            qlonglong iowait = lineList[5].toLongLong();
            qlonglong irq = lineList[6].toLongLong();
            qlonglong softing = lineList[7].toLongLong();
            cpuTotal = user + nice + system + idle + iowait + irq + softing;
            break;
        }
    }

    ProcessInfo pInfo;
    QStringList bufList = QString(proc_pid_stat).split(" ");
    if (bufList.size() >= 17) {
        pInfo._pid = bufList[0].toInt();

        if (process.pid() == pInfo.pid()) {
            pInfo = process;
            pInfo._utimePrev = pInfo._utime;
            pInfo._stimePrev = pInfo._stime;
            pInfo._cpuTotalPrev = pInfo._cpuTotal;
        }

        pInfo._tcomm = bufList[1].replace("(","").replace(")","");
        pInfo._state = bufList[2];
        pInfo._ppid = bufList[3].toUInt();
        pInfo._pgrp = bufList[4].toLongLong();
        pInfo._sid = bufList[5].toUInt();
        pInfo._tty_nr = bufList[6].toLongLong();
        pInfo._tty_pgrp = bufList[7].toLongLong();
        pInfo._flags = bufList[8].toLongLong();
        pInfo._min_flt = bufList[9].toLongLong();
        pInfo._cmin_flt = bufList[10].toLongLong();
        pInfo._maj_flt = bufList[11].toLongLong();
        pInfo._cmaj_flt = bufList[12].toLongLong();
        pInfo._utime = bufList[13].toLongLong();
        pInfo._stime = bufList[14].toLongLong();
        pInfo._cutime = bufList[15].toLongLong();
        pInfo._csutime = bufList[16].toLongLong();
    }

    // get memory usage
    QTextStream processOut2(proc_pid_status);
    while(!processOut2.atEnd()) {
        QString readLine = processOut2.readLine();
        readLine = readLine.replace("  ", " ");
        QStringList lineList = readLine.split(" ");

        QVector<int> buffIndex;
        for(int i = 0; i < lineList.size(); i++) {
            if(lineList[i].isEmpty())
                buffIndex.push_back(i);
        }
        for(int i = buffIndex.size() - 1; i >= 0; i--)
            lineList.removeAt(buffIndex[i]);

        if(lineList.size() < 2)
            continue;
        if(lineList[0].indexOf("VmSize:") == 0)
            pInfo._virtualMemoryUsage = lineList[1].toULongLong();
        else if(lineList[0].indexOf("VmRSS:") == 0)
            pInfo._physicalMemoryUsage = lineList[1].toULongLong();
        else if(lineList[0].indexOf("VmLib:") == 0)
            pInfo._libraryMemoryUsage = lineList[1].toULongLong();
    }

    // cpu total
    pInfo._cpuTotal = cpuTotal;

    // cpu usage
    pInfo._userCpuUsage = 100.0 * (float)(pInfo._utime - pInfo._utimePrev) / (float)(pInfo._cpuTotal - pInfo._cpuTotalPrev);
    pInfo._sysCpuUsage = 100.0 * (float)(pInfo._stime - pInfo._stimePrev) / (float)(pInfo._cpuTotal - pInfo._cpuTotalPrev);
    return pInfo;
}

