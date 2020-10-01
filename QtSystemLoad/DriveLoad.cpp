#include "DriveLoad.h"

#include <QDebug>
#include <QStringList>
#include <QTextStream>

#include <sys/statvfs.h>

DriveLoad::DriveLoad(QObject *parent)
    : QObject(parent)
{
}

DriveLoad::~DriveLoad()
{
}

QVector<DriveInfo> DriveLoad::driveValue(const QByteArray &proc_filesystems,
                                         const QByteArray &proc_mounts)
{
//    QFile file("/proc/filesystems");
//    QFile file("/proc/mounts");

    QVector<QString> fsList;
    QTextStream processOutFS(proc_filesystems);
    while(!processOutFS.atEnd()) {
        QString readLine = processOutFS.readLine();
        if(readLine.indexOf("nodev") != -1)
            continue;

        fsList.push_back(readLine.trimmed());
    }

    QVector<DriveInfo> driveList;
    QTextStream processOut(proc_mounts);
    while(!processOut.atEnd()) {
        QString readLine = processOut.readLine();
        readLine = readLine.replace("  ", " ");
        QStringList lineList = readLine.split(" ");

        if(lineList.size() < 6)
            continue;

        DriveInfo dInfo;
        dInfo._device = lineList[0].trimmed().replace("\\040", " ");    // \040 - пробел
        dInfo._mountPoint = lineList[1].trimmed().replace("\\040", " ");
        dInfo._fileSystem = lineList[2].trimmed();
        dInfo._readType = lineList[3].trimmed();
        dInfo._dummyValue_0 = lineList[4].trimmed();
        dInfo._dummyValue_1 = lineList[5].trimmed();

        if(dInfo._device.isEmpty()
                || dInfo._mountPoint.isEmpty()
                || dInfo._fileSystem.isEmpty())
            continue;
        if(!fsList.contains(dInfo._fileSystem))
            continue;

        driveList.push_back(dInfo);
    }

    for(int i = 0; i < driveList.size(); i++) {
        struct statvfs statFS;
        if(statvfs(driveList[i]._mountPoint.toStdString().c_str(), &statFS) != 0)
            continue;

        driveList[i]._totalSize = statFS.f_blocks * statFS.f_bsize / 1024;
        driveList[i]._availableSize = statFS.f_bavail * statFS.f_bsize / 1024;
        driveList[i]._freeSize = statFS.f_bfree * statFS.f_bsize / 1024;
        driveList[i]._usageSize = driveList[i]._totalSize - driveList[i]._freeSize;
    }
    return driveList;
}

