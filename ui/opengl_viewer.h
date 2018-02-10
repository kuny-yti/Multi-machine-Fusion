#ifndef OPENGL_VIEWER_H
#define OPENGL_VIEWER_H

#include "window_view.h"
//#include "video_output.h"
#include "ctrl.h"

class opengl_viewer_p;
class opengl_viewer:
        public window_view,
        private ctrl
{
    Q_OBJECT
public:
    opengl_viewer(QWindow *parent = 0);
    ~opengl_viewer();

protected:
    virtual void initializeGL();
    virtual void paintGL();

    virtual void select_begin(const QPoint &p);
    virtual void select_begin(const QRectF &p);
    virtual void select_move(const QPoint &p);
    virtual void select_end();

    virtual bool event(QEvent *e);

public:
    void activate_desktop(const QImage &image, const QPointF &pos);
    void activate_movie(uchar *buff, const QSize &size);
    MediaPlayMode media_play_mode();
private:
    virtual void command(const int& cmd);

    bool setp_run();
    void display_mode(const uint val);
    void device_coord_update(const QVector<QVector<QRectF> > &coord);
    void computer_coord_update(const QVector<QVector<QRectF> > &coord,
                               const QVector<QVector<QRectF> > &p =
            QVector<QVector<QRectF> >());
private:
    opengl_viewer_p *d;
};

#endif // OPENGL_VIEWER_H
