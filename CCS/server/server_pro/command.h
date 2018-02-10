#ifndef COMMAND_H
#define COMMAND_H
#include <QObject>
#include "time_server.h"
#include "tcp_interfaces.h"

class configure;
class tcp_comm_interfaces;
class command_execution;
class QTcpSocket;

class command:public QObject
{
    Q_OBJECT
    configure *conf;
    QString server_ip;
    time_server *times_server;
    int node_count;
    tcp_comm_interfaces *comm;
    command_execution *command_exec;
public:
    command(configure *);
    void on_command(const QStringList &addres,
                    const QString &cmd);
    void server_addres(const QString &ip) {server_ip = ip;}
    void on_command_exec(uint type, uint cmd, const float val);
signals:
    void send_data(const QString &ip, const QByteArray &);
    void clock_run(const float );
public slots:
    void on_network_recv(QTcpSocket *ts, const QByteArray &);//网络数据处理

    void on_clock_update(const quint32, const QByteArray &byte);

private:
    void _comm_send();//接收命令转发
    void _command_exec();//接收命令执行
    void _comm_show(QTcpSocket *ts);//接收命令显示
    void _comm_type(QTcpSocket *);//接收命令处理
    void _comm_return(QTcpSocket *ts);
    command_enum _string_command(const QString &cmd);
    void command_send(QTcpSocket *ts, const QStringList &addres,
                    const QString &cmd);
};

#endif // COMMAND_H
