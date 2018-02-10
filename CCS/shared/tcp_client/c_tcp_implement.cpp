#include "c_tcp_implement.h"
#include "client_tcp.h"
#include <QTime>

c_tcp_implement::c_tcp_implement()
{
    d_ptr = new c_tcp_implement_p();
}
c_tcp_implement::~c_tcp_implement()
{
    delete d_ptr;
}
bool c_tcp_implement::connect(quint16 port, const QString &address)
{
    return d_ptr->connect (port, address);
}
int c_tcp_implement::send(const QByteArray& data)
{
    return d_ptr->send (data);
}

void c_tcp_implement::recv(recv_client fun)
{
    d_ptr->_recv = fun;
}
void c_tcp_implement::error(error_client fun)
{
    d_ptr->_error = fun;
}
void c_tcp_implement::disconnected(disconnected_client fun)
{
    d_ptr->_disconnected = fun;
}
void c_tcp_implement::connected(connected_client fun)
{
    d_ptr->_connected = fun;
}
c_tcp_implement_p::c_tcp_implement_p(QObject *parent):
    QObject(parent),
    server(0),
    _recv(0),
    _error(0),
    _disconnected(0),
    _connected(0)
{
    server = new client_tcp(this);
    QObject::connect(server, &client_tcp::recv, this,
                  &c_tcp_implement_p::on_recv);
    QObject::connect(server, &client_tcp::error, this,
                  &c_tcp_implement_p::on_error);
    QObject::connect(server, &client_tcp::disconnected, this,
                  &c_tcp_implement_p::on_disconnected);
    QObject::connect(server, &client_tcp::connected, this,
                  &c_tcp_implement_p::on_connected);
}
bool c_tcp_implement_p::connect(quint16 port, const QString &address)
{
    if (server)
    {
        bool ret = server->connection(address, port);
        if (ret)
        {
            log_warning(tr("connection server success. [time:%1]")
                      .arg(QTime::currentTime().toString("hh:mm:ss")));
            return true;
        }
        else
        {
            log_error(tr("connection server failure! [time:%1]")
                      .arg(QTime::currentTime().toString("hh:mm:ss")));
            return false;
        }
    }
    return false;
}

int c_tcp_implement_p::send(const QByteArray& data)
{
    if (server)
    {
        return server->send(data);
    }
    return -1;
}

void c_tcp_implement_p::on_recv(QTcpSocket *ts, const QByteArray &da)
{
    if (_recv)
    {
        _recv(ts, da);
        return;
    }
    log_warning(tr("client recv func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}
void c_tcp_implement_p::on_error(const QString &addr, const QString &err)
{
    if (_error)
    {
        _error(addr, err);
        return;
    }
    log_warning(tr("client error func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}
void c_tcp_implement_p::on_disconnected(const QString& ip)
{
    if (_disconnected)
    {
        _disconnected(ip);
        server->disconnectFromHost();
        server->waitForDisconnected();
        //server->close();
        //delete server;
        //server = 0;
        return;
    }
    log_warning(tr("client disconnected func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}
void c_tcp_implement_p::on_connected(QTcpSocket *ts,const QString& ip)
{
    if (_connected)
    {
        _connected(ts, ip);
        return;
    }
    log_warning(tr("client connected func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}
