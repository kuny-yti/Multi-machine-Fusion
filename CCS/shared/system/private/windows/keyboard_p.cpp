#include "keyboard_p.h"
#include "private/system_keyboard_p.h"
#include "KEY_W.h"
#include <QDataStream>
#include <windows.h>
#include <QThread>
#include "message.h"
#include <QTime>

#define F_KEY(qt) ((qt &= 0xff) += 0x40)


keyboard_p::keyboard_p()
{
}
keyboard_p::~keyboard_p()
{

}
QByteArray keyboard_p::toByte(const QKeyEvent &key)
{
    QByteArray out;

    QDataStream in(&out, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_1);

    in << (qint32)3;
    in << (qint32)key.type();
    in << (qint32)key.modifiers();
    in << (qint32)key.key();
    in << key.text();
    return out;
}
QKeyEvent keyboard_p::toEvent(const QByteArray &byte)
{
    qint32 temp_v = 0;
    qint32 temp_type = 0;
    qint32 temp_modif = 0;
    qint32 temp_key = 0;
    QString text;
    QByteArray date = byte;

    QDataStream in(&date, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_1);

    in >> temp_v;
    if (temp_v == 3)
    {
        in >> temp_type;
        in >> temp_modif;
        in >> temp_key;
        in >> text;
    }

    return QKeyEvent((QKeyEvent::Type)temp_type, (int)temp_key,
                     (Qt::KeyboardModifiers)temp_modif, text);
}

static void char_key(const QKeyEvent&)//字符按键
{

}

static void number_keyboard_key(const QKeyEvent&) //数字键盘
{

}
static void button_press (KEY_W code)
{
    keybd_event(code, 0, 0, 0);
}
static void button_up (KEY_W code)
{
    keybd_event(code, 0, KEYEVENTF_KEYUP, 0);
}
static void button_press (KEY_W code, KEY_W code1)
{
    button_press(code);
    button_press(code1);
}
static void button_up (KEY_W code, KEY_W code1)
{
    button_up(code);
    button_up(code1);
}

static void button_r (KEY_W code, long time = 1)
{
    button_press(code);
    QThread::msleep(time);
    button_up(code);
}
static void button_r (KEY_W code,  KEY_W code1, long time = 1)
{
    button_press(code, code1);
    QThread::msleep(time);
    button_up(code, code1);
}
static void othr_key(const QKeyEvent&e)
{
    switch (e.key())
    {
    case Qt::Key_Home :
        button_r(KEY_HOME);
        break;
    case Qt::Key_End :
        button_r(KEY_END);
        break;
    case Qt::Key_PageUp :
        button_r(KEY_PRIOR);
        break;
    case Qt::Key_PageDown :
        button_r(KEY_NEXT);
        break;
    case Qt::Key_Insert:
        button_r(KEY_INSERT);
        break;
    case Qt::Key_Delete:
        button_r(KEY_DELETE);
        break;
    case Qt::Key_Help:
        button_r(KEY_HELP);
        break;
    case Qt::Key_NumLock :
        button_r(KEY_NUMLOCK);
        break;
    case Qt::Key_ScrollLock :
        button_r(KEY_SCROLL);
        break;
    case Qt::Key_Left:
        button_r(KEY_LEFT);
        break;
    case Qt::Key_Up :
        button_r(KEY_UP);
        break;
    case Qt::Key_Right :
        button_r(KEY_RIGHT);
        break;
    case Qt::Key_Down :
        button_r(KEY_DOWN);
        break;
    case Qt::Key_Print :
        button_r(KEY_PRINT);
        break;
    case Qt::Key_Execute :
        button_r(KEY_EXECUTES);
        break;
    case Qt::Key_Pause :
        button_r(KEY_PAUSE);
        break;
    case Qt::Key_CapsLock :
        button_r(KEY_CAPITAL);
        break;
    case Qt::Key_Space :
        button_r(KEY_SPACE);
        break;
    case Qt::Key_Shift:
        button_r(KEY_SHIFT);
        break;
    case Qt::Key_Control :
    case Qt::Key_Alt :
        button_r(KEY_CONTROL);
        break;
    case Qt::Key_Backspace :
        button_r(KEY_BACK);
        break;
    case Qt::Key_Tab :
        button_r(KEY_TAB);
        break;
    case Qt::Key_Return :
    case Qt::Key_Enter:
        button_r(KEY_RETURN);
        break;
    case Qt::Key_Escape :
        button_r(KEY_ESCAPE);
        break;
    case Qt::Key_Clear :
        button_r(KEY_CLEAR);
        break;
    case Qt::Key_Menu :
        button_r(KEY_MENU);
        break;
    default:
        break;
    }
}
static void letter_key(const QKeyEvent&key)//A~Z字母按键 0x41 0x5a
{
    if ((key.key() >= 0x41) && (key.key() <= 0x5a))
    {
        button_r((KEY_W)key.key());
    }
}
static void number_key(const QKeyEvent&key) //0~9数字按键 0x30 0x39
{
    if ((key.key() >= 0x30) && (key.key() <= 0x39))
    {
        button_r((KEY_W)key.key());
    }
}
static void func_key(const QKeyEvent&key)//F1~F25 0x01000030 0x01000047
{
    if ((key.key() >= 0x01000030) && (key.key() <= 0x01000047))
    {
        uint val = key.key();
        val = F_KEY(val);
        button_r((KEY_W)val);
    }
}

void keyboard_p::excEvent(const QKeyEvent&k)
{
    func_key(k);
    number_key(k);
    letter_key(k);
    othr_key(k);
}
void keyboard_p::excEvent(const QByteArray&by)
{
    QKeyEvent key = toEvent(by);
    excEvent(key);
}
