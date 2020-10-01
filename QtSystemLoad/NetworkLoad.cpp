#include "NetworkLoad.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

NetworkLoad::NetworkLoad(QObject *parent)
    : QObject(parent)
{
}

NetworkLoad::~NetworkLoad()
{
}

QVector<NetworkInfo> NetworkLoad::networkValue(const QByteArray &proc_net_dev,
                                               QVector<NetworkInfo> network)
{
//    QFile file("/proc/net/dev");

// SUSE
//    Inter-|   Receive                                                |  Transmit
//     face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
//    vmnet8:    0       0    0    0    0     0          0         0        0       6    0    0    0     0       0          0

// RedHat 6.4
//    Inter-|   Receive                                                |  Transmit
//     face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
//        lo:169731600  626313    0    0    0     0          0         0 169731600  626313    0    0    0     0       0          0
//      eth0:3563180668 3007254    0    0    0     0          0         0 3030581196 2007212    0    0    0     0       0          0


    QVector<NetworkInfo> buffNetwork;
    QMap<QString, int> valueIndex;

    QTextStream processOut(proc_net_dev);
    while(!processOut.atEnd()) {
        QString readLine = processOut.readLine();
        if(readLine.indexOf("bytes") != -1) {
            QStringList lineList = readLine.split("|");
            for(int i = 0; i < lineList.size(); i++) {
                if(lineList[i].indexOf("bytes") == -1)
                    continue;
                QStringList valueList = lineList[i].split(" ");
                int buffIndex = 0;
                for(int j = 0; j < valueList.size(); j++) {
                    if(valueList[j].isEmpty())
                        continue;
                    if(!valueIndex.contains(valueList[j]))
                        valueIndex.insert(valueList[j], buffIndex++);
                }
            }
        }
        if (readLine.indexOf("|") != -1)
            continue;
        if (readLine.indexOf(":") == -1)
            continue;

        readLine = readLine.replace("  ", " ");
        QStringList lineList = readLine.trimmed().split(":");
        if (lineList.size() < 2)
            continue;
        QString netName = lineList[0].trimmed();
        lineList = lineList[1].trimmed().split(" ");

        QVector<int> buffIndex;
        for(int i = 0; i < lineList.size(); i++) {
            if(lineList[i].isEmpty())
                buffIndex.push_back(i);
        }
        for(int i = buffIndex.size() - 1; i >= 0; i--)
            lineList.removeAt(buffIndex[i]);

        int minSize = 0;
        QMapIterator<QString, int> i(valueIndex);
        while(i.hasNext()) {
            i.next();
            if(minSize < i.value())
                minSize = i.value();
        }
        minSize++;

        if(lineList.size() < (minSize * 2))
            continue;

        NetworkInfo netVal;
        if(!network.isEmpty()) {
            for(int i = 0; i < network.size(); i++) {
                if(network[i].name() == netName) {
                    netVal = network[i];
                    netVal._inBytesPrev = netVal._inBytes;
                    netVal._inPacketsPrev = netVal._inPackets;
                    netVal._outBytesPrev = netVal._outBytes;
                    netVal._outPacketsPrev = netVal._outPackets;
                    break;
                }
            }
        }

        netVal._name = netName;
        netVal._inBytes = lineList[valueIndex.value("bytes")].toLongLong();
        netVal._inPackets = lineList[valueIndex.value("packets")].toLongLong();
        netVal._outBytes = lineList[valueIndex.value("bytes") + minSize].toLongLong();
        netVal._outPackets = lineList[valueIndex.value("packets") + minSize].toLongLong();
        buffNetwork.push_back(netVal);

//        qDebug() << netVal._name << netVal._inBytes << netVal._inPackets << netVal._outBytes << netVal._outPackets;
    }
    return buffNetwork;
}
