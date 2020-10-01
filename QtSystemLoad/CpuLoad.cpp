#include "CpuLoad.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

CpuLoad::CpuLoad(QObject *parent)
    : QObject(parent)
{
}

CpuLoad::~CpuLoad()
{
}

QVector<CpuInfo> CpuLoad::cpuValue(const QByteArray &proc_stat, QVector<CpuInfo> cpu)
{
//    QFile file("/proc/stat");

    QVector<CpuInfo> buffCpu;
    QTextStream processOut(proc_stat);
    while(!processOut.atEnd()) {
        QString readLine = processOut.readLine();
        if(readLine.indexOf("cpu") == 0) {
            readLine = readLine.replace("  ", " ");
            QStringList lineList = readLine.split(" ");
            QString cpuIdStr = lineList[0].replace("cpu", "");

            if(lineList.size() < 9)
                continue;

            int cpuId = -1;
            if(!cpuIdStr.isEmpty())
                cpuId = cpuIdStr.toInt();

            CpuInfo val;
            if(!cpu.isEmpty()) {
                for(int i = 0; i < cpu.size(); i++) {
                    if(cpu[i].cpuId() == cpuId) {
                        val = cpu[i];
                        val._userPrev = val._user;
                        val._nicePrev = val._nice;
                        val._systemPrev = val._system;
                        val._idlePrev = val._idle;
                        val._iowaitPrev = val._iowait;
                        val._irqPrev = val._irq;
                        val._softingPrev = val._softing;
                        val._totalPrev = val._total;
                        break;
                    }
                }
            }

            val._id = cpuId;
            val._user = lineList[1].toLongLong();
            val._nice = lineList[2].toLongLong();
            val._system = lineList[3].toLongLong();
            val._idle = lineList[4].toLongLong();
            val._iowait = lineList[5].toLongLong();
            val._irq = lineList[6].toLongLong();
            val._softing = lineList[7].toLongLong();
            val._total = val._user + val._nice + val._system + val._idle + val._iowait + val._irq + val._softing;
            buffCpu.append(val);
        }
    }

    for(int i = 0; i < buffCpu.size(); i++) {
        // Подсчитаем дельту с момента последнего замера по CPU
        qlonglong diffIdle = buffCpu[i]._idle - buffCpu[i]._idlePrev;
        qlonglong diffTotal = buffCpu[i]._total - buffCpu[i]._totalPrev;

        // Вычисляем процент использования CPU
        float cpuUsage = (float)(1000 * (diffTotal - diffIdle)/diffTotal + 5)/(float)10;
        buffCpu[i]._cpuUsage = cpuUsage;
    }
    return buffCpu;
}
