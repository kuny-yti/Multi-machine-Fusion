#ifndef SERVER_PRO_H
#define SERVER_PRO_H

#include "tcp_interfaces.h"
#include "time_server.h"
#include <QObject>

#include "message.h"

class command;
class configure;

class server_pro :public QObject
{
    Q_OBJECT
private:
    QMap <QString , QThread*> socket_MAP;
    int _listen_max;//服务器最大压力

    s_tcp_interfaces *server;
    command     *command_p;
    static server_pro *_this;

    void _recv(QTcpSocket *, const QByteArray &);
    void _error(const QString &, const QString &);
    void _disconnected(const QString&);
    void _connected(QThread *,const QString&);

signals:
    void s_disconnect(const QString&);
    void s_connect(QThread *,const QString&);
    void clock_run(const float );
public slots:
    void on_command(const QStringList &addres,
                    const QString &cmd);
    void on_send_data(const QString& ip, const QByteArray& data);
    void on_command_exec(uint type, uint cmd, const float val);
public:
    server_pro(configure *);
    static server_pro *object();

    bool start(quint16 port, const QHostAddress &address = QHostAddress::Any) ;
    void stop();
    void disc_client(const QString& ip);
    void disc_client(const QStringList& ip);
    int send(const QString& ip, const QByteArray& data);
    void listen_max(const int num);

public:
    static void recv(QTcpSocket *, const QByteArray &);
    static void error(const QString &, const QString &);
    static void disconnected(const QString&);
    static void connected(QThread *,const QString&);
};

#endif // SERVER_PRO_H
