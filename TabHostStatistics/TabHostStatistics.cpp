#include "TabHostStatistics.h"
#include "ui_TabHostStatistics.h"

#include "CpuHostWidget.h"
#include "MemHostWidget.h"
#include "NetworkHostWidget.h"
#include "DriveHostWidget.h"

#define MAX_W_COLUMNS 2

TabHostStatistics::TabHostStatistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabHostStatistics)
{
    ui->setupUi(this);

    // -- add widgets --
    _widgets.append(new CpuHostWidget(this));
    _widgets.append(new MemHostWidget(this));
    _widgets.append(new NetworkHostWidget(this));
    _widgets.append(new DriveHostWidget(this));

    // -- add in layout
    int row = 0;
    int column = 0;
    for (BaseWidget *w : _widgets) {
        ui->gridLayoutGraph->addWidget(dynamic_cast<QWidget*>(w), row, column++);
        if (column == MAX_W_COLUMNS) {
            column = 0;
            row++;
        }
    }
}

TabHostStatistics::~TabHostStatistics()
{
    delete ui;
}

QString TabHostStatistics::tabTitle() const
{
    return trUtf8("Host Statistics");
}

QList<BaseWidget *> TabHostStatistics::widgetsList() const
{
    return _widgets;
}
