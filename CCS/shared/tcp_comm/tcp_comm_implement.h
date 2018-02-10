#ifndef TCP_COMMUNICATION_H
#define TCP_COMMUNICATION_H
#include "tcp_interfaces.h"
#include "simple_encrypt.h"
#include "random_number.h"

class tcp_comm_implement :public tcp_comm_interfaces
{
    QByteArray tcp_data;
    tcp_data_struct anal_data;

    void _analyze();
    void _summary();
public:
    tcp_comm_implement();
    virtual ~tcp_comm_implement();

    /// 解析数据
    virtual void analyze(const QByteArray &);
    /// 封包数据
    virtual void summary(tcp_data_struct *dat = NULL);

    virtual void clean_struct();
    virtual void clean_byte();
    /// 内部数据
    virtual QByteArray data_byte();
    virtual tcp_data_struct *data_struct();

    /// 发送者地址
    virtual QString source(const QString &addr = QString()) ;
    /// 目标地址
    virtual QStringList target(const QStringList &addr = QStringList());
    /// 服务器地址
    virtual QString server(const QString &addr = QString());
    /// 发送数据
    virtual QByteArray data(const QByteArray &addr = QByteArray()) ;

    /// 发送数据类型
    virtual command_enum command(const command_enum = Cmd_Null) ;
    /// 发送命令
    virtual protocol_enum protocol(const protocol_enum = Prot_Null);
};

#endif // TCP_COMMUNICATION_H
