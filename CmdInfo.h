#ifndef CMDINFO_H
#define CMDINFO_H

#include <QString>
#include <QList>

#include "BaseWidget.h"

class CmdInfo
{
public:
    CmdInfo()
        : _cmd(QString())
    {}

    CmdInfo(const QString &cmd, BaseWidget *w)
        : _cmd(cmd.trimmed())
        , _wList(QList<BaseWidget *>() << w)
    {}

    virtual ~CmdInfo() {
        _wList.clear();
    }

    CmdInfo(const CmdInfo &obj) {
        _cmd = obj.cmd();
        _wList = obj.widgetsList();
    }


    CmdInfo& operator=(const CmdInfo &obj) {
        if (this == &obj)
            return *this;

        _cmd = obj.cmd();
        _wList = obj.widgetsList();
        return *this;
    }

    QString cmd() const { return _cmd; }
    QList<BaseWidget *> widgetsList() const { return _wList; }

    void appendWidget(BaseWidget *w) {
        if (w && !_wList.contains(w))
            _wList.append(w);
    }

    void appendWidgets(QList<BaseWidget *> widgets) {
        for (BaseWidget *w : widgets)
            this->appendWidget(w);
    }

    bool isValid() const {
        return !(_cmd.isEmpty() || _wList.isEmpty());
    }

    static QString cmdEscape(const QString &cmd) {
        QString tmpCmd;
        for (int i = 0; i < cmd.size(); i++) {
            QChar ch = cmd.at(i);
            if (ch == '\'')
                tmpCmd.append("\\'");
            else
                tmpCmd.append(ch);
        }
        return tmpCmd;
    }

private:
    QString _cmd;
    QList<BaseWidget *> _wList;
};

#endif // CMDINFO_H
