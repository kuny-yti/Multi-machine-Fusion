#include "command.h"
#include "configure.h"
#include "command_execution.h"
#include "tcp_comm_implement.h"
#include "c_tcp_implement.h"
#include "transcoding.h"
#include <QTcpSocket>
#include <QTime>

command::command(configure *cf):
    conf(cf)
{
    comm = new tcp_comm_implement();
    command_exec = new command_execution(conf);
}
 void command::on_network_recv(QTcpSocket *ts,const QByteArray &datas)
 {
    /*QString pro = QString(datas);
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
    else*/
    {
        comm->analyze(datas);
        _comm_show(ts);
        _comm_type(ts);
    }
 }

 void command::_command_exec()
 {
     switch ((int)comm->command())
     {
     case Cmd_Null:
         break;
     case Cmd_Player_Play://由控制端发出
     case Cmd_Player_Stop:
     case Cmd_Player_Pause:
     case Cmd_Player_Continue:
     case Cmd_Player_Repeat:
     case Cmd_Player_Volume:
     case Cmd_Player_Seek:
     case Cmd_Player_Synchro://由服务器发出
     case Cmd_Player_Node://由服务器发出
         emit  s_player_command(comm->command(), comm->data());
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
         command_exec->execution(comm->command(), comm->data());

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
