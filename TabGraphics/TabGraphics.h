#ifndef TABGRAPHICS_H
#define TABGRAPHICS_H

#include <QWidget>

#include "BaseTabWidget.h"

namespace Ui {
    class TabGraphics;
}

class TabGraphics : public QWidget, public BaseTabWidget
{
    Q_OBJECT

public:
    explicit TabGraphics(QWidget *parent = nullptr);
    ~TabGraphics();

    QString tabTitle() const override;
    QList<BaseWidget *> widgetsList() const override;

private:
    Ui::TabGraphics *ui;

    QList<BaseWidget *> _widgets;
};

#endif // TABGRAPHICS_H
