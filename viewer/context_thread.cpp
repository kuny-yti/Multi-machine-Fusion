#include "context_thread.h"
#include "window_base.h"

class context_thread_p
{
public :
    window_base*  _base;
    QRect         _viewport ;
    volatile bool _render;
    volatile bool _loop;

public :
    context_thread_p(window_base *_base_):
        _base(_base_),
        _render(true),
        _viewport(),
        _loop(false)
    {

    }
};

context_thread::context_thread(window_base *_base_, QObject *parent) :
    QThread(parent),
    d(new context_thread_p(_base_))
{
}
void context_thread::viewport(const QRect &r)
{
    if (d->_loop)
    {
        d->_base->lock();
        d->_viewport = r;
        d->_base->unlock();
    }
    else
    {
        d->_viewport = r;
    }
    d->_base->render_activa(d->_loop);
}
void context_thread::stop()
{
    if (d->_loop)
    {
        d->_base->lock();
        d->_render = false;
        d->_loop = false;
        d->_base->unlock();
    }
    else
    {
        d->_render = false;
        d->_loop = false;
    }

}
bool context_thread::loop()
{
    return d->_loop;
}
void context_thread::loop(bool mode)
{
    if (d->_loop)
    {
        d->_base->lock();
        d->_loop = mode;
        d->_base->unlock();
    }
    else
        d->_loop = mode;
}
void context_thread::run()
{
    while (d->_render)
    {
        d->_base->lock();
        d->_base->make_current();

        d->_base->draw(d->_viewport);

        d->_base->swap_buffers();
        d->_base->done_current();
        d->_base->unlock();

        if (!d->_loop)
        {
            d->_base->lock();
            d->_base->wait();
            d->_base->unlock();
        }
    }
}
