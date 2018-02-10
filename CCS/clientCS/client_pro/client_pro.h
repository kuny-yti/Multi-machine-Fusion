#ifndef CLIENT_PRO_H
#define CLIENT_PRO_H

#include "tcp_interfaces.h"
#include <QObject>
#include "configure.h"

#ifdef _MESSAFE_LOGS_FILE_
#include "message.h"
#    define log_object(val) message::print_object(val)
#    define log_warning(val) message::warning(val)
#    define log_about(val) message::about(val)
#    define log_error(val) message::error(val)
#    define log_information(val) message::information(val)
#else
#    define log_object(val)
#    define log_warning(val)
#    define log_about(val)
#    define log_error(val)
#    define log_information(val)
#endif

class command_execution;
class GUI;
class QTimer;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class client_pro :public QObject
{
    Q_OBJECT
private:
    configure *config;
    c_tcp_interfaces *tcp_client;
    command_execution *command;
    tcp_comm_interfaces *comm;
    tcp_comm_interfaces *comm_send;
    QTimer *time;
    GUI *key_gui;

    QStringList Player_Command;
    QStringList System_Command;
    static client_pro *_this;
private:
    void _error(const QString &, const QString &);
    void _connected(QTcpSocket *,const QString&);
    void _disconnected(const QString&);
    void _recv(QTcpSocket *, const QByteArray &);

    void _transit_type(QTcpSocket *ts);
    void _transit_show(QTcpSocket *ts);
    void _read_command_list();
    void _save_command_list();

    void command_send(const QStringList &addres,const QByteArray &data);
    void ui();
public slots:
    void on_discon_c(const QStringList &addres);
    void on_command(const QStringList &addres, const QString &cmd);
signals:
    void s_addres(const QStringList &addres);
    void s_disconnect();
private slots:
    void on_time_client();

    void on_mouse(QMouseEvent*);
    void on_wheel(QWheelEvent*);
    void on_key(QKeyEvent*);

private:
    void _type_();
public:
    client_pro(configure *file);

    static client_pro *object();
    void command_save(const QStringList &Player, const QStringList &System);
    void command_read(QStringList &Player, QStringList &System);

    void init();
public:
    static void error(const QString &, const QString &);
    static void connected(QTcpSocket *,const QString&);
    static void disconnected(const QString&);
    static void recv(QTcpSocket *, const QByteArray &);
};

#endif // SERVER_PRO_H
