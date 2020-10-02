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
    this->setGraphAxisType(_customPlot->xAxis, GraphAxisType_DateTime);
    this->setGraphAxisType(_customPlot->yAxis, GraphAxisType_Number);

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
        this->plotGraphData(_customPlot, _graphIn, in, _valueStartIn, diffIn);
        ui->labelReceiving->setText(QString("Receiving: %1Kb/c (diff: %2)")
                                    .arg(in)
                                    .arg(diffIn));

        double diffOut = 0;
        this->plotGraphData(_customPlot, _graphOut, out, _valueStartOut, diffOut);
        ui->labelSending->setText(QString("Sending: %1Kb/c (diff: %2)")
                                  .arg(out)
                                  .arg(diffOut));

        _resultLst.insert("/proc/net/dev", _resultLst.value("/proc/net/dev-last"));
        _resultLst.remove("/proc/net/dev-last");
    }
}

void NetworkHostWidget::stop()
{
    this->clearGraphData(_graphIn);
    this->clearGraphData(_graphOut);
    this->clearGraphRangeValues();

    _valueStartIn = -1;
    _valueStartOut = -1;
}
