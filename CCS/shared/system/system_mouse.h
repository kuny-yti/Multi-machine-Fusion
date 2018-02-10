#ifndef SYSTEM_MOUSE_H
#define SYSTEM_MOUSE_H
#include <QMouseEvent>
#include <QWheelEvent>

class system_mouse_p;

class system_mouse
{
    system_mouse_p *d_ptr;
    QString cmd_str;
    uint cmd_val;

public:
    system_mouse(const QString &cmd, const uint val);
    system_mouse();
    ~system_mouse();

    QString &cmd_table();
    uint &cmd_table_value();

    QByteArray toByte(bool ,const QMouseEvent &, const QWheelEvent &);
    QMouseEvent toEventMouse(const QByteArray &) ;
    QWheelEvent toEventWheel(const QByteArray &);

    void excEvent(const QMouseEvent&k);
    void excEvent(const QWheelEvent&k);
    void excEvent(const QByteArray&by);
};

#endif // SYSTEM_MOUSE_H
