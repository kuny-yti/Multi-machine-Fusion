#ifndef CLIENT_PRO_H
#define CLIENT_PRO_H

#include "tcp_interfaces.h"
#include <QObject>
#include "configure.h"
#include "message.h"

class command;
class QTimer;

class client_pro :public QObject
{
    Q_OBJECT
private:
    configure config;
    c_tcp_interfaces *tcp_client;
    command *command_exec;
    tcp_comm_interfaces *comm_send;
    QTimer *time;

    QStringList Player_Command;
    static client_pro *_this;

private:
    void _error(const QString &, const QString &);
    void _connected(QTcpSocket *,const QString&);
    void _disconnected(const QString&);
    void _recv(QTcpSocket *, const QByteArray &);

    void _read_command_list();
    void _save_command_list();
private slots:
    void on_time_client();

signals:
    void s_player_command(const uint &cmd,const QByteArray &datas);
public:
    client_pro();

    static client_pro *object();
    void command_save(const QStringList &Player);
    void command_read(QStringList &Player);
    configure *conf_file(){return &config;}
    void init();
public:
    static void error(const QString &, const QString &);
    static void connected(QTcpSocket *,const QString&);
    static void disconnected(const QString&);
    static void recv(QTcpSocket *, const QByteArray &);
};

#endif // SERVER_PRO_H
