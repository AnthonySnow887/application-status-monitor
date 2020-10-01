#ifndef SSH_H
#define SSH_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtCore/QMap>

#include <libssh/libssh.h>

//!
//! \file Ssh.h
//! \class Ssh
//! \brief Класс по работе с SSH
//!
class Ssh : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Перечисление типов серверов
    //!
    enum sshServerHostType {
        sshServerNoType = 0,    //!< не задан
        sshServerKnownOk,       //!< известен
        sshServerKnownChanged,  //!< известен-заменен
        sshServerFoundOther,    //!< найден другой
        sshServerFileNotFound,  //!< файл не найден
        sshServerNotKnown,      //!< не известен
        sshServerError          //!< ошибка
    };
    Q_ENUMS(sshServerHostType)

    //!
    //! \brief Деструктор класса
    //!
    virtual ~Ssh();

    //!
    //! \brief Создать объект класса
    //! \return
    //!
    static Ssh *sshCreate();

    //!
    //! \brief Подключиться к узлу
    //! \param host Адрес
    //! \param port Порт
    //! \param user Логин
    //!
    void sshConnect(const QString host, int port = 22, const QString user = QString());

    //!
    //! \brief Отключиться от узла
    //!
    void sshDisconnect();

    //!
    //! \brief Авторизоваться по паролю
    //! \param password Пароль
    //!
    void sshAuthenticatePassword(const QString password);

    //!
    //! \brief Авторизоваться по публичному ключу
    //!
    void sshAuthenticatePubkey();

    //!
    //! \brief Авторизовать с использованием keyboard-interactive
    //!
    void sshAuthenticateKbdint();

    //!
    //! \brief Отправить команду
    //! \param[in] command Команда
    //! \param[out] result Результат
    //!
    void sshSendCommand(QString command, QString &result);

    //!
    //! \brief Добавлять незнакомые хосты
    //! \param isAdd
    //!
    void sshSetAddNotKnownHosts(bool isAdd);

    //!
    //! \brief Игнорировать найденные хосты, но с другим типом ключа
    //! \param isIgnore
    //!
    void sshSetIgnoreFoundOtherHosts(bool isIgnore);

    //!
    //! \brief Подключен ли объект к узлу
    //! \return
    //!
    bool isSshConnected();

    //!
    //! \brief Последняя ошибка
    //! \return
    //!
    const QString sshLastError() const;

protected:
    //!
    //! \brief Консруктор класса
    //! \param parent Объект-родитель
    //!
    explicit Ssh(QObject *parent = 0);

    //!
    //! \brief Метод инициализации объекта
    //! \return
    //!
    bool init();

    //!
    //! \brief Проверка узлов
    //! \param[in] type Тип узла
    //! \param[out] error Ошибка
    //! \return
    //!
    int verifyKnownhost(sshServerHostType &type, QString &error);

private:
    QThread _thread;                //!< поток выполнения
    mutable QMutex  _mutex;         //!< мьютекс

    ssh_session _sshSession;        //!< объект ssh сессии

    bool _isAddNotKnownHosts;       //!< добавлять незнакомые хосты
    bool _isIgnoreFoundOtherHosts;  //!< игнорировать найденные хосты, но с другим типом ключа
    bool _isConnected;              //!< подключен ли объект к узлу

    QString _host;                  //!< адрес узла
    int     _port;                  //!< порт
    QString _user;                  //!< логин
    QString _password;              //!< пароль

    QString _currentQuestion;       //!< текущий запрос
    QString _currentAnswer;         //!< текущий ответ
    int     _currentIprompt;        //!< текущее значение iprompt (от 0 до N)
    int     _nprompts;              //!< значение ssh_userauth_kbdint_getnprompts

    QString _currentCommand;        //!< текущая команда

    QString _sshLastError;          //!< последняя ошибка ssh

public slots:
    //!
    //! \brief Слот отправки команды
    //! \param command SSH команда
    //!
    void sshSendCommand(QString command);

    //!
    //! \brief Слот отправки ответа
    //! \param answer SSH ответ
    //!
    void sshSendAnswer(QString answer);

protected slots:
    //!
    //! \brief Слот обработки подключения
    //!
    void onSshConnect();

    //!
    //! \brief Слот обработки отключения
    //!
    void onSshDisconnect();

    //!
    //! \brief Слот обработки авторизации по логину и паролю
    //!
    void onSshAuthenticatePassword();

    //!
    //! \brief Слот обработки авторизации по публичному ключу
    //!
    void onSshAuthenticatePubkey();

    //!
    //! \brief Слот обработки авторизации по методу keyboard-interactive
    //!
    void onSshAuthenticateKbdint();

    //!
    //! \brief Слот обработки отправки ответа
    //!
    void onSshSendAnswer();

    //!
    //! \brief Слот обработки отправки команды
    //!
    void onSshSendCommand();

Q_SIGNALS:
    //!
    //! \brief Сигнал, информирующий о состоянии подключения
    //! \param isSuccess Успешно или нет
    //!
    void sshConnected(bool isSuccess);

    //!
    //! \brief Сигнал, информирующий об отключении
    //!
    void sshDisconnected();

    //!
    //! \brief Сигнал, информирующий об ожидании ответа сервером
    //! \param question Вопрос сервера
    //!
    void sshWaitAnswer(const QString question);

    //!
    //! \brief Сигнал, информирующий о результате авторизации
    //! \param isSuccess Успешна или нет
    //!
    void sshAutenticate(bool isSuccess);

    //!
    //! \brief Сигнал, информирующий о результате
    //! \param commandResult Результат
    //!
    void sshResult(const QString commandResult);

    //!
    //! \brief Сигнал, информирующий об ошибке отправки
    //!
    void sshSendFailed();
};

#endif // SSH_H
