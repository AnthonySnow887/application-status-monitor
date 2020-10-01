#include "TabGraphics.h"
#include "ui_TabGraphics.h"

#include "FdGraphWidget.h"
#include "CpuGraphWidget.h"
#include "MemGraphWidget.h"
#include "ThreadsGraphWidget.h"

#define MAX_W_COLUMNS 2

TabGraphics::TabGraphics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabGraphics)
{
    ui->setupUi(this);

    // -- add widgets --
    _widgets.append(new CpuGraphWidget(this));
    _widgets.append(new MemGraphWidget(this));
    _widgets.append(new FdGraphWidget(this));
    _widgets.append(new ThreadsGraphWidget(this));

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

TabGraphics::~TabGraphics()
{
    delete ui;
}

QString TabGraphics::tabTitle() const
{
    return trUtf8("Application Base Statistics");
}

QList<BaseWidget *> TabGraphics::widgetsList() const
{
    return _widgets;
}
