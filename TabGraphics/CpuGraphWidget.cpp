#include "CpuGraphWidget.h"
#include "ui_CpuGraphWidget.h"

#include "QtSystemLoad/ProcessLoad.h"

CpuGraphWidget::CpuGraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuGraphWidget)
{
    ui->setupUi(this);

    _customPlot = new QCustomPlot(this);
    ui->verticalLayoutGraph->addWidget(_customPlot);

    // --
    _customPlot->xAxis->setTickLength(0, 5);
    _customPlot->yAxis->setTickLength(0, 5);
    _customPlot->legend->setVisible(false);
    _customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    _customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    _customPlot->xAxis->grid()->setSubGridPen(gridPen);
    _customPlot->yAxis->grid()->setSubGridPen(gridPen);
    this->setGraphAxisType(_customPlot->xAxis, GraphAxisType_DateTime);
    this->setGraphAxisType(_customPlot->yAxis, GraphAxisType_Number);

    // - 1 -
    _graph = _customPlot->addGraph();
    _graph->setName("CPU");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graph->setPen(pen);

    // --
    _valueStart = -1;
}

CpuGraphWidget::~CpuGraphWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList CpuGraphWidget::cmdList(const uint &pid) const
{
    QStringList cmd;
    cmd << QString("cat /proc/stat");
    cmd << QString("cat /proc/%1/stat").arg(pid);
    cmd << QString("cat /proc/%1/status").arg(pid);
    return cmd;
}

void CpuGraphWidget::processCmdResult(const QString &cmd, const QString &result)
{
    QString keyPrefix;
    if (_resultLst.size() >= 3)
        keyPrefix = QString("-last");
    if (cmd.indexOf("/proc/stat") != -1)
        _resultLst.insert(QString("/proc/stat%1").arg(keyPrefix), result.trimmed().toUtf8());
    else if (cmd.indexOf("/status") != -1)
        _resultLst.insert(QString("/proc/PID/status%1").arg(keyPrefix), result.trimmed().toUtf8());
    else if (cmd.indexOf("/stat") != -1)
        _resultLst.insert(QString("/proc/PID/stat%1").arg(keyPrefix), result.trimmed().toUtf8());

    if (_resultLst.size() == 6) {
        ProcessInfo pInfo = ProcessLoad::processValue(_resultLst.value("/proc/stat"),
                                                      _resultLst.value("/proc/PID/stat"),
                                                      _resultLst.value("/proc/PID/status"));

        pInfo = ProcessLoad::processValue(_resultLst.value("/proc/stat-last"),
                                          _resultLst.value("/proc/PID/stat-last"),
                                          _resultLst.value("/proc/PID/status-last"),
                                          pInfo);

        double diff = 0;
        double cpu = this->roundDouble(pInfo.userCpuUsage());
        this->plotGraphData(_customPlot, _graph, cpu, _valueStart, diff);

        ui->labelCpu->setText(QString("CPU: %1 (diff: %2)").arg(cpu).arg(diff));

        _resultLst.insert("/proc/stat", _resultLst.value("/proc/stat-last"));
        _resultLst.insert("/proc/PID/stat", _resultLst.value("/proc/PID/stat-last"));
        _resultLst.insert("/proc/PID/status", _resultLst.value("/proc/PID/status-last"));
        _resultLst.remove("/proc/stat-last");
        _resultLst.remove("/proc/PID/stat-last");
        _resultLst.remove("/proc/PID/status-last");
    }
}

void CpuGraphWidget::stop()
{
    this->clearGraphData(_graph);
    this->clearGraphRangeValues();
    _valueStart = -1;
}
