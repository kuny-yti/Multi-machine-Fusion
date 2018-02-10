#include "server_pro.h"
#include "s_tcp_implement.h"
#include "command.h"
#include "transcoding.h"
#include <QTcpSocket>
#include <QTime>
#include <QHostInfo>

server_pro *server_pro::_this = NULL;

server_pro::server_pro(configure *conf)
{
    server = new s_tcp_implement;
    command_p = new command(conf);
    connect(command_p, &command::send_data,
            this, &server_pro::on_send_data);
    connect(command_p, &command::clock_run,
            this, &server_pro::clock_run);

    _this = this;

    server->recv(&server_pro::recv);
    server->error(&server_pro::error);
    server->disconnected(&server_pro::disconnected);
    server->connected(&server_pro::connected);
}

server_pro *server_pro::object()
{
    return _this;
}

void server_pro::_recv(QTcpSocket *ts, const QByteArray &datas)
{
    command_p->on_network_recv(ts, datas);
}
void server_pro::_error(const QString &addr, const QString &err)
{
    log_warning(tr("Client error[%1],Reason:%2! [time:%3]").arg(addr).arg(err)
            .arg(QTime::currentTime().toString("hh:mm:ss")));
}
void server_pro::_disconnected(const QString &ip)
{
    log_warning(tr("Client:[%1]Off the assembly line. [time:%2]")
                          .arg(ip).arg(QTime::currentTime().toString("hh:mm:ss")));
    socket_MAP.remove(ip);
    emit s_disconnect(ip);
}
void server_pro::_connected(QThread *th,const QString& ip)
{
    log_warning(tr("Client:[%1]Go online. [time:%2]")
                         .arg(ip).arg(QTime::currentTime().toString("hh:mm:ss")));
    socket_MAP.insert(ip, th);
    emit s_connect(th, ip);
}

bool server_pro::start(quint16 port, const QHostAddress &address)
{
    on_command_exec(Clock_Sync, 0xff10, 0.0);
    command_p->server_addres(QHostInfo::fromName(QHostInfo::localHostName()).
                             addresses().first().toString());

    return server->start(port, address);
}
void server_pro::stop()
{
    on_command_exec(Clock_Sync, 0xff11, 0.0);
    server->stop();
}
void server_pro::disc_client(const QString& ip)
{
    server->disc_client(ip);
}
void server_pro::disc_client(const QStringList& ip)
{
    for (int i= 0; i < ip.size(); i++)
    {
        disc_client(ip[i]);
    }
}
int server_pro::send(const QString& ip, const QByteArray& data)
{
    return server->send(ip, data);
}
void server_pro::listen_max(const int num)
{
    _listen_max = num;
    server->listen_max(num);
}
void server_pro::on_command(const QStringList &addres,
                const QString &cmd)
{
    command_p->on_command(addres, cmd);
}
void server_pro::on_send_data( const QString& ip, const QByteArray& data)
{
     send(ip, data);
}
void server_pro::on_command_exec(uint type, uint cmd, const float val)
{
    command_p->on_command_exec(type, cmd, val);
}
void server_pro::recv(QTcpSocket *ts, const QByteArray &ba)
{
    _this->_recv(ts, ba);
}
void server_pro::error(const QString &ad, const QString &err)
{
    _this->_error(ad, err);
}
void server_pro::disconnected(const QString &ad)
{
    _this->_disconnected(ad);
}
void server_pro::connected(QThread *th,const QString &ad)
{
    _this->_connected(th, ad);
}
