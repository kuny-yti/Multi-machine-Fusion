#include "network_sync.h"
#include <QTimerEvent>

//! 时钟触发精度
const qint64 clock_accuracy = 1;
const int kSyncInterval = 2000;//毫秒

network_sync::network_sync(QObject *parent) :
    QThread(parent)
{
    _pause = false;
     clock = new AVClock(this);
     clock->setClockType(AVClock::ExternalClock);
}
void network_sync::play()
{
    clock->reset();
    clock->start();
    //times.invalidate();
    //times.start();
    //begin();
    timer_id = startTimer(kSyncInterval, Qt::PreciseTimer);
}
void network_sync::stop()
{
    clock->reset();
    //times.invalidate();
    //end();
    killTimer(timer_id);
}
void network_sync::pause()
{
    _pause = true;
    //times.invalidate();
    //end();
    clock->pause(true);
}
void network_sync::continues()
{
    _pause = false;
    //times.start();
    //begin();
    clock->pause(false);
}

void network_sync::seek(float val)
{
    clock->updateExternalClock(clock->value()*1000.0 +val);//毫秒
    QMutexLocker lock(&_mutex);
    _external_times = clock->value();
    //times.restart();
}//设置位置

void network_sync::toggle_play()
{
    if (clock->isActive())
    {
        stop();
    }
    else
    {
        play();
    }
}
void network_sync::toggle_pause()
{
    if (_pause)
    {
        continues();
    }
    else
    {
        pause();
    }
}

void network_sync::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != timer_id)
    {
        qDebug("Not clock id");
        return;
    }
    if (!clock->isActive())
    {
        qDebug("clock not running");
        return;
    }
    QMutexLocker lock(&_mutex);
    _external_times = clock->value();
    //times.restart();
    //emit clock_run(_external_times);
    emit sync_update(*clock);
}

void network_sync::run()
{
    while (_state)
    {
        _mutex.lock();
        //_external_times += double(times.restart()) * kThousandth;
        {
            for (int i = 0; i < _node.size(); i++)
            {
                float a = _node[i] +clock_accuracy;
                float b = _node[i] -clock_accuracy;
                if ((a > _external_times) && (b < _external_times))
                {
                    emit time_out(_external_times, _node_name[i]);
                }
            }
        }
        _mutex.unlock();
    }
}
void network_sync::begin()
{
    QMutexLocker lock(&_mutex);
    if (!this->isRunning())
    {
        this->start();
        //times.restart();
        //times.start();
    }

    //_clock_cores->nsecsElapsed();//纳秒
    //_clock_cores->elapsed();//返回过去的毫秒
}
void network_sync::end()
{
    QMutexLocker lock(&_mutex);
    if (this->isRunning())
    {
        this->terminate();
        //times.restart();
    }
}
int network_sync::append_node(const qint64 time, const QString &name)
{
    QMutexLocker lock(&_mutex);
    {
        _node << time;
        _node_name << name;
        return _node.size()-1;
    }
    return -1;
}
int network_sync::append_node(const QString &name)
{
    QMutexLocker lock(&_mutex);
    {
        _node << _external_times;
        _node_name << name;
        return _node.size()-1;
    }
    return -1;
}
