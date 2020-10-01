#include "SocketsGraphWidget.h"
#include "ui_SocketsGraphWidget.h"

#include "../BaseWidget.h"

SocketsGraphWidget::SocketsGraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketsGraphWidget),
    _maxDataTimeSec(MIN_DATA_DIFF_TIME_SEC)
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
    _graphTcp = _customPlot->addGraph();
    _graphTcp->setName("TCP");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graphTcp->setPen(pen);\

    // - 2 -
    _graphUdp = _customPlot->addGraph();
    _graphUdp->setName("UDP");

    QColor color2(Qt::red);
    QPen pen2(color2);
    pen2.setWidthF(1.2);
    _graphUdp->setPen(pen2);

    //--
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartTcp = -1;
    _valueStartUdp = -1;
}

SocketsGraphWidget::~SocketsGraphWidget()
{
    delete ui;
}

void SocketsGraphWidget::setMaxDataTime(const uint &sec)
{
    if (sec < MIN_DATA_DIFF_TIME_SEC)
        _maxDataTimeSec = MIN_DATA_DIFF_TIME_SEC;
    else
        _maxDataTimeSec = sec;
}

void SocketsGraphWidget::processData(const uint &tcpCount, const uint &udpCount)
{
    double diffTcp = 0;
    double diffUdp = 0;
    this->plotData(_graphTcp, tcpCount, _valueStartTcp, diffTcp);
    this->plotData(_graphUdp, udpCount, _valueStartUdp, diffUdp);

    ui->labelTcp->setText(QString("TCP: %1 (diff: %2)").arg(tcpCount).arg(diffTcp));
    ui->labelUdp->setText(QString("UDP: %1 (diff: %2)").arg(udpCount).arg(diffUdp));
}

void SocketsGraphWidget::stop()
{
    _graphTcp->clearData();
    _graphUdp->clearData();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartTcp = -1;
    _valueStartUdp = -1;
}

void SocketsGraphWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
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
