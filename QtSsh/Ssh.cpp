#include "Ssh.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>

Ssh::Ssh(QObject *parent)
    : QObject(parent)
{
    _sshSession = 0x00;

    _port = 0;
    _currentIprompt = 0;
    _nprompts = 0;

    _isAddNotKnownHosts = true;
    _isIgnoreFoundOtherHosts = true;
    _isConnected = false;
}

Ssh::~Ssh()
{
    this->sshDisconnect();

    _thread.quit();
    _thread.wait(10 * 1000);
}

Ssh *Ssh::sshCreate()
{
    Ssh *buffObject = new Ssh();
    if(!buffObject->init()) {
        delete buffObject;
        buffObject = 0x00;
    }
    return buffObject;
}

bool Ssh::init()
{
    this->moveToThread(&_thread);
    _thread.start();
    return _thread.isRunning();
}

void Ssh::sshConnect(const QString host, int port, const QString user)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(_isConnected)
        return;

    _host = host;
    _port = port;
    _user = user;

    QTimer::singleShot(0, this, SLOT(onSshConnect()));
}

void Ssh::sshDisconnect()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected)
        return;

    QTimer::singleShot(0, this, SLOT(onSshDisconnect()));
}

void Ssh::sshAuthenticatePassword(const QString password)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected)
        return;

    _password = password;

    QTimer::singleShot(0, this, SLOT(onSshAuthenticatePassword()));
}

void Ssh::sshAuthenticatePubkey()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected)
        return;

    QTimer::singleShot(0, this, SLOT(onSshAuthenticatePubkey()));
}

void Ssh::sshAuthenticateKbdint()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected)
        return;

    QTimer::singleShot(0, this, SLOT(onSshAuthenticateKbdint()));
}

void Ssh::sshSetAddNotKnownHosts(bool isAdd)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    _isAddNotKnownHosts = isAdd;
}

void Ssh::sshSetIgnoreFoundOtherHosts(bool isIgnore)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    _isIgnoreFoundOtherHosts = isIgnore;
}

bool Ssh::isSshConnected()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    return _isConnected;
}

const QString Ssh::sshLastError() const
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    return _sshLastError;
}

int Ssh::verifyKnownhost(sshServerHostType &type, QString &error)
{
    if(!_sshSession)
        return -1;

    type = sshServerNoType;
    error.clear();

    unsigned char *hash = 0x00;
    char *hexa;
    size_t hlen;
    ssh_key srv_pubkey;
    int rc;

    int state = ssh_is_server_known(_sshSession);
    rc = ssh_get_server_publickey(_sshSession, &srv_pubkey);
    if (rc < 0) {
        error = QString("Get server publickey failed!");
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA1,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        error = QString("Get publickey hash failed!");
        return -1;
    }

    switch(state) {
        case SSH_SERVER_KNOWN_OK:
            type = sshServerKnownOk;
            break; /* ok */

        case SSH_SERVER_KNOWN_CHANGED:
            hexa = ssh_get_hexa(hash, hlen);
            type = sshServerKnownChanged;
            error += QString("Host key for server changed: it is now:\n");
            error += QString("%1\n").arg(hexa);
            error += QString("For security reasons, connection will be stopped");

//            qDebug() << "Host key for server changed: it is now:";
//            ssh_print_hexa("Public key hash", hash, hlen);
//            qDebug() << "For security reasons, connection will be stopped";

            free(hexa);
            free(hash);
            return -1;

        case SSH_SERVER_FOUND_OTHER:
            type = sshServerFoundOther;
            if(_isIgnoreFoundOtherHosts) {
                error += QString("The host key for this server was not found but an other type of key exists.\n");
                error += QString("An attacker might change the default server key to confuse your client into thinking the key does not exist.\n");
                free(hash);
                return -1;
            }
            break;

        case SSH_SERVER_FILE_NOT_FOUND: /* Возвращение к ситуации SSH_SERVER_NOT_KNOWN */
            type = sshServerFileNotFound;
            hexa = ssh_get_hexa(hash, hlen);
            error += QString("Could not find known host file.\n");
            error += QString("Public key hash: %1\n").arg(hexa);

            if(!_isAddNotKnownHosts) {
                free(hexa);
                free(hash);
                return -1;
            }

            if(ssh_write_knownhost(_sshSession) < 0) {
                error += QString("SSH write knownhost error!");
                free(hexa);
                free(hash);
                return -1;
            }
            free(hexa);
            break;

        case SSH_SERVER_NOT_KNOWN:
            type = sshServerNotKnown;
            hexa = ssh_get_hexa(hash, hlen);
            error += QString("The server is unknown.\n");
            error += QString("Public key hash: %1\n").arg(hexa);

            if(!_isAddNotKnownHosts) {
                free(hexa);
                free(hash);
                return -1;
            }

            if(ssh_write_knownhost(_sshSession) < 0) {
                error += QString("SSH write knownhost error!");
                free(hexa);
                free(hash);
                return -1;
            }
            free(hexa);
            break;

        case SSH_SERVER_ERROR:
            type = sshServerNotKnown;
            error += QString("%1").arg(ssh_get_error(_sshSession));
            free(hash);
            return -1;
    }
    free(hash);
    return 0;
}

void Ssh::sshSendCommand(QString command)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    _currentCommand = command;
    QTimer::singleShot(0, this, SLOT(onSshSendCommand()));
}

void Ssh::sshSendAnswer(QString answer)
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    _currentAnswer = answer;
    QTimer::singleShot(0, this, SLOT(onSshSendAnswer()));
}

void Ssh::onSshConnect()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(_isConnected) {
        emit this->sshConnected(false);
        return;
    }

    _sshSession = ssh_new();
    if(!_sshSession) {
        _sshLastError = QString("Ssh session is NULL!");
        qCritical() << "[Ssh][onSshConnect]" <<  _sshLastError;

        emit this->sshConnected(false);
        return;
    }

    ssh_options_set(_sshSession, SSH_OPTIONS_HOST, _host.toStdString().c_str());
    ssh_options_set(_sshSession, SSH_OPTIONS_PORT, &_port);
    ssh_options_set(_sshSession, SSH_OPTIONS_USER, _user.toStdString().c_str());

    int rc = ssh_connect(_sshSession);
    if(rc != SSH_OK) {
        _sshLastError = QString("Error connecting to host: %1").arg(ssh_get_error(_sshSession));
        qCritical() << "[Ssh][onSshConnect]" << _sshLastError;
        ssh_disconnect(_sshSession);
        ssh_free(_sshSession);

        emit this->sshConnected(false);
        return;
    }

    sshServerHostType sshType;
    QString sshError;

    if(this->verifyKnownhost(sshType, sshError) < 0) {
        _sshLastError = sshError;
        qCritical() << "[Ssh][onSshConnect]" << _sshLastError;
        ssh_disconnect(_sshSession);
        ssh_free(_sshSession);

        emit this->sshConnected(false);
        return;
    }

    _isConnected = true;
    emit this->sshConnected(true);
}

void Ssh::onSshDisconnect()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected)
        return;

    ssh_disconnect(_sshSession);
    ssh_free(_sshSession);
    _isConnected = false;
    _sshLastError.clear();

    emit this->sshDisconnected();
}

void Ssh::onSshAuthenticatePassword()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected) {
        emit this->sshAutenticate(false);
        return;
    }

    int rc = ssh_userauth_password(_sshSession, NULL, _password.toStdString().c_str());
    if(rc != SSH_AUTH_SUCCESS) {
        _sshLastError = QString("Authentication failed: %1").arg(ssh_get_error(_sshSession));
        qDebug() << "[Ssh][onSshAuthenticatePassword]" << _sshLastError;

        emit this->sshAutenticate(false);
        return;
    }
    emit this->sshAutenticate(true);
}

void Ssh::onSshAuthenticatePubkey()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected) {
        emit this->sshAutenticate(false);
        return;
    }

    int rc = ssh_userauth_autopubkey(_sshSession, NULL);
    if(rc != SSH_AUTH_SUCCESS) {
        _sshLastError = QString("Authentication failed: %1").arg(ssh_get_error(_sshSession));
        qDebug() << "[Ssh][onSshAuthenticatePubkey]" << _sshLastError;

        emit this->sshAutenticate(false);
        return;
    }
    emit this->sshAutenticate(true);
}

void Ssh::onSshAuthenticateKbdint()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected) {
        emit this->sshAutenticate(false);
        return;
    }

    int rc = ssh_userauth_kbdint(_sshSession, NULL, NULL);
    while(rc == SSH_AUTH_INFO) {
        const char *name;
        const char *instruction;

        name = ssh_userauth_kbdint_getname(_sshSession);
        Q_UNUSED(name)
        instruction = ssh_userauth_kbdint_getinstruction(_sshSession);
        Q_UNUSED(instruction)
        _nprompts = ssh_userauth_kbdint_getnprompts(_sshSession);

        for(int iprompt = 0; iprompt < _nprompts; iprompt++) {
            _currentIprompt = iprompt;
            char echo;
            const char *prompt = ssh_userauth_kbdint_getprompt(_sshSession, iprompt, &echo);
            _currentQuestion = QString(prompt);
            if(echo) {
                emit this->sshWaitAnswer(_currentQuestion);
                return;
            } else {
                _currentQuestion = QString("password");
                emit this->sshWaitAnswer(_currentQuestion);
                return;
            }
        }
        rc = ssh_userauth_kbdint(_sshSession, NULL, NULL);
    }

    if(rc != SSH_AUTH_SUCCESS) {
        _sshLastError = QString("Authentication failed: %1").arg(ssh_get_error(_sshSession));
        qDebug() << "[Ssh][onSshAuthenticateKbdint]" << _sshLastError;
        emit this->sshAutenticate(false);
        return;
    }
    emit this->sshAutenticate(true);
}

void Ssh::onSshSendAnswer()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(ssh_userauth_kbdint_setanswer(_sshSession, _currentIprompt, _currentAnswer.toStdString().c_str()) < 0) {
        _sshLastError = QString("Authentication failed! Set answer failed! (key = %1)").arg(_currentQuestion);
        qDebug() << "[Ssh][onSshSendAnswer]" << _sshLastError;

        emit this->sshAutenticate(false);
        return;
    }

    int rc = ssh_userauth_kbdint(_sshSession, NULL, NULL);;
    while(rc == SSH_AUTH_INFO) {
        if((_currentIprompt + 1) == _nprompts) {
            const char *name;
            const char *instruction;

            name = ssh_userauth_kbdint_getname(_sshSession);
            Q_UNUSED(name)
            instruction = ssh_userauth_kbdint_getinstruction(_sshSession);
            Q_UNUSED(instruction)
            _nprompts = ssh_userauth_kbdint_getnprompts(_sshSession);
            _currentIprompt = -1;
        }

        for(int iprompt = _currentIprompt + 1; iprompt < _nprompts; iprompt++) {
            _currentIprompt = iprompt;
            char echo;
            const char *prompt = ssh_userauth_kbdint_getprompt(_sshSession, iprompt, &echo);
            _currentQuestion = QString(prompt);
            if(echo) {
                emit this->sshWaitAnswer(_currentQuestion);
                return;
            } else {
                _currentQuestion = QString("password");
                emit this->sshWaitAnswer(_currentQuestion);
                return;
            }
        }
        rc = ssh_userauth_kbdint(_sshSession, NULL, NULL);
    }

    if(rc != SSH_AUTH_SUCCESS) {
        _sshLastError = QString("Authentication failed: %1").arg(ssh_get_error(_sshSession));
        qDebug() << "[Ssh][onSshSendAnswer]" << _sshLastError;
        emit this->sshAutenticate(false);
        return;
    }
    emit this->sshAutenticate(true);
}

void Ssh::onSshSendCommand()
{
//    QMutexLocker lock(&_mutex);
//    Q_UNUSED(lock)

    if(!_isConnected) {
        emit this->sshSendFailed();
        return;
    }

    char buffer[1024];
    unsigned int nbytes = 0;

    ssh_channel channel = ssh_channel_new(_sshSession);
    if(!channel) {
        _sshLastError = QString("Ssh channel is NULL! Send failed!");
        qCritical() << "[Ssh][onSshSendCommand]" << _sshLastError;

        emit this->sshSendFailed();
        return;
    }

    int rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK) {
        _sshLastError = QString("Open ssh channel session is failed! Send failed!");
        qCritical() << "[Ssh][onSshSendCommand]" << _sshLastError;
        ssh_channel_free(channel);

        emit this->sshSendFailed();
        return;
    }

    rc = ssh_channel_request_exec(channel, _currentCommand.toStdString().c_str());
    if(rc != SSH_OK) {
        _sshLastError = QString("Ssh request exec failed!");
        qCritical() << "[Ssh][onSshSendCommand]" << _sshLastError;
        ssh_channel_close(channel);
        ssh_channel_free(channel);

        emit this->sshSendFailed();
        return;
    }

    QBuffer sshBuffer;
    if(!sshBuffer.open(QBuffer::ReadWrite)) {
        _sshLastError = QString("Open QBuffer failed!");
        qCritical() << "[Ssh][onSshSendCommand]" << _sshLastError;

        ssh_channel_close(channel);
        ssh_channel_free(channel);
        emit this->sshSendFailed();
        return;
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        if (nbytes > sizeof (buffer))
            break;
        if (sshBuffer.write(buffer, nbytes) != nbytes) {
            _sshLastError = QString("Write in QBuffer failed!");
            qCritical() << "[Ssh][onSshSendCommand]" << _sshLastError;

            ssh_channel_close(channel);
            ssh_channel_free(channel);
            emit this->sshSendFailed();
            return;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    sshBuffer.seek(0);
    QString sshResult(sshBuffer.readAll());
    sshBuffer.close();

    emit this->sshResult(sshResult);
}
