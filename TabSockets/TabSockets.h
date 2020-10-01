#ifndef TABSOCKETS_H
#define TABSOCKETS_H

#include <QWidget>
#include <QStandardItemModel>

#include "SocketData.h"
#include "BaseTabWidget.h"
#include "SocketsGraphWidget.h"

namespace Ui {
    class TabSockets;
}

class TabSockets : public QWidget, public BaseTabWidget, public BaseWidget
{
    Q_OBJECT

public:
    explicit TabSockets(QWidget *parent = nullptr);
    ~TabSockets();

    QString tabTitle() const override;
    QList<BaseWidget *> widgetsList() const override;

    QStringList cmdList(const uint &pid) const override;
    void processCmdResult(const QString &cmd, const QString &result) override;
    void stop() override;

private:
    Ui::TabSockets *ui;

    QList<BaseWidget *> _widgets;
    SocketsGraphWidget *_graphWidget;

    QStandardItemModel *_model;

    void processData(const QList<SocketData> &sockets);
};

#endif // TABSOCKETS_H
