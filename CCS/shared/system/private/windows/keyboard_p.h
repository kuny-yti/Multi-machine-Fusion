#ifndef KEYBOARD_P_H
#define KEYBOARD_P_H

#include "private/system_keyboard_p.h"

class keyboard_p : public system_keyboard_p
{
public:
    keyboard_p();
    ~keyboard_p();

    virtual QByteArray toByte(const QKeyEvent &);
    virtual QKeyEvent toEvent(const QByteArray &);

    virtual void excEvent(const QKeyEvent&);
    virtual void excEvent(const QByteArray&);
};

#endif // KEYBOARD_P_H
