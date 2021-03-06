#ifndef DRIVEHOSTWIDGET_H
#define DRIVEHOSTWIDGET_H

#include <QWidget>
#include <QMap>

#include "BaseWidget.h"

namespace Ui {
    class DriveHostWidget;
}

class DriveHostWidget : public QWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit DriveHostWidget(QWidget *parent = nullptr);
    ~DriveHostWidget();

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::DriveHostWidget *ui;

    QCustomPlot *_customPlot;
    QMap<QString /*mount point*/, QCPGraph *> _graphMap;

    QMap<QString, QByteArray> _resultLst;
    QMap<QString /*mount point*/, double> _valueStartMap;

    bool appendGraph(const QString &mountPoint, const QString &name);
    void removeAllGraphs();
    void clearAllGraphs();

    void resizeLabel();

protected:
    void resizeEvent(QResizeEvent *e) override;
};

#endif // DRIVEHOSTWIDGET_H
