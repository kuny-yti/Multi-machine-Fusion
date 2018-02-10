#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H
#include <QTcpSocket>
#include "tcp_interfaces.h"

class client_tcp: public QTcpSocket
{
    Q_OBJECT
private:
    QByteArray blockByte;
    qint64 blockNumber;
    QString ip_addres ;
signals:
    void recv(QTcpSocket *ts, const QByteArray &);

    void error(const QString &addr, const QString &err);//错误
    void disconnected(const QString& ip);//断开
    void connected(QTcpSocket *ts,const QString& ip);//连接

private slots:
    void _client_read();//socket接受
    void socketError(QAbstractSocket::SocketError);//错误
    void socketDisconnected();//socket断开
    void socketConnected();//socket连接

protected:
    void process(QByteArray &array);//处理数据

public:
    explicit client_tcp(QObject *parent = 0);
    ~client_tcp();
    int connection(const QString &addr, const int rt);

    int send(const QByteArray& data);
};

#endif // CLIENT_TCP_H
