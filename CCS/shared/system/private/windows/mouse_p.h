#ifndef MOUSE_P_H
#define MOUSE_P_H

#include "private/system_mouse_p.h"

class mouse_p : public system_mouse_p
{
public:
    mouse_p();
    ~mouse_p();
    virtual QByteArray toByte(bool, const QMouseEvent &, const QWheelEvent &);
    virtual QMouseEvent toEventMouse(const QByteArray &) ;
    virtual QWheelEvent toEventWheel(const QByteArray &);
    virtual void excEvent(const QMouseEvent&k);
    virtual void excEvent(const QWheelEvent&k);
    virtual void excEvent(const QByteArray&by);
};

#endif // MOUSE_P_H
