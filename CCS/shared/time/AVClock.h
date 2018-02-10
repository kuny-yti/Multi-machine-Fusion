
#ifndef QTAV_AVCLOCK_H
#define QTAV_AVCLOCK_H

#include <QtCore/QObject>
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
#include <QtCore/QElapsedTimer>
#else
#include <QtCore/QTime>
typedef QTime QElapsedTimer;
#endif

const quint32 Clock_Sync = 0xffffff01;
static const double kThousandth = 0.001;

class  AVClock : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        AudioClock, ExternalClock
    } ClockType;

    AVClock(ClockType c, QObject* parent = 0);
    AVClock(QObject* parent = 0);
    void setClockType(ClockType ct);
    ClockType clockType() const;
    bool isActive() const;

    void setClockAuto(bool a);
    bool isClockAuto() const;

    inline double value() const;
    inline void updateValue(double pts);

    void updateExternalClock(qint64 msecs);

    void updateExternalClock(const AVClock& clock);

    inline void updateVideoPts(double pts);
    inline double videoPts() const;
    inline double delay() const;
    inline void updateDelay(double delay);

    void setSpeed(qreal speed);
    inline qreal speed() const;
    static bool to_avclock(QByteArray &by ,AVClock &clock);
    static QByteArray to_byte(const AVClock &clock);
signals:
    void paused(bool);
    void paused();
    void resumed();
    void started();
    void resetted();

public slots:
    void start();
    void pause(bool p);
    void reset();

private:
    bool auto_clock;
    ClockType clock_type;
    mutable double pts_;
    double pts_v;
    double delay_;
    mutable QElapsedTimer timer;
    qreal mSpeed;
};

double AVClock::value() const
{
    if (clock_type == AudioClock) {
        return pts_ + delay_;
    } else {
        if (timer.isValid()) {
            pts_ += double(timer.restart()) * kThousandth;
        } else {//timer is paused
            qDebug("clock is paused. return the last value %f", pts_);
        }
        return pts_ * speed();
    }
}

void AVClock::updateValue(double pts)
{
    if (clock_type == AudioClock)
        pts_ = pts;
}

void AVClock::updateVideoPts(double pts)
{
    pts_v = pts;
}

double AVClock::videoPts() const
{
    return pts_v;
}

double AVClock::delay() const
{
    return delay_;
}

void AVClock::updateDelay(double delay)
{
    delay_ = delay;
}

qreal AVClock::speed() const
{
    return mSpeed;
}


#endif // QTAV_AVCLOCK_H
