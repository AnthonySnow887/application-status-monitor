#ifndef NETWORKHOSTWIDGET_H
#define NETWORKHOSTWIDGET_H

#include <QWidget>
#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class NetworkHostWidget;
}

class NetworkHostWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit NetworkHostWidget(QWidget *parent = nullptr);
    ~NetworkHostWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::NetworkHostWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graphIn;
    QCPGraph *_graphOut;

    QMap<QString, QByteArray> _resultLst;

    double _xMin;
    double _xMax;
    double _valueMin;
    double _valueMax;

    double _valueStartIn;
    double _valueStartOut;

    double roundDouble(const double &value) const;
    void plotData(QCPGraph *graph, const double &value, double &startValue, double &diff);
};

#endif // NETWORKHOSTWIDGET_H
