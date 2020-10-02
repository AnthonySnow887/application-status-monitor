#include "DriveHostWidget.h"
#include "ui_DriveHostWidget.h"

#include "QtSystemLoad/DriveLoad.h"

#include <QFont>
#include <QFontMetrics>

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
    this->setGraphAxisType(_customPlot->xAxis, GraphAxisType_DateTime);
    this->setGraphAxisType(_customPlot->yAxis, GraphAxisType_Number);
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
            double dTotalSize = this->roundDoubleAbs(DriveInfo::convertToGb(info.totalSize()));
            double dUsageSize = this->roundDoubleAbs(DriveInfo::convertToGb(info.usageSize()));
            if (dTotalSize < 0) {
                dSizeName = "Mb";
                dTotalSize = this->roundDoubleAbs(DriveInfo::convertToMb(info.totalSize()));
                dUsageSize = this->roundDoubleAbs(DriveInfo::convertToMb(info.usageSize()));
            }

            this->plotGraphData(_customPlot, graph, dUsageSize, vStart, diff);
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
    this->clearGraphRangeValues();
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
        this->clearGraphData(graph);
    }
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
