#ifndef CONTEXT_THREAD_H
#define CONTEXT_THREAD_H

#include <QThread>
#include <QKeyEvent>

class window_base ;
class context_thread_p;
class context_thread : public QThread
{
    Q_OBJECT
    context_thread_p *d;
public:
    explicit context_thread(window_base*_base_, QObject *parent = 0);
public:
    void stop();
    void loop(bool mode);
    bool loop();
    void viewport(const QRect &r);
protected:
    void run();
friend class window_base;
};

#endif // CONTEXT_THREAD_H
