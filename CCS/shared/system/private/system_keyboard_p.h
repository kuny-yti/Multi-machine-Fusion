#ifndef SYSTEM_KEYBOARD_P_H
#define SYSTEM_KEYBOARD_P_H
#include <QByteArray>
#include <QKeyEvent>

class system_keyboard_p
{
public:
    virtual ~system_keyboard_p(){}

    virtual QByteArray toByte(const QKeyEvent &) = 0;
    virtual QKeyEvent toEvent(const QByteArray &) = 0;
    virtual void excEvent(const QKeyEvent&) = 0;
    virtual void excEvent(const QByteArray&)= 0;
};
#endif // SYSTEM_KEYBOARD_P_H
