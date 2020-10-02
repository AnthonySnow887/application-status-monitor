#include "SocketsGraphWidget.h"
#include "ui_SocketsGraphWidget.h"

#include "../BaseWidget.h"

SocketsGraphWidget::SocketsGraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketsGraphWidget)
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
    _valueStartTcp = -1;
    _valueStartUdp = -1;
}

SocketsGraphWidget::~SocketsGraphWidget()
{
    delete ui;
    delete  _customPlot;
}

void SocketsGraphWidget::processData(const uint &tcpCount, const uint &udpCount)
{
    double diffTcp = 0;
    double diffUdp = 0;
    this->plotGraphData(_customPlot, _graphTcp, tcpCount, _valueStartTcp, diffTcp);
    this->plotGraphData(_customPlot, _graphUdp, udpCount, _valueStartUdp, diffUdp);

    ui->labelTcp->setText(QString("TCP: %1 (diff: %2)").arg(tcpCount).arg(diffTcp));
    ui->labelUdp->setText(QString("UDP: %1 (diff: %2)").arg(udpCount).arg(diffUdp));
}

void SocketsGraphWidget::stop()
{
    this->clearGraphData(_graphTcp);
    this->clearGraphData(_graphUdp);
    this->clearGraphRangeValues();

    _valueStartTcp = -1;
    _valueStartUdp = -1;
}
