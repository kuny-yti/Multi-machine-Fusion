#include "client_pro.h"
#include "tcp_comm_implement.h"
#include "c_tcp_implement.h"
#include "command_execution.h"
#include "transcoding.h"
#include "gui.h"
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>

client_pro *client_pro::_this = NULL;

const QRect toDesktopGeometry()
{
    uint screenNumber = 0;
    uint screenHeightMax = 0;
    uint screenHeightMaxBak = 0;

    QRect rect;
    QDesktopWidget *desktopWidget = QApplication::desktop();
    //QPixmap pix = QPixmap::grabWindow(desktopWidget->winId());
    //pix.save("C:/Qt/bmp.bmp");
    if (desktopWidget->screenCount())
    {
       screenNumber =  desktopWidget->screenCount();
    }
    else
    {
        return QRect();
    }

    for (uint i = 0; i < screenNumber; i++)
    {
        screenHeightMax = desktopWidget->screenGeometry(i).height();
        if (screenHeightMax > screenHeightMaxBak)
        {
            screenHeightMaxBak = screenHeightMax;
            rect.setHeight(screenHeightMax);
        }
        rect.setWidth(desktopWidget->screenGeometry(i).width() + rect.width());
    }
    return rect;
}

client_pro::client_pro(configure *file)
{
    config = file;
    key_gui = new GUI();
    key_gui->setWindowFlags(Qt::FramelessWindowHint |
                          Qt::WindowStaysOnTopHint);
    key_gui->setGeometry(toDesktopGeometry());

    key_gui->setHidden(true);

    connect(key_gui, SIGNAL(mouse(QMouseEvent*)),
            this, SLOT(on_mouse(QMouseEvent*)));
    connect(key_gui, SIGNAL(wheel(QWheelEvent*)),
            this, SLOT(on_wheel(QWheelEvent*)));
    connect(key_gui, SIGNAL(key(QKeyEvent*)),
            this, SLOT(on_key(QKeyEvent*)));
    _this = this;
}
void client_pro::init()
{
    comm = new tcp_comm_implement();
    comm_send = new tcp_comm_implement();
    command = new command_execution(config);
    if (config->isOk())
    {
        _read_command_list();
    }

    tcp_client = new c_tcp_implement();
    tcp_client->recv(&client_pro::recv);
    tcp_client->error(&client_pro::error);
    tcp_client->disconnected(&client_pro::disconnected);
    tcp_client->connected(&client_pro::connected);

    on_time_client();

    time = new QTimer(this);
    connect(time, SIGNAL(timeout()), this, SLOT(on_time_client()));
    if (config->heartbeat() > 100)
        time->start(config->heartbeat());
}
void client_pro::_read_command_list()
{
    QStringList play = config->command_play ();
    QStringList system = config->command_system ();
    Player_Command <<("[Player_Command]");
    Player_Command << play;
    System_Command <<("[System_Command]");
    System_Command << system;
}
void client_pro::_save_command_list()
{
    if (Player_Command.size ())
    {
        Player_Command.removeAt(0);
        config->command_play (Player_Command);
    }
    if (System_Command.size ())
    {
        System_Command.removeAt(0);
        config->command_system (System_Command);
    }
}
client_pro *client_pro::object()
{
    return _this;
}

void client_pro::command_save(const QStringList &Player, const QStringList &System)
{
    Player_Command = Player;
    System_Command = System;
    _save_command_list();

    log_information(tr("To connect to the server. [time:%1]")
                         .arg(QTime::currentTime().toString("hh:mm:ss")));

}
void client_pro::command_read(QStringList &Player, QStringList &System)
{
    Player_Command.clear();
    System_Command.clear();
    _read_command_list();
    Player = Player_Command;
    System = System_Command;
}
void client_pro::_transit_type(QTcpSocket *ts)
{
    _transit_show(ts);
    switch (comm->send_cmd())
    {
    case CMD_HEARTBEAT:

        break;
    case CMD_C_CONNECT_S:
        emit s_addres(comm->target_addres());
        break;
    case CMD_C_DISCON_S:

        break;

    case CMD_S_DISCON_ALL:

        break;
    case CMD_S_DISCON_C:

        break;
    case CMD_C_DISCON_C:

        break;
    case CMD_C_OFF_S:

        break;
    case CMD_C_COMM_C:
    case CMD_C_COMM_MC:
        break;
    case CMD_C_COMM_S:

        break;

    default:
        break;
    }
}
void client_pro::_recv(QTcpSocket *ts, const QByteArray &datas)
{
    comm->analyze(datas);
    switch (comm->send_type())
    {
    case TYPE_TRANSIT:
    case TYPE_TRANSIT_CIPHERTEXT:
        _transit_type(ts);
        command->execution(comm->send_data());
        break;
    case TYPE_TRANSIT_NO:

        break;
    default:
        log_warning(tr("Receive data:[%1] [time:%2]")
                     .arg(ts->peerAddress().toString())
                     .arg(QTime::currentTime().toString("hh:mm:ss")));

        log_warning(comm->data_byte());
        break;
    }
}
void client_pro::_error(const QString &ad, const QString &err)
{
    log_error (tr("server(%1) error(%2). [time:%3]").arg(ad).arg(err)
                      .arg(QTime::currentTime ().toString ()));
}
void client_pro::_disconnected(const QString &ad)
{
    log_warning (tr("disconnected to server(%1) . [time:%2]").arg(ad)
                      .arg(QTime::currentTime ().toString ()));

    emit s_disconnect();
}
void client_pro::_connected(QTcpSocket *,const QString& ad)
{
    log_warning (tr("connected to server(%1) . [time:%2]").arg(ad)
                      .arg(QTime::currentTime ().toString ()));
    _type_();
}
void client_pro::_type_()
{
    comm_send->clean_byte();
    comm_send->clean_struct();
    comm_send->send_cmd(CMD_C_CONNECT_S);
    comm_send->send_type(TYPE_TRANSIT_CIPHERTEXT);
    comm_send->server_addres(config->addres());
    comm_send->source_addres(QString());
    comm_send->target_addres(QStringList());
    comm_send->send_data(QString::number((int)S_CS).toLatin1());
    comm_send->summary();
    if (tcp_client->send(comm_send->data_byte()) <= 0)
    {
        log_warning (tr("send to client type fail! [time:%1")
                     .arg(QTime::currentTime ().toString ()));
    }
}

void client_pro::recv(QTcpSocket *ts, const QByteArray &ba)
{
    _this->_recv(ts, ba);
}
void client_pro::error(const QString &ad, const QString &err)
{
    _this->_error(ad, err);
}
void client_pro::disconnected(const QString &ad)
{
    _this->_disconnected(ad);
}
void client_pro::connected(QTcpSocket *th,const QString&ad)
{
    _this->_connected(th, ad);
}

void client_pro::_transit_show(QTcpSocket *ts)
{
    log_information(tr("*******************************"));
    log_information(tr("  Receive data:[%1] [time:%2]")
                         .arg(ts->peerAddress().toString())
                         .arg(QTime::currentTime().toString("hh:mm:ss")));

    log_information(tr("    server send cmd:[%1]")
                          .arg(tcp_cmd_str[comm->send_cmd()]));
    log_information(tr("    server send type:[%1]")
                          .arg(tcp_type_str[comm->send_type()]));

    log_information(tr("    server server ip addres:[%1]")
                          .arg(comm->server_addres()));
    log_information(tr("    server source ip addres:[%1]")
                          .arg(comm->source_addres()));
    log_information(tr("    server target ip addres list:"));
    QStringList list = comm->target_addres();
    for (int i = 0; i < list.size(); i++)
    {
        log_information(tr("      [%1]").arg(list[i]));
    }
    log_information(tr("    server send data:"));
    QString dat = transcoding::ByteArrayToHexStr(comm->send_data());
    log_information(tr("      %1").arg(dat));
    log_information(tr("*******************************"));
}
void client_pro::on_time_client()
{
    comm_send->clean_byte();
    comm_send->clean_struct();
    comm_send->send_cmd(CMD_HEARTBEAT);
    comm_send->send_type(TYPE_TRANSIT_CIPHERTEXT);
    comm_send->server_addres(config->addres());
    comm_send->source_addres(QString());
    comm_send->target_addres(QStringList());
    comm_send->send_data(QByteArray());
    comm_send->summary();
    if (tcp_client->send(comm_send->data_byte()) <= 0)
    {
        log_warning (tr("Connect to server. [time:%1")
                     .arg(QTime::currentTime ().toString ()));
        tcp_client->connect(config->port(), config->addres());
    }
}
void client_pro::on_discon_c(const QStringList &addres)
{
    comm_send->clean_byte();
    comm_send->clean_struct();
    comm_send->send_cmd(CMD_C_DISCON_C);
    comm_send->send_type(TYPE_TRANSIT_CIPHERTEXT);
    comm_send->server_addres(config->addres());
    comm_send->source_addres(QString());
    comm_send->target_addres(addres);
    comm_send->send_data(QByteArray());
    comm_send->summary();
    if (tcp_client->send(comm_send->data_byte()) <= 0)
    {
        log_warning (tr("send to data fail! [time:%1")
                     .arg(QTime::currentTime ().toString ()));
    }
}

void client_pro::command_send(const QStringList &addres,const QByteArray &data)
{
    comm_send->clean_byte();
    comm_send->clean_struct();
    comm_send->send_cmd(CMD_C_COMM_C);
    comm_send->send_type(TYPE_TRANSIT_CIPHERTEXT);
    comm_send->server_addres(config->addres());
    comm_send->source_addres(QString());
    comm_send->target_addres(addres);
    comm_send->send_data(data);
    comm_send->summary();
    if (tcp_client->send(comm_send->data_byte()) <= 0)
    {
        log_warning (tr("send to data fail! [time:%1")
                     .arg(QTime::currentTime ().toString ()));
    }
}
void client_pro::ui()
{
    if (key_gui->isHidden())
    {
        key_gui->setHidden(false);
    }
}

QStringList address ;
QString cmds;
void client_pro::on_mouse(QMouseEvent*m)
{
    system_mouse ke;
    QByteArray out = ke.toByte(true, *m, QWheelEvent(QPointF(0,0), 0,0,0));
    QByteArray tem = command->toByte(cmds, out);
    command_send(address, tem);
}
void client_pro::on_wheel(QWheelEvent*w)
{
    system_mouse ke;
    QByteArray out = ke.toByte(false, QMouseEvent(QEvent::None, QPointF(0,0),
                                           Qt::NoButton,0,0), *w);
    QByteArray tem = command->toByte(cmds, out);
    command_send(address, tem);
}
void client_pro::on_key(QKeyEvent*k)
{
    system_keyboard ke;
    QByteArray out = ke.toByte(*k);
    QByteArray tem = command->toByte(cmds, out);
    command_send(address, tem);
}

void client_pro::on_command(const QStringList &addres, const QString &cmd)
{
    QStringList sss = cmd.split("@");
    if (sss[0] == "KeyboardMouse")
    {
        address = addres;
        cmds = cmd;
        ui();
        return ;
    }
    QByteArray out = command->toByte(cmd, QByteArray());
    command_send(addres, out);
}
