#ifndef TIME_SERVER_H
#define TIME_SERVER_H

#include <QObject>
#include <AVClock.h>

const quint32 Clock_Time = 0xffffff02;

typedef enum
{
    Cmd_Play = 0xffffff10,
    Cmd_Stop,
    Cmd_Pause,
    Cmd_Continues,
    Cmd_Repeat,
    Cmd_Toggle_Play,
    Cmd_Toggle_Pause,
    Cmd_Mute,
    Cmd_Seek,
    Cmd_Volume
}PlayerCommand;

class time_server_p;
class time_server : public QObject
{
    Q_OBJECT
    time_server_p *_d;
public:
    explicit time_server(QObject *parent = 0);

    ~time_server();

    void begin();
    void end();
    //! 根据外部时钟添加一个节点
    int append_node(const qint64 time, const QString &name = QString());
    int append_node(const QString &name = QString());
signals:
    void clock_update(const quint32, const QByteArray &byte);
    void clock_run(const float val);
private slots:
    void sync_update(const AVClock &clock);
    void time_out (const qint64 curr_time, const QString &node_name);

public slots:
    void command(const PlayerCommand, const float val = 0.0);
};

#endif // TIME_SERVER_H
