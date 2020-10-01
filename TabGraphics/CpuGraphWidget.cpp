#include "CpuGraphWidget.h"
#include "ui_CpuGraphWidget.h"

#include "QtSystemLoad/ProcessLoad.h"

#include <qmath.h>

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
    _customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    _customPlot->yAxis->setTickLabelType(QCPAxis::ltNumber);

    // - 1 -
    _graph = _customPlot->addGraph();
    _graph->setName("CPU");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graph->setPen(pen);

    //--
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

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
        double cpu = pInfo.userCpuUsage();
        cpu = static_cast<double>(qRound(cpu * 100)) / 100.0;
        this->plotData(_graph, cpu, _valueStart, diff);

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
    _graph->clearData();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStart = -1;
}

void CpuGraphWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
{
    if (!graph)
        return;
    // plot data
    uint cDateTime = QDateTime::currentDateTime().toTime_t();
    graph->addData(cDateTime, value);

    // set axis range
    double lastV = 0;

    QCPDataMap *data = graph->data();
    QList<double> dKeys = data->keys();
    QMapIterator<double, QCPData> it(*data);
    while (it.hasNext()) {
        it.next();
        if (startValue == -1)
            startValue = it.value().value;
        if (it.key() < _xMin)
            _xMin = it.key();
        if (it.key() > _xMax)
            _xMax = it.key();
        if (it.value().value < _valueMin)
            _valueMin = it.value().value;
        if (it.value().value > _valueMax)
            _valueMax = it.value().value;

        lastV = it.value().value;
    }
    if (dKeys.size() > 1) {
        double diffKey = dKeys[dKeys.size() - 1] - dKeys[0];
        if (diffKey >= _maxDataTimeSec) {
            graph->removeData(dKeys[0]);
            _xMin = dKeys[1];
        }
    }

    diff = lastV - startValue;

    _customPlot->xAxis->setRange(_xMin, _xMax + 5);
    _customPlot->xAxis->setPadding(5); // a bit more space to the left border
    int vDiff = (_valueMax - _valueMin) / 2;
    if (vDiff < 5)
        vDiff = 5;
    _customPlot->yAxis->setRange(_valueMin, _valueMax + vDiff);
    _customPlot->yAxis->setPadding(5);
    _customPlot->replot();
}
