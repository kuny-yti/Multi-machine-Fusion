#include "time_server.h"
#include "network_sync.h"
#include <QByteArray>
#include <QDataStream>
#include <QTime>

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

class time_server_p
{
public :
    time_server_p()
    {
        _sync = 0;
    }

    QByteArray to_byte(const qint64 curr_time, const QString &node_name)
    {
        QByteArray byte_;

        QDataStream out(&byte_, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_2);

        out << Clock_Time;
        out << node_name << curr_time;
    }
    bool to_time(const QByteArray &by, qint64 &curr_time, QString &node_name)
    {
        QByteArray byte_ = by;

        QDataStream out(&byte_, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_2);

        quint32 time;
        out >> time;
        if (time != Clock_Time) return false;
        out >> node_name >> curr_time;

        return true;
    }

    network_sync *_sync;
};

time_server::time_server(QObject *parent) :
    QObject(parent)
{
    _d = new time_server_p();
}
time_server::~time_server()
{
    delete _d;
}

void time_server::begin()
{
    if (!_d->_sync)
    {
        _d->_sync = new network_sync();
        connect(_d->_sync, &network_sync::sync_update,
                this, &time_server::sync_update);
        connect(_d->_sync, &network_sync::time_out,
                this, &time_server::time_out);
        connect(_d->_sync, &network_sync::clock_run,
                this, &time_server::clock_run);

        log_information(tr("start sync server. [time:%1]").arg(QTime::currentTime().toString("hh:mm:ss")));
    }

}
void time_server::end()
{
    if (_d->_sync)
        delete _d->_sync;
    _d->_sync = 0;
    log_information(tr("stop sync server. [time:%1]").arg(QTime::currentTime().toString("hh:mm:ss")));
}
int time_server::append_node(const qint64 time, const QString &name )
{
    if (_d->_sync)
        return _d->_sync->append_node(time, name);
    log_information(tr("append sync node(%1,%2). [time:%3]").arg(time).arg(name).arg(QTime::currentTime().toString("hh:mm:ss")));
}
int time_server::append_node(const QString &name)
{
    if (_d->_sync)
        return _d->_sync->append_node(name);
    log_information(tr("append sync node(%1). [time:%2]").arg(name).arg(QTime::currentTime().toString("hh:mm:ss")));
}
void time_server::sync_update(const AVClock &clock)
{
    QByteArray arr = AVClock::to_byte(clock);
    emit clock_update(Clock_Sync, arr);
}
void time_server::time_out (const qint64 curr_time, const QString &node_name)
{
    QByteArray arr = _d->to_byte(curr_time, node_name);
    emit clock_update(Clock_Time, arr);
}

void time_server::command(const PlayerCommand cmd, const float val)
{
    if (!_d->_sync) return;

    switch ((int)cmd)
    {
    case Cmd_Play:
        _d->_sync->play();
        log_information(tr("sync play. [time:%3]").arg(QTime::currentTime().toString("hh:mm:ss")));
        break;
    case Cmd_Stop:
        _d->_sync->stop();
        log_information(tr("sync stop. [time:%3]").arg(QTime::currentTime().toString("hh:mm:ss")));
        break;
    case Cmd_Pause:
        _d->_sync->pause();
        log_information(tr("sync pause. [time:%3]").arg(QTime::currentTime().toString("hh:mm:ss")));
        break;
    case Cmd_Continues:
        _d->_sync->continues();
        log_information(tr("sync continue. [time:%3]").arg(QTime::currentTime().toString("hh:mm:ss")));
        break;
    case Cmd_Repeat:
        _d->_sync->stop();
        _d->_sync->play();
        break;
    case Cmd_Toggle_Play:
        _d->_sync->toggle_play();
        break;
    case Cmd_Toggle_Pause:
        _d->_sync->pause();
        break;
    case Cmd_Seek:
        _d->_sync->seek(val);
        break;
    default:
        break;
    }
}
