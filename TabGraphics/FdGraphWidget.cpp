#include "FdGraphWidget.h"
#include "ui_FdGraphWidget.h"

FdGraphWidget::FdGraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FdGraphWidget)
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
    _graph->setName("Open FD");

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

FdGraphWidget::~FdGraphWidget()
{
    delete ui;
    delete _customPlot;
}

QStringList FdGraphWidget::cmdList(const uint &pid) const
{
    QStringList cmd;
//    cmd << QString("lsof -p %1 | awk '{print $1}' | sort | uniq -c | sort -r | head").arg(pid); // lsof incorrect size if used root-user
    cmd << QString("ls -la /proc/%1/fd | wc -l").arg(pid);
    return cmd;
}

/*
 * old wersion with lsof result
 *
void FdGraphWidget::processCmdResult(const QString &cmd, const QString &result)
{
    if (cmd.indexOf("lsof -p") != -1) {
        QStringList lst = result.trimmed().split("\n");
        if (lst.size() == 0)
            return;
        for (const QString row : lst) {
            if (row.indexOf("COMMAND") != -1)
                continue;
            QStringList lst2 = row.trimmed().split(" ");
            if (lst2.size() != 2)
                _resultLst.insert("lsof", 0);
            else
                _resultLst.insert("lsof", lst2[0].toDouble());
            break;
        }
    } else if (cmd.indexOf("ls -la") != -1) {
        _resultLst.insert("ls", result.trimmed().toDouble());
    }

    if (_resultLst.size() == 2) {
        double diffLsof = 0;
        double diffLs = 0;
        this->plotData(_fdGraphLsof, _resultLst.value("lsof", 0), _valueStartLsof, diffLsof);
        this->plotData(_fdGraphLs, _resultLst.value("ls", 0), _valueStartLs, diffLs);
        // Open Files Descriptors: 0 (diff: 0)
//        ui->labelFdLsof->setText(QString("FD Size (lsof): %1 (diff: %2)").arg(_resultLst.value("lsof", 0)).arg(diffLsof));
//        ui->labelFdLs->setText(QString("FD Size (ls): %1 (diff: %2)").arg(_resultLst.value("ls", 0)).arg(diffLs));
        _resultLst.clear();
    }
}
*/

void FdGraphWidget::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    int fdSize = result.trimmed().toInt() - 3; // remove header, '.' and '..'
    if (fdSize < 0)
        fdSize = 0;
    double diff = 0;
    this->plotData(_graph, fdSize, _valueStart, diff);
    ui->labelFd->setText(QString("Open Files Descriptors: %1 (diff: %2)")
                         .arg(fdSize)
                         .arg(diff));
}

void FdGraphWidget::stop()
{
    _graph->clearData();

    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();

    _valueStart = -1;
}

void FdGraphWidget::plotData(QCPGraph *graph, const double &value, double &startValue, double &diff)
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
