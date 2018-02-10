#include "gui.h"
#include <QEvent>
#include <QTouchEvent>
#include <QPainter>

GUI::GUI(QWidget *parent) :
    QWidget(parent)
{
    text.clear();
    setMouseTracking(true);
}
void GUI::paintEvent(QPaintEvent *)
{
    QPainter k;

    k.begin(this);
    for (int i = 0; i < text.size(); i++)
    {
        k.drawText(20, 20*i, text[i]);
    }
    k.end();
}
bool GUI::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent* mous = (QMouseEvent*)event;
        emit mouse(mous);

        text.clear();
        text << (tr("mouse event:"));
        text << (tr("    pos:x=%1, y=%2").arg(mous->pos().x()).arg(mous->pos().y()));
        text << (tr("    globalPos:x=%1, y=%2").arg(mous->globalPos().x()).arg(mous->globalPos().y()));
        text << (tr("    localPos:x=%1, y=%2").arg(mous->localPos().x()).arg(mous->localPos().y()));
        text << (tr("    windowPos:x=%1, y=%2").arg(mous->windowPos().x()).arg(mous->windowPos().y()));
        text << (tr("    screenPos:x=%1, y=%2").arg(mous->screenPos().x()).arg(mous->screenPos().y()));
        text << (tr("    button: %1").arg(mous->button()));
        text << (tr("    buttons: %1").arg(mous->buttons()));

        text << (tr("    modifiers: %1").arg(mous->modifiers()));
        text << (tr("    type: %1").arg(mous->type()));
        text << (tr("    isAccepted: %1").arg(mous->isAccepted()));
        break;
    }
    #ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
    {
        QWheelEvent* mous = (QWheelEvent*)event;
        emit wheel(mous);

        text.clear();
        text << (tr("wheel event:"));
        text << (tr("    pixelDelta:x=%1, y=%2").arg(mous->pixelDelta().x()).arg(mous->pixelDelta().y()));
        text << (tr("    angleDelta:x=%1, y=%2").arg(mous->angleDelta().x()).arg(mous->angleDelta().y()));
        text << (tr("    delta: %1").arg(mous->delta()));
        text << (tr("    orientation: %1").arg(mous->orientation()));
        text << (tr("    pos:x=%1, y=%2").arg(mous->pos().x()).arg(mous->pos().y()));
        text << (tr("    globalPos:x=%1, y=%2").arg(mous->globalPos().x()).arg(mous->globalPos().y()));
        text << (tr("    buttons: %1").arg(mous->buttons()));
        text << (tr("    phase: %1").arg(mous->phase()));

        text << (tr("    modifiers: %1").arg(mous->modifiers()));
        text << (tr("    type: %1").arg(mous->type()));
        text << (tr("    isAccepted: %1").arg(mous->isAccepted()));
        break;
    }
    #endif
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent* mous = (QKeyEvent*)event;
        emit key((QKeyEvent*)event);

        text.clear();
        text << (tr("key event:"));
        text << (tr("    key: %1").arg(mous->key()));
        text << (tr("    text: %1").arg(mous->text()));
        text << (tr("    count: %1").arg(mous->count()));

        text << (tr("    modifiers: %1").arg(mous->modifiers()));
        text << (tr("    type: %1").arg(mous->type()));
        text << (tr("    isAccepted: %1").arg(mous->isAccepted()));
        break;
    }
    #ifndef QT_NO_DRAGANDDROP
    case QEvent::Drop:
        emit drop((QDropEvent*) event);
        break;
    case QEvent::DragEnter:
        emit dragEnter((QDragEnterEvent*) event);
        break;
    case QEvent::DragMove:
        emit dragMove((QDragMoveEvent*) event);
        break;
    case QEvent::DragLeave:
        emit dragLeave((QDragLeaveEvent*) event);
        break;
    #endif

    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
    {
        emit touch((QTouchEvent *)event);

        break;
    }
    default:
        return QWidget::event(event);
    }
    this->update();
    return true;
}
