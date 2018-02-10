#include "server_tcp.h"
#include "socket_thread_tcp.h"
#include <QTime>

server_tcp::server_tcp(QObject *parent):
    QTcpServer(parent)
{
    _listen_max = 200;
}
bool server_tcp::start(quint16 port, const QHostAddress &address)
{
    socket_MAP.clear();
    bool ret = this->listen(address, port);
    log_warning (tr("start server. [time:%1]")
                 .arg(QTime::currentTime().toString("hh:mm:ss"))) ;
    return ret;
}

void server_tcp::listen_max(const int num)
{
    _listen_max = num;
}
void server_tcp::stop()
{
    QList<QString> keys = socket_MAP.keys();

    for (int i = 0; i < keys.size(); i++)
    {
        socket_thread_tcp *t = (socket_thread_tcp *)socket_MAP.value(keys[i]);
        qDebug() << keys[i] << "disconnected";
        delete t;//线程对象还存在，则不会自动删除，需要手动删除
    }
    log_warning (tr("stop server. [time:%1]")
                 .arg(QTime::currentTime().toString("hh:mm:ss"))) ;
    socket_MAP.clear();
    this->close();
}
void server_tcp::disc_client(const QString& ip)
{
    socket_thread_tcp *t = (socket_thread_tcp *)socket_MAP.value(ip);
    //t->exit ();
    log_warning (tr("disconnected (%1). [time:%2]").arg(ip)
                 .arg(QTime::currentTime().toString("hh:mm:ss"))) ;
    delete t;
}
void server_tcp::incomingConnection(int handle)
{
    if (socket_MAP.size() >= _listen_max) return;

    socket_thread_tcp* thread = new socket_thread_tcp(handle, this);

    this->connect(thread, &socket_thread_tcp::finished, thread,
                  &socket_thread_tcp::deleteLater);//线程执行完成后删除对象

    this->connect(thread, &socket_thread_tcp::recv, this,
                  &server_tcp::recv);
    this->connect(thread, &socket_thread_tcp::error, this,
                  &server_tcp::error);
    this->connect(thread, &socket_thread_tcp::disconnected, this,
                  &server_tcp::disconnected);
    this->connect(thread, &socket_thread_tcp::connected, this,
                  &server_tcp::connected);

    this->connect(thread, &socket_thread_tcp::recv, this,
                  &server_tcp::on_recv);
    this->connect(thread, &socket_thread_tcp::error, this,
                  &server_tcp::on_error);
    this->connect(thread, &socket_thread_tcp::disconnected, this,
                  &server_tcp::on_disconnected);
    this->connect(thread, &socket_thread_tcp::connected, this,
                  &server_tcp::on_connected);

    thread->start();
}
void server_tcp::on_error(const QString &, const QString &)
{
}
void server_tcp::on_disconnected(const QString& ips)
{
    QString ip = ips;
    //socket_thread_tcp *t = (socket_thread_tcp *)socket_MAP.value(ip);
    //delete t;//不用删除对象了，因为已经有信号自动删除对象
    socket_MAP.remove(ip);
}
void server_tcp::on_connected(QThread *ts,const QString& ip)
{
    socket_MAP.insert(ip, ts);
}

void server_tcp::on_recv(QTcpSocket *, const QByteArray &)
{
}

int  server_tcp::send(const QString& ip, const QByteArray& data)
{
    QStringList add = socket_MAP.keys();
    if (ip.isEmpty())
        for (int i = 0; i < add.size(); i++)
        {
                socket_thread_tcp *t = (socket_thread_tcp *)socket_MAP.value(add[i]);
                return t->send(data);
        }
    else
        for (int i = 0; i < add.size(); i++)
        {
            if (add[i] == ip)
            {
                socket_thread_tcp *t = (socket_thread_tcp *)socket_MAP.value(ip);
                return t->send(data);
            }
        }
    return -1;
}
