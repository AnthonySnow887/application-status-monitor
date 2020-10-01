#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTimer>

#include "BaseTabWidget.h"
#include "CmdInfo.h"

#include "QtSsh/Ssh.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Ssh *_ssh;
    bool _sshConnected;
    QTimer _timer;

    uint _startDTime;

    QList<BaseTabWidget *> _widgets;
    QList<CmdInfo> _cmdList;

    QList<BaseWidget *> allWidgets() const;
    QString workingTimeToStr(uint sec) const;

    void appendCmd(const CmdInfo &cmd);
    CmdInfo searchCmd(const CmdInfo &cmd, int &index) const;
    CmdInfo takeFirstCmd() const;
    void removeFirstCmd();
    bool isCmdListEmpty() const;

    void selectLocalData();
    void selectSshData();

public slots:
    void onConnect();
    void onDisconnect();

    void onStart();
    void onStop();

    void onSelectData();

    void onSshConnected(bool isSuccess);
    void onSshDisconnected();
    void onSshWaitAnswer(const QString question);
    void onSshAutenticate(bool isSuccess);
    void onSshResult(const QString commandResult);
    void onSshSendFailed();
};
#endif // MAINWINDOW_H
