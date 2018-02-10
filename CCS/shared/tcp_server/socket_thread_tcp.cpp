#include "socket_thread_tcp.h"
#include <QDataStream>
#include <QHostAddress>

socket_thread_tcp::socket_thread_tcp(int Handle, QObject *parent):
    QThread(parent), _socket_tcp(parent)
{
    _socket_tcp.setSocketDescriptor(Handle);
    connect(&_socket_tcp, SIGNAL(readyRead()), this, SLOT(_client_read()));
    connect(&_socket_tcp, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&_socket_tcp, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(&_socket_tcp, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
}
void socket_thread_tcp::run()
{
    ip_addres = _socket_tcp.peerAddress().toString();
    emit connected(this, ip_addres);

    this->exec();
}

void socket_thread_tcp::_client_read()
{
    while (_socket_tcp.bytesAvailable() >= sizeof(qint64))
    {
        block_byte= _socket_tcp.readAll();

        this->process(block_byte);
    }
}

void socket_thread_tcp::process( QByteArray& array)
{
    emit recv(&_socket_tcp, array);
}
void socket_thread_tcp::socketDisconnected()
{
    _socket_tcp.close();
    this->wait(1000);
    this->quit();
    emit disconnected(ip_addres);
}
socket_thread_tcp::~socket_thread_tcp()
{
    _socket_tcp.close();
    this->wait(1000);
    this->quit();
}
void socket_thread_tcp::socketError(QAbstractSocket::SocketError )
{
    emit error(ip_addres, _socket_tcp.errorString ());
}
void socket_thread_tcp::socketConnected()
{
    emit connected(this, ip_addres);
}

int  socket_thread_tcp::send(const QByteArray& data)
{
    int ret;

    ret = _socket_tcp.write(data);
    return ret;
}
