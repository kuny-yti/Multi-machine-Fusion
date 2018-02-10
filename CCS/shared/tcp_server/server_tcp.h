#ifndef SERVER_TCP_H
#define SERVER_TCP_H
#include <QTcpServer>
#include <QMap>
#include "tcp_interfaces.h"

#ifdef _MESSAFE_LOGS_FILE_
#include "message.h"
#    define log_object(val) message::print_object(val)
#    define log_warning(val) message::warning(val)
#    define log_about(val) message::about(val)
#    define log_error(val) message::error(val)
#    define log_information(val) message::information(val)
#else
#    define log_object(val)
#    define log_warning(val)
#    define log_about(val)
#    define log_error(val)
#    define log_information(val)
#endif

class server_cmd_process;
class server_tcp: public QTcpServer
{
    Q_OBJECT
private:
    QMap <QString, QThread*> socket_MAP;
    int _listen_max;
    bool _recv_type;
private:
    void incomingConnection(int handle);

private slots:
    void on_recv(QTcpSocket *ts, const QByteArray &);

    void on_error(const QString &addr, const QString &err);//错误
    void on_disconnected(const QString& ip);//断开
    void on_connected(QThread *ts,const QString& ip);//连接
signals:
    void recv(QTcpSocket *ts, const QByteArray &);
    void error(const QString &addr, const QString &err);//错误
    void disconnected(const QString& ip);//断开
    void connected(QThread *ts,const QString& ip);//连接

public :
    bool start(quint16 port, const QHostAddress &address = QHostAddress::Any);
    void stop();
    void disc_client(const QString& ip);
    int send(const QString& ip, const QByteArray& data);
    void listen_max(const int num);
public:
    explicit server_tcp(QObject *parent = 0);

};

#endif // SERVER_TCP_H
