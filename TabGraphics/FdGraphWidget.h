#ifndef FDGRAPHWIDGET_H
#define FDGRAPHWIDGET_H

#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

#include <QMap>

namespace Ui {
    class FdGraphWidget;
}

class FdGraphWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit FdGraphWidget(QWidget *parent = nullptr);
    ~FdGraphWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::FdGraphWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graph;

//    QMap<QString, double> _resultLst; // TODO delete...

    double _xMin;
    double _xMax;
    double _valueMin;
    double _valueMax;

    double _valueStart;

    void plotData(QCPGraph *graph, const double &value, double &startValue, double &diff);
};

#endif // FDGRAPHWIDGET_H
