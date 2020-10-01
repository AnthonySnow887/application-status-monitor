#include "NetworkHostWidget.h"
#include "ui_NetworkHostWidget.h"

#include "QtSystemLoad/NetworkLoad.h"

NetworkHostWidget::NetworkHostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkHostWidget)
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
    _graphIn = _customPlot->addGraph();
    _graphIn->setName("Receiving");

    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1.2);
    _graphIn->setPen(pen);

    // - 2 -
    _graphOut = _customPlot->addGraph();
    _graphOut->setName("Sending");

    QColor color2(Qt::red);
    QPen pen2(color2);
    pen2.setWidthF(1.2);
    _graphOut->setPen(pen2);

    //--
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartIn = -1;
    _valueStartOut = -1;
}

NetworkHostWidget::~NetworkHostWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList NetworkHostWidget::cmdList(const uint &pid) const
{
    Q_UNUSED(pid)
    return QStringList() << "cat /proc/net/dev";
}

void NetworkHostWidget::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    QString keyPrefix;
    if (_resultLst.size() >= 1)
        keyPrefix = QString("-last");
    _resultLst.insert(QString("/proc/net/dev%1").arg(keyPrefix), result.trimmed().toUtf8());
    if (_resultLst.size() == 2) {
        QVector<NetworkInfo> nInfo = NetworkLoad::networkValue(_resultLst.value("/proc/net/dev"));
        nInfo = NetworkLoad::networkValue(_resultLst.value("/proc/net/dev-last"), nInfo);
        double in = 0;
        double out = 0;
        for (const NetworkInfo &info : nInfo) {
            in += NetworkInfo::convertToKb(info.inBytesNow());
            out += NetworkInfo::convertToKb(info.outBytesNow());
        }
        in = this->roundDouble(in);
        out = this->roundDouble(out);
        double diffIn = 0;
        this->plotData(_graphIn, in, _valueStartIn, diffIn);
        ui->labelReceiving->setText(QString("Receiving: %1Kb/c (diff: %2)")
                                    .arg(in)
                                    .arg(diffIn));

        double diffOut = 0;
        this->plotData(_graphOut, out, _valueStartOut, diffOut);
        ui->labelSending->setText(QString("Sending: %1Kb/c (diff: %2)")
                                  .arg(out)
                                  .arg(diffOut));

        _resultLst.insert("/proc/net/dev", _resultLst.value("/proc/net/dev-last"));
        _resultLst.remove("/proc/net/dev-last");
    }
}

void NetworkHostWidget::stop()
{
    _graphIn->clearData();
    _graphOut->clearData();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStartIn = -1;
    _valueStartOut = -1;
}

double NetworkHostWidget::roundDouble(const double &value) const
{
    return (static_cast<double>(qRound(value * 100)) / 100.0);
}

void NetworkHostWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
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
