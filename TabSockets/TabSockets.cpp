#include "TabSockets.h"
#include "ui_TabSockets.h"

#include <QTextStream>
#include <QDebug>

TabSockets::TabSockets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSockets)
{
    ui->setupUi(this);
    _widgets.append(this);
    _graphWidget = new SocketsGraphWidget(this);
    ui->verticalLayoutGraph->addWidget(_graphWidget);

    _model = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(_model);
}

TabSockets::~TabSockets()
{
    delete ui;
}

QString TabSockets::tabTitle() const
{
    return trUtf8("Application Opend Network Ports");
}

QList<BaseWidget *> TabSockets::widgetsList() const
{
    return _widgets;
}

QStringList TabSockets::cmdList(const uint &pid) const
{
    QStringList cmd;
    cmd << QString("lsof -Pan -p %1 -i").arg(pid);
    return cmd;
}

void TabSockets::processCmdResult(const QString &cmd, const QString &result)
{
    Q_UNUSED(cmd)
    //    COMMAND     PID   USER   FD   TYPE   DEVICE SIZE/OFF NODE NAME
    //    [APP NAME] [PID] [USER]  18u  IPv4  18279460 0t0      TCP [IP]:[PORT]->[IP]:[PORT] (ESTABLISHED)
    //    [APP NAME] [PID] [USER]  18u  IPv4  18279460 0t0      UDP [IP]:[PORT]

    uint tcp = 0;
    uint udp = 0;
    QList<SocketData> sockets;
    QStringList rows = result.trimmed().split("\n");
    if (rows.size() > 1) {
        QStringList header = rows[0].split(" ");
        header.removeDuplicates();
        header.removeAll(QString(""));
        rows.removeAt(0);
        for (const QString &row : rows) {
            QStringList rowData = row.split(" ");
            rowData.removeDuplicates();
            rowData.removeAll(QString(""));
            int nodeIndex = header.indexOf("NODE");
            int nameIndex = header.indexOf("NAME");
            if (nodeIndex == -1
                || nameIndex == -1
                || nodeIndex >= rowData.size()
                || nameIndex >= rowData.size())
                continue;
            if (rowData[nodeIndex] == "TCP")
                tcp++;
            else if (rowData[nodeIndex] == "UDP")
                udp++;
            else
                continue;

            QString name = rowData[nameIndex];
            if (nameIndex < rowData.size() - 1)
                for (int i = nameIndex + 1; i < rowData.size(); i++)
                    name += QString(" %1").arg(rowData[i]);

            sockets.append(SocketData(SocketData::strToType(rowData[nodeIndex]), name));
        }
    }
    this->processData(sockets);
    _graphWidget->processData(tcp, udp);
}

void TabSockets::stop()
{
    _graphWidget->stop();
}

void TabSockets::processData(const QList<SocketData> &sockets)
{
    if (_model->columnCount() == 0) {
        _model->setHorizontalHeaderLabels(QStringList() << "Type" << "Name");
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->verticalHeader()->hide();
    }

    _model->removeRows(0, _model->rowCount());
    for (const SocketData &socket : sockets) {
        QList<QStandardItem*> row;
        row << new QStandardItem(SocketData::typeToStr(socket.type()));
        row << new QStandardItem(socket.name());
        _model->appendRow(row);
    }
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch); // column "Name"
}
