#ifndef THREADSGRAPHWIDGET_H
#define THREADSGRAPHWIDGET_H

#include <QWidget>

#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class ThreadsGraphWidget;
}

class ThreadsGraphWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit ThreadsGraphWidget(QWidget *parent = nullptr);
    ~ThreadsGraphWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::ThreadsGraphWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graph;

    QMap<QString, QByteArray> _resultLst;

    double _xMin;
    double _xMax;
    double _valueMin;
    double _valueMax;

    double _valueStart;

    void plotData(QCPGraph *graph, const double &value, double &startValue, double &diff);
};

#endif // THREADSGRAPHWIDGET_H
