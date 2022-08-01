#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "TabGraphics/TabGraphics.h"
#include "TabSockets/TabSockets.h"
#include "TabHostStatistics/TabHostStatistics.h"

#include <QProcess>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    this->setWindowTitle(trUtf8("Application Status Monitor"));
#else
    this->setWindowTitle(QString("Application Status Monitor"));
#endif

    ui->lineEditMaxDataTime->setText(QString::number(MAX_DATA_DIFF_TIME_SEC));

    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);

    _sshConnected = false;
    _startDTime = -1;

    connect(ui->pushButtonConnect, SIGNAL(clicked()), this, SLOT(onConnect()));
    connect(ui->pushButtonDisconnect, SIGNAL(clicked()), this, SLOT(onDisconnect()));

    connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(onStop()));

    //--
    _timer.setInterval(1000); // 1 sec
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onSelectData()));


    // ssh
    _ssh = Ssh::sshCreate();
    _ssh->sshSetAddNotKnownHosts(true);
    _ssh->sshSetIgnoreFoundOtherHosts(false);
    connect(_ssh, SIGNAL(sshConnected(bool)), this, SLOT(onSshConnected(bool)));
    connect(_ssh, SIGNAL(sshDisconnected()), this, SLOT(onSshDisconnected()));
    connect(_ssh, SIGNAL(sshWaitAnswer(QString)), this, SLOT(onSshWaitAnswer(QString)));
    connect(_ssh, SIGNAL(sshAutenticate(bool)), this, SLOT(onSshAutenticate(bool)));
    connect(_ssh, SIGNAL(sshResult(QString)), this, SLOT(onSshResult(QString)));
    connect(_ssh, SIGNAL(sshSendFailed()), this, SLOT(onSshSendFailed()));

    // -- add tab widgets --
    _widgets.append(new TabGraphics(this));
    _widgets.append(new TabSockets(this));
    _widgets.append(new TabHostStatistics(this));

    // -- add in main window
    for (BaseTabWidget *w : _widgets) // TODO qAsConst
        ui->tabWidget->addTab(dynamic_cast<QWidget*>(w), w->tabTitle());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _ssh;
}

QList<BaseWidget *> MainWindow::allWidgets() const
{
    QList<BaseWidget *> wList;
    for (BaseTabWidget *w : _widgets) // TODO qAsConst
        wList.append(w->widgetsList());
    return wList;
}

QString MainWindow::workingTimeToStr(uint sec) const
{
    if (sec == 0)
        return QString();

    int days = sec / (24 * 60 * 60);
    int hours = (sec - (days * 24 * 60 * 60)) / (60 * 60);
    int minutes = (sec - ((hours + days * 24) * 3600)) / 60;
    int seconds = sec - ((hours * 60 + days * 24 * 60 + minutes) * 60);

    QString minutesString = QString::number(minutes);
    QString secondsString = QString::number(seconds);

    if (minutes < 10)
        minutesString = "0" + minutesString;
    if (seconds < 10)
        secondsString = "0" + secondsString;

    return QString("%1days %2h %3m %4sec").arg(days).arg(hours).arg(minutesString).arg(secondsString);
}

void MainWindow::appendCmd(const CmdInfo &cmd)
{
    if (cmd.isValid()) {
        int index = -1;
        CmdInfo tmpCmd = this->searchCmd(cmd, index);
        if (index != -1) {
            tmpCmd.appendWidgets(cmd.widgetsList());
            _cmdList.replace(index, tmpCmd);
        } else {
            _cmdList.append(cmd);
        }
    }
}

CmdInfo MainWindow::searchCmd(const CmdInfo &cmd, int &index) const
{
    index = -1;
    for (int i = 0; i < _cmdList.size(); i++) {
        if (_cmdList[i].cmd() == cmd.cmd()) {
            index = i;
            return _cmdList[i];
        }
    }
    return CmdInfo();
}

CmdInfo MainWindow::takeFirstCmd() const
{
    if (_cmdList.isEmpty())
        return CmdInfo();

    return _cmdList[0];
}

void MainWindow::removeFirstCmd()
{
    if (!_cmdList.isEmpty())
        _cmdList.removeAt(0);
}

bool MainWindow::isCmdListEmpty() const
{
    return _cmdList.isEmpty();
}

void MainWindow::selectLocalData()
{
    CmdInfo cmd = this->takeFirstCmd();
    if (!cmd.isValid())
        return;
    QString cmdStr = cmd.cmd();
    QProcess process;
    process.start("/bin/bash", {"-c" , cmdStr.trimmed()});
    process.waitForFinished();
    QByteArray res = process.readAllStandardOutput();
    const QList<BaseWidget *> wList = cmd.widgetsList();
    for (BaseWidget *w : wList)
        w->processCmdResult(cmdStr, QString::fromUtf8(res.constData()));

    this->removeFirstCmd();

    if (!this->isCmdListEmpty()) {
        this->selectLocalData();
        return;
    }
    _timer.start();
}

void MainWindow::selectSshData()
{
    CmdInfo cmd = this->takeFirstCmd();
    if (!cmd.isValid())
        return;
    _ssh->sshSendCommand(cmd.cmd());
}

void MainWindow::onConnect()
{
    // ssh settings
    if (ui->lineEditLogin->text().trimmed().isEmpty()) {
        qCritical() << "[onConnect] Err (Invalid login [empty]!)";
        return;
    }

    QStringList host_port = ui->lineEditHost->text().trimmed().split(":");
    QString host = host_port[0].trimmed();
    if (host.isEmpty()) {
        qCritical() << "[onConnect] Err (Invalid host [empty]!)";
        return;
    }
    int port = 22;
    if (host_port.size() > 1)
        port = host_port[1].trimmed().toInt();
    if (port <= 0) {
        qWarning() << qPrintable(QString("[onConnect] Warn (Invalid port [%1]! Set to 22)").arg(port));
        port = 22;
    }

    ui->lineEditHost->setEnabled(false);
    ui->lineEditLogin->setEnabled(false);
    ui->lineEditPassword->setEnabled(false);
    ui->comboBoxAuthType->setEnabled(false);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);

    // ssh connect...
    _ssh->sshConnect(host,
                     port,
                     ui->lineEditLogin->text().trimmed());

    qDebug() << "[onConnect] Ok";
}

void MainWindow::onDisconnect()
{
    ui->lineEditHost->setEnabled(true);
    ui->lineEditLogin->setEnabled(true);
    ui->lineEditPassword->setEnabled(true);
    ui->comboBoxAuthType->setEnabled(true);
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);

    // ssh disconnect...
    _ssh->sshDisconnect();
    _sshConnected = false;

    qDebug() << "[onDisconnect] Ok";
}

void MainWindow::onStart()
{
    if (ui->lineEditPid->text().trimmed().toUInt() == 0) {
        qCritical() << "[onStart] Err (Invalid PID!)";
        return;
    }

    ui->groupBoxRemoteHost->setEnabled(false);
    ui->lineEditPid->setEnabled(false);
    ui->checkBoxUseSudo->setEnabled(false);
    ui->lineEditMaxDataTime->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);

    // set max diff time
    const QList<BaseWidget *> wList = this->allWidgets();
    for (BaseWidget *w : wList)
        w->setMaxDataTime(ui->lineEditMaxDataTime->text().trimmed().toUInt());

    _timer.start();
    _startDTime = QDateTime::currentDateTime().toTime_t();

    qDebug() << "[onStart] Ok";
}

void MainWindow::onStop()
{
    ui->groupBoxRemoteHost->setEnabled(true);
    ui->lineEditPid->setEnabled(true);
    ui->checkBoxUseSudo->setEnabled(true);
    ui->lineEditMaxDataTime->setEnabled(true);
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    _timer.stop();

    // clear cmd list
    _cmdList.clear();

    const QList<BaseWidget *> wList = this->allWidgets();
    for (BaseWidget *w : wList)
        w->stop();

    qDebug() << "[onStop] Ok";
}

void MainWindow::onSelectData()
{
    _timer.stop();

    // 0days 0h 0m 0sec
    uint diffTime = QDateTime::currentDateTime().toTime_t() - _startDTime;
    ui->labelWTime->setText(this->workingTimeToStr(diffTime));

    // init cmd list...
    const QList<BaseWidget *> wList = this->allWidgets();
    for (BaseWidget *w : wList) {
        const QStringList cmdList = w->cmdList(ui->lineEditPid->text().trimmed().toUInt());
        if (cmdList.isEmpty())
            continue;
        for (const QString &cmd : cmdList) {
            if (ui->checkBoxUseSudo->isChecked())
                this->appendCmd(CmdInfo(QString("sudo %1").arg(cmd), w));
            else
                this->appendCmd(CmdInfo(cmd, w));
        }
    }

    // select data...
    if (_sshConnected)
        this->selectSshData();
    else
        this->selectLocalData();
}

void MainWindow::onSshConnected(bool isSuccess)
{
    if (!isSuccess) {
        this->onDisconnect();
        return;
    }
    if (ui->comboBoxAuthType->currentText() == "Password") {
        qDebug() << "[onSshConnected] Ok --> sshAuthenticatePassword...";
        _ssh->sshAuthenticatePassword(ui->lineEditPassword->text().trimmed());
    } else if (ui->comboBoxAuthType->currentText() == "Public Key") {
        qDebug() << "[onSshConnected] Ok --> sshAuthenticatePubkey...";
        _ssh->sshAuthenticatePubkey();
    } else if (ui->comboBoxAuthType->currentText() == "Keyboard Interactive") {
        qDebug() << "[onSshConnected] Ok --> sshAuthenticateKbdint...";
        _ssh->sshAuthenticateKbdint();
    }
}

void MainWindow::onSshDisconnected()
{
    qDebug() << "[onSshDisconnected] Ok";
    _sshConnected = false;
}

void MainWindow::onSshWaitAnswer(const QString question)
{
    if (question == "password") {
        _ssh->sshSendAnswer(ui->lineEditPassword->text().trimmed());
        qDebug() << "[onSshWaitAnswer] Send password Ok";
    }
}

void MainWindow::onSshAutenticate(bool isSuccess)
{
    if (!isSuccess) {
        qDebug() << "[onSshAutenticate] SshAutenticate failed!";
        this->onDisconnect();
        return;
    }
    _sshConnected = true;

    qDebug() << "[onSshAutenticate] Ok";
}

void MainWindow::onSshResult(const QString commandResult)
{
    CmdInfo cmd = this->takeFirstCmd();
    if (!cmd.isValid())
        return;
    const QList<BaseWidget *> wList = cmd.widgetsList();
    for (BaseWidget *w : wList)
        w->processCmdResult(cmd.cmd(), commandResult);

    this->removeFirstCmd();

    if (!this->isCmdListEmpty()) {
        this->selectSshData();
        return;
    }
    _timer.start();
}

void MainWindow::onSshSendFailed()
{
    qDebug() << "[onSshSendFailed] Send failed!";

    CmdInfo cmd = this->takeFirstCmd();
    this->removeFirstCmd();

    if (!this->isCmdListEmpty()) {
        this->selectSshData();
        return;
    }
    _timer.start();
}

