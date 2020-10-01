#ifndef BASETABWIDGET_H
#define BASETABWIDGET_H

#include <QString>
#include <QList>

#include "BaseWidget.h"

class BaseTabWidget
{
public:
    BaseTabWidget() {}
    virtual ~BaseTabWidget() {}

    virtual QString tabTitle() const = 0;
    virtual QList<BaseWidget *> widgetsList() const = 0;
};

#endif // BASETABWIDGET_H
