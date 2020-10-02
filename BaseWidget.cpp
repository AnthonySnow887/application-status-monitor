#include "BaseWidget.h"

#include <qmath.h>

BaseWidget::BaseWidget()
    : _maxDataTimeSec(MIN_DATA_DIFF_TIME_SEC)
{
    this->clearGraphRangeValues();
}

BaseWidget::~BaseWidget()
{}

void BaseWidget::setMaxDataTime(const uint &sec)
{
    if (sec < MIN_DATA_DIFF_TIME_SEC)
        _maxDataTimeSec = MIN_DATA_DIFF_TIME_SEC;
    else
        _maxDataTimeSec = sec;
}

void BaseWidget::setGraphAxisType(QCPAxis *axis, const GraphAxisType &type)
{
    if (!axis)
        return;
    switch (type) {
        case GraphAxisType_Number: {
#if (QCUSTOMPLOT_VERSION >= QCUSTOMPLOT_VERSION_CHECK(2,0,0))
            // configure bottom axis to show date instead of number:
            QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
            axis->setTicker(ticker);
#else
            axis->setTickLabelType(QCPAxis::ltNumber);
#endif
            break;
        }
        case GraphAxisType_DateTime: {
#if (QCUSTOMPLOT_VERSION >= QCUSTOMPLOT_VERSION_CHECK(2,0,0))
            // configure bottom axis to show date instead of number:
            QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
            dateTicker->setDateTimeFormat("hh:mm:ss\ndd.MM.yy");
            axis->setTicker(dateTicker);
#else
            axis->setTickLabelType(QCPAxis::ltDateTime);
#endif
            break;
        }
        default:
            break;
    }
}

void BaseWidget::clearGraphRangeValues()
{
    _xMin = std::numeric_limits<double>::max();
    _xMax = std::numeric_limits<double>::min();
    _valueMin = 0; //std::numeric_limits<double>::max();
    _valueMax = std::numeric_limits<double>::min();
}

void BaseWidget::plotGraphData(QCustomPlot *customPlot,
                               QCPGraph *graph,
                               const double &value,
                               double &startValue,
                               double &diff)
{
    if (!graph)
        return;
    // plot data
    uint cDateTime = QDateTime::currentDateTime().toTime_t();
    graph->addData(cDateTime, value);

    // set axis range
    double lastV = 0;

#if (QCUSTOMPLOT_VERSION >= QCUSTOMPLOT_VERSION_CHECK(2,0,0))
    for (int i = 0; i < graph->dataCount(); i++) {
        double key = graph->dataMainKey(i);
        double value = graph->dataMainValue(i);
        if (startValue == -1)
            startValue = value;
        if (key < _xMin)
            _xMin = key;
        if (key > _xMax)
            _xMax = key;
        if (value < _valueMin)
            _valueMin = value;
        if (value > _valueMax)
            _valueMax = value;

        lastV = value;
    }
    if (graph->dataCount() > 1) {
        double diffKey = graph->dataMainKey(graph->dataCount() - 1) - graph->dataMainKey(0);
        if (diffKey >= _maxDataTimeSec) {
            _xMin = graph->dataMainKey(1);
            graph->data()->remove(graph->dataMainKey(0));
        }
    }
#else
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
#endif

    diff = lastV - startValue;

    customPlot->xAxis->setRange(_xMin, _xMax + 5);
    customPlot->xAxis->setPadding(5); // a bit more space to the left border
    int vDiff = (_valueMax - _valueMin) / 2;
    if (vDiff < 5)
        vDiff = 5;
    customPlot->yAxis->setRange(_valueMin, _valueMax + vDiff);
    customPlot->yAxis->setPadding(5);
    customPlot->replot();
}

void BaseWidget::clearGraphData(QCPGraph *graph)
{
    if (!graph)
        return;
#if (QCUSTOMPLOT_VERSION >= QCUSTOMPLOT_VERSION_CHECK(2,0,0))
    graph->data()->clear();
#else
    graph->clearData();
#endif
}

QColor BaseWidget::randomColor() const
{
    return QColor(rand()%245 + 10, rand()%245 + 10, rand()%245 + 10);
}

double BaseWidget::roundDouble(const double &value) const
{
    return (static_cast<double>(qRound(value * 100)) / 100.0);
}

double BaseWidget::roundDoubleAbs(const double &value) const
{
    return qAbs(this->roundDouble(value));
}
