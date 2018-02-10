#ifndef SYSTEM_MOUSE_P_H
#define SYSTEM_MOUSE_P_H

#include <QByteArray>
#include <QMouseEvent>
#include <QWheelEvent>

class system_mouse_p
{
public:
    virtual ~system_mouse_p(){}

    virtual QByteArray toByte(bool, const QMouseEvent &, const QWheelEvent &) = 0;
    virtual QMouseEvent toEventMouse(const QByteArray &) = 0;
    virtual QWheelEvent toEventWheel(const QByteArray &) = 0;
    virtual void excEvent(const QMouseEvent&k) = 0;
    virtual void excEvent(const QWheelEvent&k) = 0;
    virtual void excEvent(const QByteArray&by) = 0;
};

#endif // SYSTEM_MOUSE_P_H
