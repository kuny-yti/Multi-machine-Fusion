#ifndef TCP_IMPLEMENT_H
#define TCP_IMPLEMENT_H

#include "tcp_interfaces.h"
#include "tcp_comm_implement.h"
#include <QObject>

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
const int max_data_show_size = 1024*1024*10;
class server_tcp;

class s_tcp_implement_p : public QObject
{
    Q_OBJECT
private:
    server_tcp *server;

public:
    recv_server _recv;
    error_server _error;
    disconnected_server _disconnected;
    connected_server _connected;

    QHostAddress addres;
public:
    explicit s_tcp_implement_p(QObject *parent = 0):
        QObject(parent),
        server(0),
        _recv(0),
        _error(0),
        _disconnected(0),
        _connected(0)
    {

    }
    bool start(quint16 port, const QHostAddress &address = QHostAddress::Any) ;
    void stop();
    void disc_client(const QString& ip);
    int send(const QString& ip, const QByteArray& data);
    void listen_max(const int num);
private slots:
    void on_recv(QTcpSocket *ts, const QByteArray &);
    void on_error(const QString &addr, const QString &err);
    void on_disconnected(const QString& ip);
    void on_connected(QThread *ts, const QString& ip);
};

/// tcp 服务器实现
class s_tcp_implement :public s_tcp_interfaces
{
private:
    s_tcp_implement_p *d_ptr;

public:
    s_tcp_implement();

    virtual ~s_tcp_implement() ;

    virtual bool start(quint16 port, const QHostAddress &address = QHostAddress::Any) ;
    virtual void stop();
    virtual void disc_client(const QString& ip);
    virtual int send(const QString& ip, const QByteArray& data);
    virtual void listen_max(const int num);

    virtual void recv(recv_server);
    virtual void error(error_server);
    virtual void disconnected(disconnected_server);
    virtual void connected(connected_server);

    virtual QString source();
    virtual QString server();
};


#endif // TCP_IMPLEMENT_H
