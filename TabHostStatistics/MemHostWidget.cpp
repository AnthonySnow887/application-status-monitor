#include "MemHostWidget.h"
#include "ui_MemHostWidget.h"

#include "QtSystemLoad/MemoryLoad.h"

MemHostWidget::MemHostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemHostWidget)
{
    ui->setupUi(this);

    _customPlot = new QCustomPlot(this);
    ui->verticalLayoutGraph->addWidget(_customPlot);

    // --
    _customPlot->xAxis->setTickLength(0, 5);
    _customPlot->yAxis->setTickLength(0, 5);
    _customPlot->legend->setVisible(true);
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
    _graphMem = _customPlot->addGraph();
    _graphMem->setName("Memory");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graphMem->setPen(pen);

    // - 2 -
    _graphSwap = _customPlot->addGraph();
    _graphSwap->setName("Swap");

    QColor color2(Qt::red);
    QPen pen2(color2);
    pen2.setWidthF(1.2);
    _graphSwap->setPen(pen2);

    //--
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartMem = -1;
    _valueStartSwap = -1;
}

MemHostWidget::~MemHostWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList MemHostWidget::cmdList(const uint &pid) const
{
    Q_UNUSED(pid)
    return QStringList() << "cat /proc/meminfo";
}

void MemHostWidget::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    MemoryInfo memInfo = MemoryLoad::memoryValue(result.trimmed().toUtf8());
    double diffMem = 0;
    QString memSizeName("Gb");
    double dataMem = this->roundDouble(MemoryInfo::convertToGb(memInfo.memoryUsage()));
    double dataMemTotal = this->roundDouble(MemoryInfo::convertToGb(memInfo.memoryTotal()));
    if (dataMemTotal < 0) {
        memSizeName = "Mb";
        dataMem = this->roundDouble(MemoryInfo::convertToMb(memInfo.memoryUsage()));
        dataMemTotal = this->roundDouble(MemoryInfo::convertToMb(memInfo.memoryTotal()));
    }
    if (dataMemTotal < 0) {
        memSizeName = "Kb";
        dataMem = this->roundDouble(MemoryInfo::convertToKb(memInfo.memoryUsage()));
        dataMemTotal = this->roundDouble(MemoryInfo::convertToKb(memInfo.memoryTotal()));
    }
    this->plotData(_graphMem, dataMem, _valueStartMem, diffMem);

    double diffSwap = 0;
    QString swapSizeName("Gb");
    double dataSwap = this->roundDouble(MemoryInfo::convertToGb(memInfo.swapUsage()));
    double dataSwapTotal = this->roundDouble(MemoryInfo::convertToGb(memInfo.swapTotal()));
    if (dataSwapTotal < 0) {
        swapSizeName = "Mb";
        dataSwap = this->roundDouble(MemoryInfo::convertToMb(memInfo.swapUsage()));
        dataSwapTotal = this->roundDouble(MemoryInfo::convertToMb(memInfo.swapTotal()));
    }
    if (dataSwapTotal < 0) {
        swapSizeName = "Kb";
        dataSwap = this->roundDouble(MemoryInfo::convertToKb(memInfo.swapUsage()));
        dataSwapTotal = this->roundDouble(MemoryInfo::convertToKb(memInfo.swapTotal()));
    }
    this->plotData(_graphSwap, dataSwap, _valueStartSwap, diffSwap);

    //Memory: 0Mb (diff: 0; Max: 0Mb)
    ui->labelMem->setText(QString("Memory: %2%1 (diff: %3; Max: %4%1)")
                          .arg(memSizeName)
                          .arg(dataMem)
                          .arg(diffMem)
                          .arg(dataMemTotal));

    //Swap: 0Mb (diff: 0; Max: 0Mb)
    ui->labelSwap->setText(QString("Swap: %2%1 (diff: %3; Max: %4%1)")
                           .arg(swapSizeName)
                           .arg(dataSwap)
                           .arg(diffSwap)
                           .arg(dataSwapTotal));
}

void MemHostWidget::stop()
{
    _graphMem->clearData();
    _graphSwap->clearData();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartMem = -1;
    _valueStartSwap = -1;
}

double MemHostWidget::roundDouble(const double &value) const
{
    return (static_cast<double>(qRound(value * 100)) / 100.0);
}

void MemHostWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
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
