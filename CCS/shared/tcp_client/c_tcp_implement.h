#ifndef C_TCP_IMPLEMENT_H
#define C_TCP_IMPLEMENT_H

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
class client_tcp;
class c_tcp_implement_p : public QObject
{
    Q_OBJECT
private:
    client_tcp *server;

public:
    recv_client _recv;
    error_client _error;
    disconnected_client _disconnected;
    connected_client _connected;

public:
    explicit c_tcp_implement_p(QObject *parent = 0);
    bool connect(quint16 port, const QString &addr) ;
    void stop();
    int send(const QByteArray& data);
private slots:
    void on_recv(QTcpSocket *ts, const QByteArray &);
    void on_error(const QString &addr, const QString &err);
    void on_disconnected(const QString& ip);
    void on_connected(QTcpSocket *ts,const QString& ip);
};

/// tcp 客户端实现
class c_tcp_implement :public c_tcp_interfaces
{
private:
    c_tcp_implement_p *d_ptr;

public:
    c_tcp_implement();

    virtual ~c_tcp_implement() ;

    virtual bool connect(quint16 port, const QString &address) ;
    virtual int send(const QByteArray& data);

    virtual void recv(recv_client);
    virtual void error(error_client);
    virtual void disconnected(disconnected_client);
    virtual void connected(connected_client);

};

#endif // C_TCP_IMPLEMENT_H
