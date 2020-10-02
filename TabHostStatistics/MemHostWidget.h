#ifndef MEMHOSTWIDGET_H
#define MEMHOSTWIDGET_H

#include <QWidget>

#include "BaseWidget.h"

namespace Ui {
    class MemHostWidget;
}

class MemHostWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit MemHostWidget(QWidget *parent = nullptr);
    ~MemHostWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::MemHostWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graphMem;
    QCPGraph *_graphSwap;

    double _valueStartMem;
    double _valueStartSwap;
};

#endif // MEMHOSTWIDGET_H
