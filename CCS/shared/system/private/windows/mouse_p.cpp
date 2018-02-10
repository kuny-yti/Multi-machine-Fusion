#include "mouse_p.h"
#include <windows.h>
#include <QMouseEvent>

mouse_p::mouse_p()
{
}
mouse_p::~mouse_p()
{
}

QByteArray mouse_p::toByte(bool v, const QMouseEvent &mu, const QWheelEvent &wh)
{
    QByteArray out;

    QDataStream in(&out, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_1);

    if(v)
    {
        in << (qint32)1;
        in << (qint32)mu.type();
        in << (qint32)mu.button() ;
        in << (qint32)mu.buttons();
        in << (qint32)mu.modifiers();
        in << mu.pos();
        in << mu.globalPos();
        in << mu.localPos();
        in << mu.windowPos();
        in << mu.screenPos();
    }
    else
    {
        in << (qint32)2;
        in << (qint32)wh.type();
        in << (qint32)wh.buttons();
        in << (qint32)wh.delta();
        in << (qint32)wh.modifiers();
        in << (qint32)wh.orientation();
        in << (qint32)0;
        in << wh.angleDelta();
        in << wh.globalPos();
        in << wh.globalPosF();
        in << wh.pixelDelta();
        in << wh.pos();
        in << wh.posF();
    }

    return out;
}
QMouseEvent mouse_p::toEventMouse(const QByteArray &byte)
{
    QByteArray data = byte;
    qint32 temp_v = 0;
    qint32 temp_type = 0;
    qint32 temp_button = 0;
    qint32 temp_buttons = 0;
    qint32 temp_modifiers = 0;
    QPoint pos;
    QPoint globalPos;
    QPointF localPos;
    QPointF windowPos;
    QPointF screenPos;

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_1);

    in >> temp_v;
    if (temp_v == 1)
    {
        in >> temp_type;
        in >> temp_button;
        in >> temp_buttons;
        in >> temp_modifiers;
        in >> pos;
        in >> globalPos;
        in >> localPos;
        in >> windowPos;
        in >> screenPos;

    }
    return QMouseEvent((QMouseEvent::Type)temp_type, localPos,
                       (Qt::MouseButton)temp_button, (Qt::MouseButtons)temp_buttons,
                       (Qt::KeyboardModifiers)temp_modifiers);
}
QWheelEvent mouse_p::toEventWheel(const QByteArray &byte)
{
    QByteArray data = byte;
    qint32 temp_v = 0;
    qint32 temp_type = 0;
    qint32 temp_button = 0;
    qint32 temp_buttons = 0;
    qint32 temp_modifiers = 0;
    QPoint pos;
    QPoint globalPos;
    QPointF localPos;
    QPointF windowPos;
    QPointF screenPos;

    qint32 wh_type= 0;
    qint32 wh_buttons= 0;
    qint32 wh_delta= 0;
    qint32 wh_modifiers= 0;
    qint32 wh_orientation= 0;
    qint32 wh_phase= 0;
    QPoint wh_angleDelta ;
    QPoint wh_globalPos;
    QPointF wh_globalPosF;
    QPoint wh_pixelDelta;
    QPoint wh_pos;
    QPointF wh_posF;

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_1);

    in >> temp_v;
    if (temp_v == 2)
    {
        in >> temp_type;
        in >> temp_button;
        in >> temp_buttons;
        in >> temp_modifiers;
        in >> pos;
        in >> globalPos;
        in >> localPos;
        in >> windowPos;
        in >> screenPos;

        in >> wh_type;
        in >> wh_buttons;
        in >> wh_delta;
        in >> wh_modifiers;
        in >> wh_orientation;
        in >> wh_phase;
        in >> wh_angleDelta;

        in >> wh_globalPos;
        in >> wh_globalPosF;
        in >> wh_pixelDelta;
        in >> wh_pos;
        in >> wh_posF;
    }

    return QWheelEvent(wh_pos, wh_delta, (Qt::MouseButtons)wh_buttons,
                      (Qt::KeyboardModifiers)wh_modifiers,
                      (Qt::Orientation)wh_orientation);
}
//
void mouse_p::excEvent(const QMouseEvent &k)
{
    switch ((int)k.type())
    {
        case QEvent::MouseMove:
            SetCursorPos(k.screenPos().x(), k.screenPos().y());
            //mouse_event(MOUSEEVENTF_MOVE, k.screenPos().x(), k.screenPos().y(), 0, 0);
            break;
        case QEvent::MouseButtonPress:
        switch (k.buttons())
        {
            case Qt::LeftButton:
                mouse_event(MOUSEEVENTF_LEFTDOWN, k.pos().x(), k.pos().y(), 0, 0);
                break;
            case Qt::RightButton:
                mouse_event(MOUSEEVENTF_RIGHTDOWN, k.pos().x(), k.pos().y(), 0, 0);
                break;
            case Qt::MidButton:
                mouse_event(MOUSEEVENTF_MIDDLEDOWN, k.pos().x(), k.pos().y(), 0, 0);
                break;
            default:
                break;
        }
            break;
        case QEvent::MouseButtonRelease:
        switch (k.button())
        {
            case Qt::LeftButton:
                mouse_event(MOUSEEVENTF_LEFTUP, k.pos().x(), k.pos().y(), 0, 0);
                break;
            case Qt::RightButton:
                mouse_event(MOUSEEVENTF_RIGHTUP, k.pos().x(), k.pos().y(), 0, 0);
                break;
            case Qt::MidButton:
                mouse_event(MOUSEEVENTF_MIDDLEUP, k.pos().x(), k.pos().y(), 0, 0);
                break;
            default:
                break;
        }
            break;
        case QEvent::MouseButtonDblClick:
            break;
        case QEvent::Wheel:
            break;
    }
}
void mouse_p::excEvent(const QWheelEvent&k)
{
    switch ((int)k.type())
    {
        case QEvent::Wheel:
            mouse_event(MOUSEEVENTF_WHEEL, k.pos().x(), k.pos().y(), k.delta(), 0);
            break;
    }

}
void mouse_p::excEvent(const QByteArray&by)
{
    QMouseEvent mouse = toEventMouse(by);
    QWheelEvent wheel = toEventWheel(by);
    excEvent(mouse);
    excEvent(wheel);
}
