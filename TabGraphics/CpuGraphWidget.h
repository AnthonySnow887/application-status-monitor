#ifndef CPUGRAPHWIDGET_H
#define CPUGRAPHWIDGET_H

#include <QWidget>
#include <QMap>

#include "BaseWidget.h"

namespace Ui {
    class CpuGraphWidget;
}

class CpuGraphWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit CpuGraphWidget(QWidget *parent = nullptr);
    ~CpuGraphWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::CpuGraphWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graph;

    QMap<QString, QByteArray> _resultLst;

    double _valueStart;
};

#endif // CPUGRAPHWIDGET_H
