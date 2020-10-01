#include "MemoryLoad.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

MemoryLoad::MemoryLoad(QObject *parent)
    : QObject(parent)
{
}

MemoryLoad::~MemoryLoad()
{
}

MemoryInfo MemoryLoad::memoryValue(const QByteArray &proc_meminfo)
{
//    QFile file("/proc/meminfo");

    MemoryInfo buffMem;
    QTextStream processOut(proc_meminfo);
    while(!processOut.atEnd()) {
        QString readLine = processOut.readLine();
        readLine = readLine.replace("  ", " ");
        QStringList lineList = readLine.split(" ");

        QVector<int> buffIndex;
        for(int i = 0; i < lineList.size(); i++) {
            if(lineList[i].isEmpty())
                buffIndex.push_back(i);
        }
        for(int i = buffIndex.size() - 1; i >= 0; i--)
            lineList.removeAt(buffIndex[i]);

        if(lineList.size() < 3)
            continue;
        if(lineList[0].indexOf("MemTotal") == 0)
            buffMem._memoryTotal = lineList[1].toInt();
        else if(lineList[0].indexOf("MemFree") == 0)
            buffMem._memoryFree = lineList[1].toInt();
        else if(lineList[0].indexOf("Buffers") == 0)
            buffMem._memoryBuffers = lineList[1].toInt();
        else if(lineList[0].indexOf("Cached") == 0)
            buffMem._memoryCached = lineList[1].toInt();
        else if(lineList[0].indexOf("SwapTotal") == 0)
            buffMem._swapTotal = lineList[1].toInt();
        else if(lineList[0].indexOf("SwapFree") == 0)
            buffMem._swapFree = lineList[1].toInt();
    }
    return buffMem;
}

