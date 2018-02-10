#include "window_base.h"
#include "context_thread.h"
#include <QMutex>
#include <QWaitCondition>
#include <QOpenGLContext>

class window_base_p
{
public:
    window_base*    _base;
    context_thread  _thread;
    volatile bool   _is_run;

    QSurfaceFormat  _format;
    QOpenGLContext* _context;
    QMutex          _mutex;
    QWaitCondition  _condition;
public:
    window_base_p(window_base *_base_):
        _thread(_base_),
        _base(_base_),
        _format(),
        _context(0),
        _is_run(false),
        _mutex(),
        _condition()
    {

    }

    void ensureContext()
    {
        if (!_context)
        {
            _context = new QOpenGLContext();
            _context->setFormat(_format);

            _context->create();
        }
    }
};
window_base::window_base(QWindow *parent) :
    QWindow(parent),
    d(new window_base_p(this))
{
    d->_format = QSurfaceFormat();
    d->_format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(d->_format);
}
window_base::window_base(const QSurfaceFormat &format,
                         QWindow *parent):
    QWindow(parent),
    d(new window_base_p(this))
{
    d->_format = format;
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(d->_format);
}
window_base::~window_base()
{
    render_stop();
    if (d)
        delete d;
}

void window_base::draw(const QRect &r)
{
}
QOpenGLContext *window_base::context()
{
    return d->_context;
}
void window_base::render_start ()
{
    if (!d->_is_run)
    {
        d->_thread.start();
        d->_context->moveToThread (&d->_thread);
        d->_is_run = true;
    }
}
void window_base::render_stop()
{
    if (d->_is_run)
    {
        d->_is_run = false;
        d->_thread.stop();
        wake_one();
        d->_thread.wait();
        d->_context->moveToThread (QThread::currentThread ());
    }
}

void window_base::render_activa(bool loop)
{
    d->_thread.loop(loop);
    wake_one();
}
void window_base::update()
{
    if (!d->_thread.loop())
        render_activa();
}
void window_base::wait()
{
    d->_condition.wait(&d->_mutex);
}
void window_base::wake_all()
{
    d->_condition.wakeAll();
}
void window_base::wake_one()
{
    d->_condition.wakeOne();
}
void window_base::lock()
{
    d->_mutex.lock();
}
void window_base::unlock()
{
    d->_mutex.unlock();
}
void window_base::swap_buffers()
{
    d->_context->swapBuffers(this);
}
void window_base::done_current()
{
    d->_context->doneCurrent();
}
void window_base::make_current()
{
    d->_context->makeCurrent(this);
}

bool window_base::event(QEvent *e)
{
    switch ((int)e->type())
    {
    case QEvent::Close:
    case QEvent::Quit:
        d->_thread.stop();
        break;
    case QEvent::Expose:
    case QEvent::Show:
        d->ensureContext();
        if (!d->_is_run) render_start();
        render_activa(d->_thread.loop());
        break;
    case QEvent::Resize:
        d->_thread.viewport (geometry());
        break;
    default:
        break;
    }
    return QWindow::event(e);;
}
