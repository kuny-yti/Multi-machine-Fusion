#ifndef PLAYERNODE_H
#define PLAYERNODE_H

#include "ui_playernode.h"
#include "time_server.h"
#include "tcp_interfaces.h"

class server_time : public QWidget, private Ui::PlayerNode
{
    Q_OBJECT

public:
    explicit server_time(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
signals:
    void command_exec(uint type, uint cmd, const float val);
public slots:
    void update_seek(const float val);
    void update_node(const QByteArray &val);
private slots:
    void on_but_append_node_clicked();
    void on_but_save_clicked();
    void on_but_load_clicked();
    void on_but_start_clicked();
    void on_but_stop_clicked();
    void on__seek_slider_valueChanged(int value);
    void on__play_button_clicked();
    void on__pause_button_clicked();
    void on__stop_button_clicked();
    void on__continue_button_clicked();
    void on__repeat_button_clicked();
    void on__toggle_play_button_clicked();
    void on__toggle_pause_button_clicked();
};

#endif // PLAYERNODE_H
