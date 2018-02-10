#ifndef SYSTEM_KEYBOARD_H
#define SYSTEM_KEYBOARD_H
#include <QKeyEvent>

class system_keyboard_p;

class system_keyboard
{
    system_keyboard_p *d_ptr;
    QString cmd_str;
    uint cmd_val;

public:
    system_keyboard(const QString &cmd, const uint val);
    system_keyboard();
    ~system_keyboard();

    QString &cmd_table();
    uint &cmd_table_value();

    QByteArray toByte(const QKeyEvent &);
    QKeyEvent toEvent(const QByteArray &);

    void excEvent(const QKeyEvent&);
    void excEvent(const QByteArray&);
};

#endif // SYSTEM_KEYBOARD_H
