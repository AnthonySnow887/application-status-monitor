#ifndef CPUHOSTWIDGET_H
#define CPUHOSTWIDGET_H

#include <QWidget>

#include "BaseWidget.h"
#include "QCustomPlot/qcustomplot.h"

namespace Ui {
    class CpuHostWidget;
}

class CpuInfo;

class CpuHostWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit CpuHostWidget(QWidget *parent = nullptr);
    ~CpuHostWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::CpuHostWidget *ui;

    QCustomPlot *_customPlot;
    QMap<int /*cpu id*/, QCPGraph *> _graphMap;

    QMap<QString, QByteArray> _resultLst;
    QMap<int /*cpu id*/, double> _valueStartMap;

    bool appendGraph(const int &cpu_id, const QString &name);
    void removeAllGraphs();
    void clearAllGraphs();

    void resizeLabel();

protected:
    void resizeEvent(QResizeEvent *e) override;
};

#endif // CPUHOSTWIDGET_H
