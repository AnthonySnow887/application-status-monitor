#ifndef MEMGRAPHWIDGET_H
#define MEMGRAPHWIDGET_H

#include <QWidget>

#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class MemGraphWidget;
}

class MemGraphWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit MemGraphWidget(QWidget *parent = nullptr);
    ~MemGraphWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::MemGraphWidget *ui;

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

#endif // MEMGRAPHWIDGET_H
