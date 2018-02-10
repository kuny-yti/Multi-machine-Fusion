#ifndef NETWORK_SYNC_H
#define NETWORK_SYNC_H

#include <QThread>
#include "AVClock.h"
#include <QMutex>
#include <QElapsedTimer>
#include <QList>
#include <QString>

//! 根据融合播放器,进行网络同步服务器
class network_sync : public QThread
{
    Q_OBJECT
    friend class serialize;
private:
    float _external_times;
    QElapsedTimer times;
    QList <float> _node ;
    QList <QString> _node_name;
    bool _state;
    QMutex _mutex;

    int timer_id;
    AVClock *clock;
    bool _pause;
public:
    explicit network_sync(QObject *parent = 0);

    //! 启动时钟
    void begin();
    //! 结束时钟
    void end();

    //! 定时器检测线程运行
    void run();

    //! 更新外部时钟时间
    void update_nanosecond(const qint64 time);//纳秒
    void update_msec(const qint64 time);//毫秒

    virtual void timerEvent(QTimerEvent *e);
signals:
    void sync_update(const AVClock &clock);
    void clock_run(const float);
signals:
    //! 内部计时器发出有相应节点
    void time_out (const qint64 curr_time, const QString &node_name);
public slots:
    void play();//播放
    void stop();//停止

    void pause();//暂停
    void continues();

    void seek(float val);//设置位置//毫秒

    void toggle_play();//播放/停止
    void toggle_pause();//暂停/继续

public slots:
    //! 根据外部时钟添加一个节点
    int append_node(const qint64 time, const QString &name = QString());
    int append_node(const QString &name = QString());
};

#endif // NETWORK_SYNC_H
