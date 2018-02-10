#include "s_tcp_implement.h"
#include "server_tcp.h"
#include <QTime>
#include <QTcpSocket>
#include <QThread>

s_tcp_implement::s_tcp_implement()
{
    d_ptr = new s_tcp_implement_p();
}

s_tcp_implement::~s_tcp_implement()
{
    delete d_ptr;
}

bool s_tcp_implement::start(quint16 port, const QHostAddress &address )
{
    return d_ptr->start (port, address);
}

void s_tcp_implement::stop()
{
    d_ptr->stop ();
}

void s_tcp_implement::disc_client(const QString& ip)
{
    d_ptr->disc_client (ip);
}

int s_tcp_implement::send(const QString& ip, const QByteArray& data)
{
    return d_ptr->send (ip, data);
}

void s_tcp_implement::listen_max(const int num)
{
    d_ptr->listen_max (num);
}

void s_tcp_implement::recv(recv_server fun)
{
    d_ptr->_recv = fun;
}

void s_tcp_implement::error(error_server fun)
{
    d_ptr->_error = fun;
}

void s_tcp_implement::disconnected(disconnected_server fun)
{
    d_ptr->_disconnected = fun;
}

void s_tcp_implement::connected(connected_server fun)
{
    d_ptr->_connected = fun;
}

bool s_tcp_implement_p::start(quint16 port, const QHostAddress &address)
{
    if (!server)
    {
        server = new server_tcp(this);
        this->connect(server, &server_tcp::recv, this,
                      &s_tcp_implement_p::on_recv);
        this->connect(server, &server_tcp::error, this,
                      &s_tcp_implement_p::on_error);
        this->connect(server, &server_tcp::disconnected, this,
                      &s_tcp_implement_p::on_disconnected);
        this->connect(server, &server_tcp::connected, this,
                      &s_tcp_implement_p::on_connected);

        addres = address;
        bool ret = server->start(port, address);
        if (ret)
        {
            log_warning(tr("start success. [time:%1]")
                      .arg(QTime::currentTime().toString("hh:mm:ss")));
            return true;
        }
        else
        {
            log_error(tr("start failure! [time:%1]")
                      .arg(QTime::currentTime().toString("hh:mm:ss")));
            return false;
        }
    }
    log_warning(tr("server already start. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
    return true;
}

void s_tcp_implement_p::stop()
{
    if (server)
    {
        server->stop ();
        delete server;
        server = 0;
        log_warning(tr("stop success. [time:%1]")
                  .arg(QTime::currentTime().toString("hh:mm:ss")));
        return ;
    }
    log_warning(tr("server not start. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void s_tcp_implement_p::disc_client(const QString& ip)
{
    server->disc_client(ip);
}

int s_tcp_implement_p::send(const QString& ip, const QByteArray& data)
{
    if (server)
    {
        return server->send(ip, data);
    }
    return -1;
}

void s_tcp_implement_p::listen_max(const int num)
{
    if (server)
    {
        log_warning(tr("server listen max(%1). [time:%2]")
                  .arg (num)
                  .arg(QTime::currentTime().toString("hh:mm:ss")));
        server->listen_max (num);
    }
}

void s_tcp_implement_p::on_recv(QTcpSocket *ts, const QByteArray &da)
{
    if (_recv)
    {
        _recv(ts, da);
        return;
    }

    log_warning(tr("server recv func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void s_tcp_implement_p::on_error(const QString &addr, const QString &err)
{
    if (_error)
    {
        _error(addr, err);
        return;
    }
    log_warning(tr("server error func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void s_tcp_implement_p::on_disconnected(const QString& ip)
{
    if (_disconnected)
    {
        _disconnected(ip);
        return;
    }
    log_warning(tr("server disconnected func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void s_tcp_implement_p::on_connected(QThread *ts,const QString& ip)
{
    if (_connected)
    {
        _connected(ts, ip);
        return;
    }
    log_warning(tr("server connected func null. [time:%1]")
              .arg(QTime::currentTime().toString("hh:mm:ss")));
}
QString s_tcp_implement::source()
{
    return d_ptr->addres.toString();
}
QString s_tcp_implement::server()
{
    return d_ptr->addres.toString();
}
