#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <QtNetwork/QHostAddress>
#include <QString>
#include <QFile>
#include <QStringList>

class QTcpSocket;
//确定数据量
typedef enum
{
    Prot_Null,
    Prot_Heartbeat,     //心跳检测(小)
    Prot_Command,       //命令传输(小)
    Prot_Answer,        //传输后回答(小)

    Prot_Start_Send,//开始传输(小)
    Prot_Data_Send,//开始传输(大)
    Prot_End_Send,//结束传输(小)

}protocol_enum;

const QString tcp_protocol_str[] =
{
    QObject::tr("Null protocol."),
    QObject::tr("Heartbeat detection."),
    QObject::tr("Command."),
    QObject::tr("Answer."),
    QObject::tr("Start sending."),
    QObject::tr("Send data."),
    QObject::tr("Send end.")
};
typedef enum
{
    Cmd_Null = 0x00,
    Cmd_Player_Play ,
    Cmd_Player_Stop,
    Cmd_Player_Pause,
    Cmd_Player_Continue,
    Cmd_Player_Repeat,
    Cmd_Player_Volume,
    Cmd_Player_Mute,
    Cmd_Player_Seek,
    Cmd_Player_Synchro,
    Cmd_Player_Node,
    Cmd_System_Shutdown,
    Cmd_System_Boot,
    Cmd_System_Volume,
    Cmd_System_KeyboardMouse,
    Cmd_System_Restart,
    Cmd_PJlink_Power_On,
    Cmd_PJlink_Power_Off,
    Cmd_Softwrare_Off,
    Cmd_Softwrare_On,
}command_enum;

const QString tcp_command_str[] =
{
    QObject::tr("Null command."),
    QObject::tr("The player starts playing."),
    QObject::tr("The player stops playing."),
    QObject::tr("The player pauses playing."),
    QObject::tr("The players continue playing."),
    QObject::tr("The players reset playing."),
    QObject::tr("The players volume setup."),
    QObject::tr("The players seek setup."),
    QObject::tr("The players synchro setup."),
    QObject::tr("The players control node triggers."),
    QObject::tr("The system shutdown."),
    QObject::tr("The system boot."),
    QObject::tr("The system volume setup."),
    QObject::tr("The system mouse and keyboard mapping."),
    QObject::tr("The system reset boot."),
    QObject::tr("The projector power on."),
    QObject::tr("The projector power off."),
    QObject::tr("The close the computer software."),
    QObject::tr("The start the computer software.")
};

typedef struct
{
    QString src_addres;//发送方 地址
    QString dst_addres;//目的方 地址(有多个地址时用';'隔开)
    QString ser_addres;//服务器 地址
    command_enum command;//命令
    protocol_enum protocol;//协议
    QByteArray data;//发送数据
} tcp_data_struct;

typedef void (*recv_server)(QTcpSocket *, const QByteArray &);
typedef void (*error_server)(const QString &, const QString &);
typedef void (*disconnected_server)(const QString&);
typedef void (*connected_server)(QThread *,const QString&);

typedef void (*recv_client)(QTcpSocket *, const QByteArray &);
typedef void (*error_client)(const QString &, const QString &);
typedef void (*disconnected_client)(const QString&);
typedef void (*connected_client)(QTcpSocket *,const QString&);

class tcp_comm_interfaces
{
public:
    virtual ~tcp_comm_interfaces() {}

    virtual void analyze(const QByteArray &)= 0;
    virtual void summary(tcp_data_struct *dat = NULL)= 0;

    virtual void clean_struct()=0;
    virtual void clean_byte()=0;
    virtual QByteArray data_byte() = 0;
    virtual tcp_data_struct *data_struct()= 0;

    virtual QString source(const QString &addr = QString())= 0;
    virtual QStringList target(const QStringList &addr = QStringList())= 0;
    virtual QString server(const QString &addr = QString())= 0;
    virtual QByteArray data(const QByteArray &addr = QByteArray()) = 0;
    virtual command_enum command(const command_enum = Cmd_Null) = 0;
    virtual protocol_enum protocol(const protocol_enum = Prot_Null)= 0;
};

class s_tcp_interfaces
{
public:
    virtual ~s_tcp_interfaces() {}

    virtual bool start(quint16 port, const QHostAddress &address = QHostAddress::Any) = 0;
    virtual void stop()= 0;
    virtual void disc_client(const QString& ip) = 0;
    virtual int send(const QString& ip, const QByteArray& data)= 0;
    virtual void listen_max(const int num)= 0;

    virtual void recv(recv_server)= 0;
    virtual void error(error_server)= 0;
    virtual void disconnected(disconnected_server)= 0;
    virtual void connected(connected_server)= 0;

    virtual QString source()= 0;
    virtual QString server()= 0;
};
class c_tcp_interfaces
{
public:
    virtual ~c_tcp_interfaces() {}

    virtual bool connect(quint16 port, const QString &address) = 0;
    virtual int send(const QByteArray& data)= 0;

    virtual void recv(recv_client)= 0;
    virtual void error(error_client)= 0;
    virtual void disconnected(disconnected_client)= 0;
    virtual void connected(connected_client)= 0;
};

#endif // TCP_SERVER_H
