#include "CpuHostWidget.h"
#include "ui_CpuHostWidget.h"

#include "QtSystemLoad/CpuLoad.h"

#include <QFont>
#include <QFontMetrics>

CpuHostWidget::CpuHostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuHostWidget)
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
}

CpuHostWidget::~CpuHostWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList CpuHostWidget::cmdList(const uint &pid) const
{
    Q_UNUSED(pid)
    return QStringList() << "cat /proc/stat";
}

void CpuHostWidget::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    QString keyPrefix;
    if (_resultLst.size() >= 1)
        keyPrefix = QString("-last");
    _resultLst.insert(QString("/proc/stat%1").arg(keyPrefix), result.trimmed().toUtf8());
    if (_resultLst.size() == 2) {
        QVector<CpuInfo> cpuInfo = CpuLoad::cpuValue(_resultLst.value("/proc/stat"));
        cpuInfo = CpuLoad::cpuValue(_resultLst.value("/proc/stat-last"), cpuInfo);

        // remove average value
        for (int i = 0; i < cpuInfo.size(); i++) {
            if (cpuInfo[i].cpuId() == -1) {
                cpuInfo.remove(i);
                break;
            }
        }

        // recreate graphs
        if (_graphMap.size() != cpuInfo.size()) {
            _customPlot->legend->setVisible(false);
            this->removeAllGraphs();
            for (const CpuInfo &info: cpuInfo) {
                if (!this->appendGraph(info.cpuId(), QString("CPU %1").arg(info.cpuId() + 1)))
                    return;
            }
            _customPlot->legend->setVisible(true);
        }

        // add graph data
        QString cpuInfoStr;
        for (const CpuInfo &info : cpuInfo) {
            QCPGraph *graph = _graphMap.value(info.cpuId());
            if (!graph)
                continue;
            double vStart = _valueStartMap.value(info.cpuId());
            double diff = 0;
            double cpu = info.cpuUsage();
            cpu = static_cast<double>(qRound(cpu * 100)) / 100.0;
            this->plotGraphData(_customPlot, graph, cpu, vStart, diff);
            // replace value start
            _valueStartMap.insert(info.cpuId(), vStart);
            // add text
            QString cpuText = QString("CPU %1: %2% (diff: %3)")
                              .arg(info.cpuId() + 1)
                              .arg(cpu)
                              .arg(diff);
            if (cpuInfoStr.isEmpty())
                cpuInfoStr = cpuText;
            else
                cpuInfoStr += QString(";  %1").arg(cpuText);
        }
        ui->label->setText(cpuInfoStr.trimmed());
        this->resizeLabel();
        _resultLst.insert("/proc/stat", _resultLst.value("/proc/stat-last"));
        _resultLst.remove("/proc/stat-last");
    }
}

void CpuHostWidget::stop()
{
    this->clearAllGraphs();
    this->clearGraphRangeValues();
}

bool CpuHostWidget::appendGraph(const int &cpu_id, const QString &name)
{
    if (name.isEmpty())
        return false;
    QCPGraph *graph = _customPlot->addGraph();
    graph->setName(name.trimmed());

    QColor color(this->randomColor());
    QPen pen(color);
    pen.setWidthF(1.2);
    graph->setPen(pen);

    _graphMap.insert(cpu_id, graph);
    _valueStartMap.insert(cpu_id, -1);
    return true;
}

void CpuHostWidget::removeAllGraphs()
{
    QMapIterator<int /*cpu id*/, QCPGraph *> it(_graphMap);
    while (it.hasNext()) {
        it.next();
        _customPlot->removeGraph(it.value());
    }
    _graphMap.clear();
    _valueStartMap.clear();
}

void CpuHostWidget::clearAllGraphs()
{
    QList<int> keys = _valueStartMap.keys();
    for (const int &key : keys) {
        _valueStartMap.insert(key, -1);
        QCPGraph *graph = _graphMap.value(key, nullptr);
        this->clearGraphData(graph);
    }
}

void CpuHostWidget::resizeLabel()
{
    QFont lFont = ui->label->font();
    lFont.fromString(ui->label->text());
    QFontMetrics fontMetrics(lFont);
    int lWidth = fontMetrics.width(ui->label->text());
    int lMaxHeight = ui->label->maximumHeight();
    if (lWidth > this->width()) {
        int diff = lWidth / this->width();
        ui->label->setMaximumHeight(25 * (diff + 1));
    } else {
        ui->label->setMaximumHeight(25);
    }
    if (lMaxHeight != ui->label->maximumHeight())
        ui->label->resize(ui->label->width(), ui->label->maximumHeight());
}

void CpuHostWidget::resizeEvent(QResizeEvent *e)
{
    this->resizeLabel();
    e->accept();
}
