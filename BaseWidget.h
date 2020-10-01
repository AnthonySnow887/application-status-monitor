#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QString>
#include <QStringList>

#define MIN_DATA_DIFF_TIME_SEC 60   // 1 min
#define MAX_DATA_DIFF_TIME_SEC 3600 // 1 hour

class BaseWidget
{
public:
    BaseWidget()
        : _maxDataTimeSec(MIN_DATA_DIFF_TIME_SEC)
    {}

    virtual ~BaseWidget() {}

    void setMaxDataTime(const uint &sec) {
        if (sec < MIN_DATA_DIFF_TIME_SEC)
            _maxDataTimeSec = MIN_DATA_DIFF_TIME_SEC;
        else
            _maxDataTimeSec = sec;
    }

    virtual QStringList cmdList(const uint &pid) const = 0;
    virtual void processCmdResult(const QString &cmd, const QString &result) = 0;
    virtual void stop() = 0;

protected:
    uint _maxDataTimeSec;
};

#endif // BASEWIDGET_H
