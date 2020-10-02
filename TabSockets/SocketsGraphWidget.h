#ifndef SOCKETSGRAPHWIDGET_H
#define SOCKETSGRAPHWIDGET_H

#include <QWidget>

#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class SocketsGraphWidget;
}

class SocketsGraphWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit SocketsGraphWidget(QWidget *parent = nullptr);
    ~SocketsGraphWidget();

    void processData(const uint &tcpCount, const uint &udpCount);

    QStringList cmdList(const uint &) const override { return QStringList(); }
    void processCmdResult(const QString &, const QString &) override {}
    void stop() override;

private:
    Ui::SocketsGraphWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graphTcp;
    QCPGraph *_graphUdp;

    double _valueStartTcp;
    double _valueStartUdp;
};

#endif // SOCKETSGRAPHWIDGET_H
