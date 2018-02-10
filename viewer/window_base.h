#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#include <QWindow>

class window_base_p;
class window_base : public QWindow
{
    Q_OBJECT
    window_base_p *d;
public:
    explicit window_base(QWindow *parent = 0);
    explicit window_base(const QSurfaceFormat &format,
                         QWindow *parent = 0);
    ~window_base();

protected://渲染控制
    void render_start();
    void render_stop();

public://上下文操作
    void wait();
    void wake_all();
    void wake_one();

    void lock();
    void unlock();

    QOpenGLContext *context();
    void swap_buffers();
    void done_current();
    void make_current();

protected://线程内部执行
    virtual void draw(const QRect &r);
    void render_activa(bool loop = false);//若是true则自动运行，否则运行一次

public slots:
    void update() ;
protected://执行UI事件，传入线程中
    bool event(QEvent *e);

friend class context_thread;
};

#endif // WINDOW_BASE_H
