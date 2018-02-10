#include "command.h"
#include "configure.h"
#include "s_tcp_implement.h"
#include "command_execution.h"
#include "tcp_comm_implement.h"
#include "transcoding.h"
#include <QTcpSocket>
#include <QTime>

command::command(configure *cf):
    conf(cf)
{
    node_count = 0;
    times_server = new time_server();
    connect(times_server, &time_server::clock_update,
            this, &command::on_clock_update);
    connect(times_server, &time_server::clock_run,
            this, &command::clock_run);

    comm = new tcp_comm_implement();
    command_exec = new command_execution(conf);
}
 void command::on_network_recv(QTcpSocket *ts,const QByteArray &datas)
 {
    QString pro = QString(datas);
    if (pro.contains("clinet_to_server",Qt::CaseSensitive))
    {
        QStringList str_list = pro.split(";");
        if (str_list.size())
        {
            if (str_list[0] == "clinet_to_server")
            {
                QStringList addres;
                for (int i = 1; i < str_list.size() -1; i++)
                {
                    addres << str_list[i];
                }
                command_send(ts, addres, str_list[str_list.size() -1]);
            }
        }
        log_warning(tr("Receive data:[%1] [time:%2]")
                             .arg(ts->peerAddress().toString())
                             .arg(QTime::currentTime().toString("hh:mm:ss")));

        log_warning(QString(comm->data_byte()));
    }
    else
    {
        comm->analyze(datas);
        _comm_show(ts);
        _comm_type(ts);
    }
 }

 //根据接收数据进行发送
 void command::_comm_send()
 {
     comm->summary();
     QStringList addr = comm->target();
     for (int i = 0; i < addr.size(); i++)
     {
         if (!addr[i].isEmpty())
             emit send_data(addr[i], comm->data_byte());
     }
 }
 void command::_command_exec()
 {
     switch ((int)comm->command())
     {
     case Cmd_Null:
         break;
     case Cmd_Player_Play://由控制端发出
         times_server->command(Cmd_Play);
         break;
     case Cmd_Player_Stop:
         times_server->command(Cmd_Stop);
         break;
     case Cmd_Player_Pause:
         times_server->command(Cmd_Pause);
         break;
     case Cmd_Player_Continue:
         times_server->command(Cmd_Continues);
         break;
     case Cmd_Player_Repeat:
         times_server->command(Cmd_Repeat);
         break;
     case Cmd_Player_Volume:
         break;
     case Cmd_Player_Seek:
         times_server->command(Cmd_Seek, comm->data().toFloat());
         break;
     case Cmd_Player_Synchro://由服务器发出
     case Cmd_Player_Node://由服务器发出
         break;
     case Cmd_PJlink_Power_On://由控制端发出
     case Cmd_PJlink_Power_Off:
         //PJlink模块执行
         break;
     case Cmd_Softwrare_Off://由控制端发出
     case Cmd_Softwrare_On:
         //转发
         break;
     }
     bool ser_sta = false;
     QStringList ser = comm->target();
     for (int i =0; i< ser.size(); i++)
     {
         if (ser[i] == comm->server())
         {
             ser_sta = true;
         }
     }
     if (ser_sta)
     switch ((int)comm->command())
     {
     case Cmd_System_Shutdown://由控制端发出
     case Cmd_System_Boot:
     case Cmd_System_Volume:
     case Cmd_System_KeyboardMouse:
     case Cmd_System_Restart:
         //系统执行
         break;
     case Cmd_Softwrare_Off://由控制端发出
     case Cmd_Softwrare_On:
         //软件执行
         break;
         default:
             break;
     }
 }

 //根据接收数据进行显示
void command::_comm_show(QTcpSocket *ts)
 {
     log_information("");
     log_information(tr("*******************************"));
     log_information(tr("  Receive data:[%1] [time:%2]")
                          .arg(ts->peerAddress().toString())
                          .arg(QTime::currentTime().toString("hh:mm:ss")));

     log_information(tr("    protocol type:[%1]")
                           .arg(tcp_protocol_str[comm->protocol()]));
     log_information(tr("    command type:[%1]")
                           .arg(tcp_command_str[comm->command()]));

     log_information(tr("    server ip addres:[%1]")
                           .arg(comm->server()));
     log_information(tr("    source ip addres:[%1]")
                           .arg(comm->source()));
     log_information(tr("    target ip addres list:"));
     QStringList list = comm->target();
     for (int i = 0; i < list.size(); i++)
     {
         log_information(tr("     addres(%1): [%2]").arg(i).arg(list[i]));
     }
     log_information(tr("    data:"));
     QString dat = transcoding::ByteArrayToHexStr(comm->data());
     if (dat.size() < max_data_show_size)
         log_information(tr("      %1").arg(dat));

     log_information(tr("*******************************"));
     log_information("");
 }
void command::_comm_type(QTcpSocket *)
 {
     switch (comm->protocol())
     {
     case Prot_Null :
         break;
     case Prot_Heartbeat:

         break;
     case Prot_Command:
         _comm_send();
         _command_exec();
         break;
     case Prot_Answer:

         break;
     case Prot_Start_Send:

         break;
     case Prot_Data_Send:

         break;
     case Prot_End_Send:

         break;
    //disc_client(comm->target());
     default:
         break;
     }
 }

 command_enum command::_string_command(const QString &cmd)
 {
     if (cmd == "Player_Play") return Cmd_Player_Play;
     else if (cmd == "Player_Stop") return Cmd_Player_Stop;
     else if (cmd == "Player_Pause") return Cmd_Player_Pause;
     else if (cmd == "Player_Continue") return Cmd_Player_Continue;
     else if (cmd == "Player_Repeat") return Cmd_Player_Repeat;
     else if (cmd == "Player_Volume") return Cmd_Player_Volume;
     else if (cmd == "Player_Seek") return Cmd_Player_Seek;
     else if (cmd == "Player_Synchro") return Cmd_Player_Synchro;
     else if (cmd == "Player_Node") return Cmd_Player_Node;
     else if (cmd == "System_Shutdown") return Cmd_System_Shutdown;
     else if (cmd == "System_Boot") return Cmd_System_Boot;
     else if (cmd == "System_Volume") return Cmd_System_Volume;
     else if (cmd == "System_KeyboardMouse") return Cmd_System_KeyboardMouse;
     else if (cmd == "System_Restart") return Cmd_System_Restart;
     else if (cmd == "PJlink_Power_On") return Cmd_PJlink_Power_On;
     else if (cmd == "PJlink_Power_Off") return Cmd_PJlink_Power_Off;
     else if (cmd == "Softwrare_Off") return Cmd_Softwrare_Off;
     else if (cmd == "Softwrare_On") return Cmd_Softwrare_On;
     return Cmd_Null;
 }
 void command::command_send(QTcpSocket *ts, const QStringList &addres,
                 const QString &cmd)
 {
     command_enum cmd_ = _string_command(cmd);
     for (int i = 0; i < addres.size (); i++)
     {
         comm->protocol(Prot_Command);
         comm->command(cmd_);
         comm->server (ts->localAddress().toString());
         comm->source (ts->peerAddress().toString());
         comm->target (QStringList(addres[i]));
         comm->data (cmd.toLatin1());
         comm->summary ();
         emit send_data (addres[i], comm->data_byte ());
     }
 }
 void command::on_command(const QStringList &addres,
                          const QString &cmd)
 {
     QStringList mmm = conf->command_get(cmd);
     command_enum cmd_ = _string_command(mmm[0]);
     for (int i = 0; i < addres.size (); i++)
     {
         comm->protocol(Prot_Command);
         comm->command(cmd_);
         comm->server (server_ip);
         comm->source (server_ip);
         comm->target (QStringList(addres[i]));
         comm->data (cmd.toLatin1());
         comm->summary ();
         emit send_data (addres[i], comm->data_byte ());
     }
 }

 void command::on_clock_update(const quint32 v, const QByteArray &byte)
 {
     switch (v)
     {
     case Clock_Time:
         comm->protocol (Prot_Command);
         comm->command (Cmd_Player_Node);
         comm->server (server_ip);
         comm->source (server_ip);
         comm->target (QStringList());
         comm->data (byte);
         comm->summary ();
         emit send_data(QString(), comm->data_byte ());
         break;
     case Clock_Sync:
         comm->protocol (Prot_Command);
         comm->command (Cmd_Player_Synchro);
         comm->server (server_ip);
         comm->source (server_ip);
         comm->target (QStringList());
         comm->data (byte);
         comm->summary ();
         emit send_data(QString(), comm->data_byte ());
         break;
     default:
         break;
     }
 }
void command::on_command_exec(uint type, uint cmd, const float val)
{
    if (Clock_Sync == type)
    switch (cmd)
    {
    case 0xff10://启动
        times_server->begin();
        break;
    case 0xff11://停止
        times_server->end();
        break;
    case Cmd_Play:
    case Cmd_Player_Play://由控制端发出
        times_server->command(Cmd_Play);
        break;
    case Cmd_Stop:
    case Cmd_Player_Stop:
        times_server->command(Cmd_Stop);
        break;
    case Cmd_Pause:
    case Cmd_Player_Pause:
        times_server->command(Cmd_Pause);
        break;
    case Cmd_Continues:
    case Cmd_Player_Continue:
        times_server->command(Cmd_Continues);
        break;
    case Cmd_Repeat:
    case Cmd_Player_Repeat:
        times_server->command(Cmd_Repeat);
        break;
    case Cmd_Player_Volume:
        break;
    case Cmd_Seek:
    case Cmd_Player_Seek:
        times_server->command(Cmd_Seek, val);
        break;
    case Cmd_Player_Synchro://由服务器发出
    case Cmd_Player_Node://由服务器发出
        break;
    case Cmd_PJlink_Power_On://由控制端发出
    case Cmd_PJlink_Power_Off:
        //PJlink模块执行
        break;
    case Cmd_Softwrare_Off://由控制端发出
    case Cmd_Softwrare_On:
        //转发
        break;
    default:
        break;
    }
    else if(Clock_Time == type)
        switch (cmd)
        {
        case 0x10://append_node
            node_count = times_server->append_node(QString::number(node_count));
            break;
        case 0x11://save

            break;
        case 0x12://load

            break;
        default:
            break;
        }

}
