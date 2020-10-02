#include "ThreadsGraphWidget.h"
#include "ui_ThreadsGraphWidget.h"

ThreadsGraphWidget::ThreadsGraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThreadsGraphWidget)
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
    _graph->setName("Threads");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graph->setPen(pen);

    //--
    _valueStart = -1;
}

ThreadsGraphWidget::~ThreadsGraphWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList ThreadsGraphWidget::cmdList(const uint &pid) const
{
    QStringList cmd;
    cmd << QString("ps -T -p %1 | wc -l").arg(pid);
    return cmd;
}

void ThreadsGraphWidget::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    int threadSize = result.trimmed().toInt() - 1; // remove header
    if (threadSize < 0)
        threadSize = 0;
    double diff = 0;
    this->plotGraphData(_customPlot, _graph, threadSize, _valueStart, diff);
    ui->labelCpu->setText(QString("Threads: %1 (diff: %2)").arg(threadSize).arg(diff));
}

void ThreadsGraphWidget::stop()
{
    this->clearGraphData(_graph);
    this->clearGraphRangeValues();

    _valueStart = -1;
}
