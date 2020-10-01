#ifndef SOCKETSGRAPHWIDGET_H
#define SOCKETSGRAPHWIDGET_H

#include <QWidget>

#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class SocketsGraphWidget;
}

class SocketsGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SocketsGraphWidget(QWidget *parent = nullptr);
    ~SocketsGraphWidget();

    void setMaxDataTime(const uint &sec);

    void processData(const uint &tcpCount, const uint &udpCount);
    void stop();

private:
    Ui::SocketsGraphWidget *ui;

    uint _maxDataTimeSec;

    QCustomPlot *_customPlot;
    QCPGraph *_graphTcp;
    QCPGraph *_graphUdp;

    double _xMin;
    double _xMax;
    double _valueMin;
    double _valueMax;

    double _valueStartTcp;
    double _valueStartUdp;

    void plotData(QCPGraph *graph, const double &value, double &startValue, double &diff);
};

#endif // SOCKETSGRAPHWIDGET_H
