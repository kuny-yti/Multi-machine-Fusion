
#include <AVClock.h>
#include <QDataStream>

QByteArray AVClock::to_byte(const AVClock &clock)
{
    static QByteArray byte_;

    QDataStream out(&byte_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out << (quint32)Clock_Sync;
    /*out << (quint32)clock.clockType();
    out << (double)clock.delay();
    out << (double)clock.isClockAuto();
    out << (double)clock.speed();
    out << (double)clock.videoPts();*/
    out << (double)clock.value();
    return byte_;
}
bool AVClock::to_avclock(QByteArray &byte_ ,AVClock &clock)
{
    QDataStream out(&byte_, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_1);

    quint32 time;
    double temp = 0;
    //bool state;
    out >> time; ;
    if (time != Clock_Sync) return false;

    /*out >> time;   clock.setClockType((AVClock::ClockType)time);
    out >> temp;   clock.updateDelay(temp);
    out >> state;  clock.setClockAuto(state);
    out >> temp;   clock.setSpeed(temp);
    out >> temp;   clock.updateVideoPts(temp);*/
    out >> temp;   clock.updateValue(temp);
    return true;
}

AVClock::AVClock(AVClock::ClockType c, QObject *parent):
    QObject(parent)
  , auto_clock(true)
  , clock_type(c)
  , mSpeed(1.0)
{
    pts_ = pts_v = delay_ = 0;
}

AVClock::AVClock(QObject *parent):
    QObject(parent)
  , auto_clock(true)
  , clock_type(AudioClock)
  , mSpeed(1.0)
{
    pts_ = pts_v = delay_ = 0;
}

void AVClock::setClockType(ClockType ct)
{
    clock_type = ct;
}

AVClock::ClockType AVClock::clockType() const
{
    return clock_type;
}

bool AVClock::isActive() const
{
    return clock_type == AudioClock || timer.isValid();
}

void AVClock::setClockAuto(bool a)
{
    auto_clock = a;
}

bool AVClock::isClockAuto() const
{
    return auto_clock;
}

void AVClock::updateExternalClock(qint64 msecs)
{
    if (clock_type != ExternalClock)
        return;
    qDebug("External clock change: %f ==> %f", value(), double(msecs) * kThousandth);
    pts_ = double(msecs) * kThousandth; //can not use msec/1000.
    timer.restart();
}

void AVClock::updateExternalClock(const AVClock &clock)
{
    if (clock_type != ExternalClock)
        return;
    qDebug("External clock change: %f ==> %f", value(), clock.value());
    pts_ = clock.value();
    timer.restart();
}

void AVClock::setSpeed(qreal speed)
{
    mSpeed = speed;
}

void AVClock::start()
{
    qDebug("AVClock started!!!!!!!!");
    timer.start();
    emit started();
}
//remember last value because we don't reset  pts_, pts_v, delay_
void AVClock::pause(bool p)
{
    if (clock_type != ExternalClock)
        return;
    if (p) {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        timer.invalidate();
#else
        timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        emit paused();
    } else {
        timer.start();
        emit resumed();
    }
    emit paused(p);
}

void AVClock::reset()
{
    pts_ = pts_v = delay_ = 0;
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    timer.invalidate();
#else
    timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    emit resetted();
}



