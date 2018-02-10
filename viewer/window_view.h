#ifndef WINDOW_VIEW_H
#define WINDOW_VIEW_H
#include "window_base.h"
#include "render_kernel.h"
#include "camera.h"

class window_view_p;
class window_view :public window_base, public render_kernel
{
    Q_OBJECT
    window_view_p *d;
private:
    void wheel(int delta);
    void pan(int deltax, int deltay);
    void rotate(int deltax, int deltay);
    void draw_select_rect();
protected:
    virtual void draw(const QRect &r);
public:
    explicit window_view(QWindow *parent = 0);
    explicit window_view(const QSurfaceFormat &format, QWindow *parent = 0);
    ~window_view();

    camera *cameras() const;
    void cameras(camera *cam);

    QMatrix4x4 matrix_combined() const;
    QMatrix3x3 matrix_normal() const;
    QMatrix4x4 matrix_world() const;
    QMatrix4x4 matrix_projection() const;
    QMatrix4x4 matrix_mode_view() const;

    void screen_start(bool upward = false);
    void screen_stop();
    static QImage load_image(const QString &name);

    bool &show_texture_backgroud();
    uint &texture_backgroud_id();
    QColor &color_backgroud ();
    SelectMode &mode_select();

signals:
    void quit();
public:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

protected:
    virtual void select_begin(const QPoint &p);
    virtual void select_begin(const QRectF &p);
    virtual void select_move(const QPoint &p);
    virtual void select_end();

    virtual void create_begin(const QPoint &){}
    virtual void create_end(const QPoint &){}

    void draw_backgroud();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e);
#endif
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    QPointF delta_view(int deltax, int deltay) const;
    QPointF delta_view(const QPoint &delta) const
        { return delta_view(delta.x(), delta.y()); }

private slots:
    void changed_camera();
};

#endif // WINDOW_VIEW_H
