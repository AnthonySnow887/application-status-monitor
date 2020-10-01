#ifndef TABHOSTSTATISTICS_H
#define TABHOSTSTATISTICS_H

#include <QWidget>

#include "BaseTabWidget.h"

namespace Ui {
    class TabHostStatistics;
}

class TabHostStatistics : public QWidget, public BaseTabWidget
{
    Q_OBJECT

public:
    explicit TabHostStatistics(QWidget *parent = nullptr);
    ~TabHostStatistics();

    QString tabTitle() const override;
    QList<BaseWidget *> widgetsList() const override;

private:
    Ui::TabHostStatistics *ui;

    QList<BaseWidget *> _widgets;
};

#endif // TABHOSTSTATISTICS_H
