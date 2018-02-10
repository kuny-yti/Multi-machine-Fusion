#include "server_time.h"

server_time::server_time(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    QString text = "0.89,1#1.7,nam";
    update_node(text.toLatin1());
}

void server_time::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void server_time::on_but_append_node_clicked()
{
    emit command_exec(Clock_Time, 0x10, 0.0);
}

void server_time::on_but_save_clicked()
{
    emit command_exec(Clock_Time, 0x11, 0.0);
}

void server_time::on_but_load_clicked()
{
    emit command_exec(Clock_Time, 0x12, 0.0);
}

void server_time::on_but_start_clicked()
{
    emit command_exec(Clock_Sync, 0xff10, 0.0);
}

void server_time::on_but_stop_clicked()
{
    emit command_exec(Clock_Sync, 0xff11, 0.0);
}

void server_time::on__seek_slider_valueChanged(int value)
{
    emit command_exec(Clock_Sync, Cmd_Seek, value*0.0001);
}

void server_time::on__play_button_clicked()
{
    emit command_exec(Clock_Sync, Cmd_Play, 0.0);
}

void server_time::on__pause_button_clicked()
{
     emit command_exec(Clock_Sync, Cmd_Pause, 0.0);
}

void server_time::on__stop_button_clicked()
{
    emit command_exec(Clock_Sync, Cmd_Stop, 0.0);
}

void server_time::on__continue_button_clicked()
{
    emit command_exec(Clock_Sync, Cmd_Continues, 0.0);
}

void server_time::on__repeat_button_clicked()
{
    //emit command_exec(Clock_Sync, Cmd_Player_Repeat, 0.0);
}

void server_time::on__toggle_play_button_clicked()
{
    //emit command_exec(Clock_Sync, Cmd_Player_Toggle_Play, 0.0);
}

void server_time::on__toggle_pause_button_clicked()
{
    //emit command_exec(Clock_Sync, Cmd_Player_Toggle_Pause, 0.0);
}
void server_time::update_seek(const float val)
{
    _seek_slider->setValue(val / 0.001);
}
void server_time::update_node(const QByteArray &val)
{
    QStringList list = QString(val).split("#");
    this->tree_node->clear();
    for (int i = 0; i < list.size(); i++)
    {
        QStringList name = list[i].split(",");
        if (name.size() ==1)
        {
            this->tree_node->append_node(i, name[0].toFloat(), QString());
        }
        else if (name.size() ==2)
        {
            this->tree_node->append_node(i, name[0].toFloat(), name[1]);
        }

    }
}
