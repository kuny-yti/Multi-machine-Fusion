#ifndef SOCKET_THREAD_TCP_H
#define SOCKET_THREAD_TCP_H
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include "tcp_interfaces.h"

class socket_thread_tcp: public QThread
{
    Q_OBJECT
private:
    QByteArray block_byte;

    QString ip_addres;
    QTcpSocket _socket_tcp;
signals:
    void recv(QTcpSocket *ts, const QByteArray&);

    void error(const QString &addr,const QString &err);
    void disconnected(const QString& ip);
    void connected(QThread *ts,const QString& ip);

protected:
    void process(QByteArray &array);//处理数据
private slots:
    void _client_read();
    void socketError(QAbstractSocket::SocketError);
    void socketDisconnected();
    void socketConnected();
public:
    explicit socket_thread_tcp(int Handle,QObject *parent = 0);
    ~socket_thread_tcp();
    int send(const QByteArray& data);
protected:
    void run();
};

#endif // SOCKET_THREAD_TCP_H
