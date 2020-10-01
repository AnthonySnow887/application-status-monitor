#include "DriveHostWidget.h"
#include "ui_DriveHostWidget.h"

#include "QtSystemLoad/DriveLoad.h"

#include <QFont>
#include <QFontMetrics>
#include <qmath.h>

#define ISO_FS "iso"

DriveHostWidget::DriveHostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveHostWidget)
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

    //--
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();
}

DriveHostWidget::~DriveHostWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList DriveHostWidget::cmdList(const uint &pid) const
{
    Q_UNUSED(pid)
    return QStringList() << "cat /proc/filesystems" << "cat /proc/mounts";
}

void DriveHostWidget::processCmdResult(const QString &cmd, const QString &result)
{
    if (cmd.indexOf("/proc/filesystems") != -1)
        _resultLst.insert("/proc/filesystems", result.trimmed().toUtf8());
    if (cmd.indexOf("/proc/mounts") != -1)
        _resultLst.insert("/proc/mounts", result.trimmed().toUtf8());

    if (_resultLst.size() == 2) {
        QVector<DriveInfo> driveList = DriveLoad::driveValue(_resultLst.value("/proc/filesystems"),
                                                             _resultLst.value("/proc/mounts"));

        // remove iso
        for (int i = 0; i < driveList.size(); i++) {
            if (driveList[i].fileSystem().indexOf(ISO_FS) != -1) {
                driveList.remove(i);
                i--;
            }
        }

        // recreate graphs
        if (_graphMap.size() != driveList.size()) {
            _customPlot->legend->setVisible(false);
            this->removeAllGraphs();
            for (const DriveInfo &info: driveList) {
                if (!this->appendGraph(info.mountPoint(), info.mountPoint()))
                    return;
            }
            _customPlot->legend->setVisible(true);
        }

        // add graph data
        QString diskInfoStr;
        for (const DriveInfo &info : driveList) {
            QCPGraph *graph = _graphMap.value(info.mountPoint());
            if (!graph)
                continue;
            double vStart = _valueStartMap.value(info.mountPoint());
            double diff = 0;
            QString dSizeName("Gb");
            double dTotalSize = this->roundDouble(DriveInfo::convertToGb(info.totalSize()));
            double dUsageSize = this->roundDouble(DriveInfo::convertToGb(info.usageSize()));
            if (dTotalSize < 0) {
                dSizeName = "Mb";
                dTotalSize = this->roundDouble(DriveInfo::convertToMb(info.totalSize()));
                dUsageSize = this->roundDouble(DriveInfo::convertToMb(info.usageSize()));
            }

            this->plotData(graph, dUsageSize, vStart, diff);
            // replace value start
            _valueStartMap.insert(info.mountPoint(), vStart);
            // add text
            QString dText = QString("Disk \"%2\": %3%1 (diff: %4; Max: %5%1)")
                            .arg(dSizeName)
                            .arg(info.mountPoint())
                            .arg(dUsageSize)
                            .arg(diff)
                            .arg(dTotalSize);

            if (diskInfoStr.isEmpty())
                diskInfoStr = dText;
            else
                diskInfoStr += QString(";  %1").arg(dText);
        }
        ui->label->setText(diskInfoStr.trimmed());
        this->resizeLabel();
        _resultLst.clear();
    }
}

void DriveHostWidget::stop()
{
    this->clearAllGraphs();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();
}

double DriveHostWidget::roundDouble(const double &value) const
{
    return (static_cast<double>(qRound(value * 100)) / 100.0);
}

bool DriveHostWidget::appendGraph(const QString &mountPoint, const QString &name)
{
    if (name.isEmpty())
        return false;
    QCPGraph *graph = _customPlot->addGraph();
    graph->setName(name.trimmed());

    QColor color(this->randomColor());
    QPen pen(color);
    pen.setWidthF(1.2);
    graph->setPen(pen);

    _graphMap.insert(mountPoint, graph);
    _valueStartMap.insert(mountPoint, -1);
    return true;
}

void DriveHostWidget::removeAllGraphs()
{
    QMapIterator<QString, QCPGraph *> it(_graphMap);
    while (it.hasNext()) {
        it.next();
        _customPlot->removeGraph(it.value());
    }
    _graphMap.clear();
    _valueStartMap.clear();
}

void DriveHostWidget::clearAllGraphs()
{
    QList<QString> keys = _valueStartMap.keys();
    for (const QString &key : keys) {
        _valueStartMap.insert(key, -1);
        QCPGraph *graph = _graphMap.value(key, nullptr);
        if (graph)
            graph->clearData();
    }
}

void DriveHostWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
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

QColor DriveHostWidget::randomColor()
{
    return QColor(rand()%245 + 10, rand()%245 + 10, rand()%245 + 10);
}

void DriveHostWidget::resizeLabel()
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

void DriveHostWidget::resizeEvent(QResizeEvent *e)
{
    this->resizeLabel();
    e->accept();
}
