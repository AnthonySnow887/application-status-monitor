#ifndef NETWORKHOSTWIDGET_H
#define NETWORKHOSTWIDGET_H

#include <QWidget>
#include <QMap>

#include "BaseWidget.h"

namespace Ui {
    class NetworkHostWidget;
}

class NetworkHostWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit NetworkHostWidget(QWidget *parent = nullptr);
    ~NetworkHostWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::NetworkHostWidget *ui;

    QCustomPlot *_customPlot;
    QCPGraph *_graphIn;
    QCPGraph *_graphOut;

    QMap<QString, QByteArray> _resultLst;

    double _valueStartIn;
    double _valueStartOut;
};

#endif // NETWORKHOSTWIDGET_H
