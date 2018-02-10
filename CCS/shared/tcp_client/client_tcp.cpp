#include "client_tcp.h"
#include <QDataStream>

client_tcp::client_tcp(QObject *parent):
    QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(_client_read()));
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
}

int client_tcp::connection(const QString &addr, const int rt)
{
    ip_addres = addr;
    this->connectToHost(addr, rt);
    return this->waitForConnected(1000);
}
void client_tcp::socketDisconnected()
{
    this->close();
    emit disconnected(ip_addres);
}
client_tcp::~client_tcp()
{
    this->close();
}
void client_tcp::socketError(QAbstractSocket::SocketError )
{
    emit error(ip_addres, this->errorString ());
}
void client_tcp::socketConnected()
{
    emit connected(this, ip_addres);
}

void client_tcp::_client_read()
{
    while (this->bytesAvailable() >= sizeof(qint64))
    {
        blockByte = this->readAll();
        this->process(blockByte);
    }
}
void client_tcp::process( QByteArray& array)
{
    emit recv(this, array);
}

int  client_tcp::send(const QByteArray& data)
{
    int ret;
    ret = this->write(data);
    return ret;
}
