#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QString>
#include <QStringList>
#include <QColor>

#include "QCustomPlot/qcustomplot.h"

#define MIN_DATA_DIFF_TIME_SEC 60   // 1 min
#define MAX_DATA_DIFF_TIME_SEC 3600 // 1 hour

#define QCUSTOMPLOT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

class BaseWidget
{
public:
    enum GraphAxisType {
        GraphAxisType_Number = 0,
        GraphAxisType_DateTime
    };

    BaseWidget();
    virtual ~BaseWidget();

    void setMaxDataTime(const uint &sec);

    virtual QStringList cmdList(const uint &pid) const = 0;
    virtual void processCmdResult(const QString &cmd, const QString &result) = 0;
    virtual void stop() = 0;

protected:
    uint _maxDataTimeSec;

    double _xMin;
    double _xMax;
    double _valueMin;
    double _valueMax;

    void setGraphAxisType(QCPAxis *axis,
                          const GraphAxisType &type);

    void clearGraphRangeValues();
    void plotGraphData(QCustomPlot *customPlot,
                       QCPGraph *graph,
                       const double &value,
                       double &startValue,
                       double &diff);
    void clearGraphData(QCPGraph *graph);

    QColor randomColor() const;

    double roundDouble(const double &value) const;
};

#endif // BASEWIDGET_H
