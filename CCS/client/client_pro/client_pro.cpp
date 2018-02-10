#include "client_pro.h"
#include "tcp_comm_implement.h"
#include "command.h"
#include "c_tcp_implement.h"
#include "transcoding.h"
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

client_pro *client_pro::_this = NULL;

client_pro::client_pro()
{
    _this = this;
}
void client_pro::init()
{
    comm_send = new tcp_comm_implement();

    if (config.isOk())
    {
        _read_command_list();
    }

    command_exec = new command(&config);
    connect(command_exec, &command::s_player_command,
            this, &client_pro::s_player_command);

    tcp_client = new c_tcp_implement();
    tcp_client->recv(&client_pro::recv);
    tcp_client->error(&client_pro::error);
    tcp_client->disconnected(&client_pro::disconnected);
    tcp_client->connected(&client_pro::connected);

    on_time_client();

    time = new QTimer(this);
    connect(time, SIGNAL(timeout()), this, SLOT(on_time_client()));
    if (config.heartbeat() > 100)
        time->start(config.heartbeat());
}
void client_pro::_read_command_list()
{
    QStringList play = config.command ();
    Player_Command <<("[Command]");
    Player_Command << play;
}
void client_pro::_save_command_list()
{
    if (Player_Command.size ())
    {
        Player_Command.removeAt(0);
        config.command (Player_Command);
    }
}
client_pro *client_pro::object()
{
    return _this;
}

void client_pro::command_save(const QStringList &Player)
{
    Player_Command = Player;
    _save_command_list();

    log_information(tr("To connect to the server. [time:%1]")
                         .arg(QTime::currentTime().toString("hh:mm:ss")));

}
void client_pro::command_read(QStringList &Player)
{
    Player_Command.clear();
    _read_command_list();
    Player = Player_Command;
}

void client_pro::_recv(QTcpSocket *ts, const QByteArray &datas)
{
    command_exec->on_network_recv(ts, datas);
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
}
void client_pro::_connected(QTcpSocket *,const QString& ad)
{
    log_warning (tr("connected to server(%1) . [time:%2]").arg(ad)
                      .arg(QTime::currentTime ().toString ()));
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

void client_pro::on_time_client()
{
    comm_send->clean_byte();
    comm_send->clean_struct();

    comm_send->command(Cmd_Null);
    comm_send->protocol(Prot_Heartbeat);
    comm_send->server(config.addres());
    comm_send->source(QString());
    comm_send->target(QStringList());
    comm_send->data(QByteArray());
    comm_send->summary();
    if (tcp_client->send(comm_send->data_byte()) <= 0)
    {
        log_warning (tr("Connect to server. [time:%1")
                     .arg(QTime::currentTime ().toString ()));
        tcp_client->connect(config.port(), config.addres());
    }
}
