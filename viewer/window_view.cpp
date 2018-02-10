#include "window_view.h"
#include <QTime>
#include <QGLWidget>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QResizeEvent>
#include <QDebug>
#include <QQuaternion>
#include <QMatrix4x4>
class window_view_p
{
public:
    window_view *view;
    bool initialized;
    bool visible;
    camera *cameras;
    QRect viewport;

    QPoint startPan;
    QPoint lastPan;
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers;
    QTime logTime;
    QTime enterTime;
    QTime lastFrameTime;

    Mouse3DKeys panning;
    QVector3D startPos;
    QVector3D lastPos;
    QRectF rectangle;
    bool _s_tex_backgroud;
    uint _t_backgroud_id;
    QColor _c_backgroud;
    SelectMode _m_select;

    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    QMatrix4x4 inverseEyeMatrix;

    inline void logEnter(const char *message)
    {
        int ms = logTime.elapsed();
        enterTime.start();
        int sinceLast = lastFrameTime.restart();
        qDebug("LOG[%d:%02d:%02d.%03d]: ENTER: %s (%d ms since last enter)",
               ms / 3600000, (ms / 60000) % 60,
               (ms / 1000) % 60, ms % 1000, message, sinceLast);
    }

    inline void logLeave(const char *message)
    {
        int ms = logTime.elapsed();
        int duration = enterTime.elapsed();
        qDebug("LOG[%d:%02d:%02d.%03d]: LEAVE: %s (%d ms elapsed)",
               ms / 3600000, (ms / 60000) % 60,
               (ms / 1000) % 60, ms % 1000, message, duration);
    }
public:
    window_view_p(window_view *parent)
        : view(parent)
        , initialized(false)
        , visible(false)
        , viewport()
    {
        cameras = new camera(parent);;

        lastPos = startPos = QVector3D(0,0,0);
        panning = Key_No;
        startPan = QPoint(-1, -1);
        lastPan = QPoint(-1, -1);
        panModifiers = Qt::NoModifier;
        QObject::connect(cameras, SIGNAL(changed_projection()),
                         parent, SLOT(changed_camera()));
        QObject::connect(cameras, SIGNAL(changed_view()),
                         parent, SLOT(changed_camera()));

        logTime.start();
        lastFrameTime.start();
        _s_tex_backgroud = false;
        _t_backgroud_id = 0;
        _c_backgroud = QColor(0,0,0);
        _m_select = SELECT_NO;
    }
    ~window_view_p()
    {
    }
};

window_view::window_view(QWindow *parent):
    window_base(parent)
{
    d = new window_view_p(this);
}
window_view::window_view(const QSurfaceFormat& format, QWindow *parent)
    : window_base(format, parent)
{
    d = new window_view_p(this);
}
window_view::~window_view()
{
    delete d;
}
camera *window_view::cameras() const
{
    return d->cameras;
}

void window_view::cameras(camera *value)
{
    if (d->cameras == value)
        return;

    disconnect(d->cameras, SIGNAL(changed_projection()),
               this, SLOT(changed_camera()));
    disconnect(d->cameras, SIGNAL(changed_view()),
               this, SLOT(changed_camera()));

    d->cameras = value;

    connect(d->cameras, SIGNAL(changed_projection()),
            this, SLOT(changed_camera()));
    connect(d->cameras, SIGNAL(changed_view()),
            this, SLOT(changed_camera()));

    changed_camera();
}
void window_view::changed_camera()
{
    update();
}

void window_view::draw(const QRect &r)
{
    if (!d->initialized)
    {
        initializeGL();
        d->initialized = true;
    }
    if (d->viewport != r)
    {
        resizeGL(r.width(), r.height());
        d->viewport = r;
    }

    //d->logEnter("draw_begin");
    QMatrix4x4 lookAt = d->cameras->matrix_model_view();
    d->modelViewMatrix = lookAt;
    d->projectionMatrix = d->cameras->matrix_projection(d->cameras->field_view ());
    d->inverseEyeMatrix = lookAt.inverted();

    matrix_mode(MATRIX_MODELVIEW);
    load_matrix(d->modelViewMatrix);

    matrix_mode(MATRIX_PROJECTION);
    load_matrix(d->projectionMatrix);

    paintGL();

    if (!d->rectangle.isNull())
        draw_select_rect();

    draw_backgroud();
    //d->logLeave("draw_end");
}
void window_view::initializeGL()
{

}
void window_view::resizeGL(int w, int h)
{
    viewport(QRect(0, 0, w, h));
}
void window_view::paintGL()
{
}

void window_view::select_begin(const QPoint &)
{

}
void window_view::select_begin(const QRectF &)
{

}
void window_view::select_move(const QPoint &)

{

}
void window_view::select_end()

{

}
void window_view::mousePressEvent(QMouseEvent *e)
{
    if (e->button()== Qt::RightButton)
    {
        //在融合中禁止使用3D场景
        /*d->panning = Key_Rotations;
        d->lastPan = d->startPan = e->pos();
        d->startEye = d->cameras->eye();
        d->startCenter = d->cameras->center();
        d->startUpVector = d->cameras->upVector();
        d->panModifiers = e->modifiers();*/

        select_end();
        this->mode_select () = SELECT_NO;
    }
    if (d->panning == Key_Move) return;

    if ((e->modifiers () != Qt::AltModifier)&&
        (this->mode_select () != SELECT_NO) &&
        (d->panning == Key_Group))
    {
        this->mode_select () = SELECT_NO;
        d->panning = Key_Move;
        return;
    }
    if ((this->mode_select () != SELECT_NO) &&
            (d->panning != Key_Group)) return;

    if (e->button() == Qt::LeftButton)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            d->panning = Key_RectSelect;
            d->rectangle = QRect(e->pos(), e->pos());
        }
        else
        {
            if (this->mode_select () != SELECT_NO)
            {
                d->panning = Key_Group;
            }
            else if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_POINT;
                d->panning = Key_Group;
            }
            else
            {
                d->panning = Key_Select;
            }

            d->lastPan = d->startPan = e->pos();
            select_begin(e->pos());
        }
    }

    update();
    QWindow::mousePressEvent(e);
}

void window_view::mouseMoveEvent(QMouseEvent *e)
{
    if (d->panning == Key_Rotations)
    {
        QPoint delta = e->pos() - d->startPan;
        if (e->modifiers() == d->panModifiers)
        {
            d->cameras->eye(d->startEye);
            d->cameras->center(d->startCenter);
            d->cameras->upVector(d->startUpVector);
        }
        else
        {
            d->startPan = d->lastPan;
            delta = e->pos() - d->startPan;
            d->startEye = d->cameras->eye();
            d->startCenter = d->cameras->center();
            d->startUpVector = d->cameras->upVector();
            d->panModifiers = e->modifiers();
        }
        d->lastPan = e->pos();
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(delta.y() * -60);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(delta.x(), delta.y());
        else
            rotate(delta.x(), delta.y());
    }
    else if (d->panning == Key_Select)
    {
        //计算移动量
        select_move(e->pos());
    }
    else if (d->panning == Key_RectSelect)
    {
        d->rectangle.setBottomRight(e->pos());
        //计算框选
    }
    else if ((d->panning == Key_Move) )
    {
        //计算框选移动
        select_move(e->pos());
    }

    update();
    QWindow::mouseMoveEvent(e);
}

void window_view::mouseReleaseEvent(QMouseEvent *e)
{
    if ((d->panning == Key_Rotations) && e->button() == Qt::RightButton)
    {
        d->panning = Key_No;
    }
    else if ((d->panning == Key_RectSelect) && e->button() == Qt::LeftButton)
    {
        d->rectangle = d->rectangle.normalized();
        d->rectangle.setX(e->pos().x());
        d->rectangle.setY(e->pos().y());
        float w = 4.0f / (float)size().width();
        float h = 4.0f / (float)size().height();
        d->rectangle.setWidth(w);
        d->rectangle.setHeight(h);

        select_begin(d->rectangle);
        d->rectangle.setRect(0,0,0,0);
        d->panning = Key_Move;
    }
    else if ((d->panning == Key_Select) && e->button() == Qt::LeftButton)
    {
        d->panning = Key_No;
        select_end();
    }
    else if ((d->panning == Key_Move))
    {
        d->panning = Key_No;
        select_end();
    }
    else if ((d->panning == Key_Group) && ((this->mode_select () == SELECT_NO)))
    {
        d->panning = Key_No;
        select_end();
    }

    update();
    QWindow::mouseReleaseEvent(e);
}

void window_view::mouseDoubleClickEvent(QMouseEvent *e)
{
    QWindow::mouseDoubleClickEvent(e);
}

#ifndef QT_NO_WHEELEVENT
void window_view::wheelEvent(QWheelEvent *e)
{
    //wheel(e->delta());
    QWindow::wheelEvent(e);
}
#endif

void window_view::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_H:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_NO;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_V:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_NO;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_B:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_NO;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_S:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_SURFACK;
            }
            else QWindow::keyPressEvent(e);
        break;
    }
}
void window_view::keyPressEvent(QKeyEvent *e)
{
    camera *_camera;
    float sep;

    switch (e->key())
    {
        case Qt::Key_H:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_HORIZONTAL;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_V:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_VERTICAL;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_B:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_BLOCK;
            }
            else QWindow::keyPressEvent(e);
        break;
        case Qt::Key_S:
            if (e->modifiers () == Qt::AltModifier)
            {
                this->mode_select () = SELECT_SURFACK;
            }
            else QWindow::keyPressEvent(e);
        break;
/*
        case Qt::Key_Left:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0)
            {
                pan(-10, 0);
            }
            else if ((e->modifiers() & Qt::ControlModifier) != 0)
            {
                _camera = this->cameras();
                sep = _camera->eye_separation();
                sep -= (sep / 10.0f);
                if (sep < 0.0f)
                    sep = 0.0f;
                _camera->eye_separation(sep);
                e->accept();
                return;
            }
            else
            {
                rotate(-10, 0);
            }
        }
        break;

        case Qt::Key_Right:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0)
            {
                pan(10, 0);
            }
            else if ((e->modifiers() & Qt::ControlModifier) != 0)
            {
                _camera = this->cameras();
                sep = _camera->eye_separation();
                sep += (sep / 10.0f);
                _camera->eye_separation(sep);
                e->accept();
                return;
            }
            else
            {
                rotate(10, 0);
            }
        }
        break;

        case Qt::Key_Up:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, -10);
            else
                rotate(0, -10);
        }
        break;

        case Qt::Key_Down:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(-120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, 10);
            else
                rotate(0, 10);
        }
        break;*/
        default:
            QWindow::keyPressEvent(e);
            break;
    }

}
void window_view::wheel(int delta)
{
    if (1)
    {
        //Use field-of view as zoom (much like a traditional camera)
        float scale = qAbs(delta_view(delta, delta).x());
        if (delta < 0)
            scale = -scale;
        if (scale >= 0.0f)
            scale += 1.0f;
        else
            scale = 1.0f / (1.0f - scale);
        float fov = d->cameras->field_view();
        if (fov != 0.0f)
            d->cameras->field_view(d->cameras->field_view() / scale);
        else
            d->cameras->size_view(d->cameras->size_view() / scale);
    }
    else
    {
        // enable this to get wheel navigation that actually zooms by moving the
        // camera back, as opposed to making the angle of view wider.
        QVector3D viewVector= cameras()->eye() - cameras()->center();
        float zoomMag = viewVector.length();
        float zoomIncrement = -float(delta) / 100.0f;
        if (!qFuzzyIsNull(zoomIncrement))
        {
            zoomMag += zoomIncrement;
            if (zoomMag < 1.0f)
                zoomMag = 1.0f;
            cameras()->eye(cameras()->center() + zoomMag*viewVector.normalized());
        }
    }

}

// Pan left/right/up/down without rotating about the object.
void window_view::pan(int deltax, int deltay)
{
    QPointF delta = delta_view(deltax, deltay);
    QVector3D t = d->cameras->translation(delta.x(), -delta.y(), 0.0f);

    // Technically panning the eye left should make the object appear to
    // move off to the right, but this looks weird on-screen where the user
    // actually thinks they are picking up the object and dragging it rather
    // than moving the eye.  We therefore apply the inverse of the translation
    // to make it "look right".
    d->cameras->eye(d->cameras->eye() - t);
    d->cameras->center(d->cameras->center() - t);
}

// Rotate about the object being viewed.
void window_view::rotate(int deltax, int deltay)
{
    int rotation = d->cameras->rotation_screen();
    if (rotation == 90 || rotation == 270) {
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    float anglex = deltax * 90.0f / width();
    float angley = deltay * 90.0f / height();
    QQuaternion q = d->cameras->pan(-anglex);
    q *= d->cameras->tilt(-angley);
    d->cameras->center_rotate(q);
}

QPointF window_view::delta_view(int deltax, int deltay) const
{
    int w = width();
    int h = height();
    bool scaleToWidth;
    float scaleFactor, scaleX, scaleY;
    QSizeF viewSize = d->cameras->size_view();
    if (w >= h) {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = true;
        else
            scaleToWidth = false;
    } else {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = false;
        else
            scaleToWidth = true;
    }
    int rotation = d->cameras->rotation_screen();
    if (rotation == 90 || rotation == 270) {
        scaleToWidth = !scaleToWidth;
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    if (scaleToWidth) {
        scaleFactor = 2.0f / viewSize.width();
        scaleX = scaleFactor * float(h) / float(w);
        scaleY = scaleFactor;
    } else {
        scaleFactor = 2.0f / viewSize.height();
        scaleX = scaleFactor;
        scaleY = scaleFactor * float(w) / float(h);
    }
    return QPointF(deltax * scaleX / w, deltay * scaleY / h);
}
QMatrix4x4 window_view::matrix_combined() const
{
    if (!d)
        return QMatrix4x4();
    return d->projectionMatrix * d->modelViewMatrix;
}

static inline float _stablize_value(float value)
{
    return (qAbs(value) >= 0.00001f) ? value : 0.0f;
}
static inline QMatrix4x4 _stablize_matrix(const QMatrix4x4 &m)
{
    return QMatrix4x4(_stablize_value(m(0, 0)),
                      _stablize_value(m(0, 1)),
                      _stablize_value(m(0, 2)),
                      _stablize_value(m(0, 3)),
                      _stablize_value(m(1, 0)),
                      _stablize_value(m(1, 1)),
                      _stablize_value(m(1, 2)),
                      _stablize_value(m(1, 3)),
                      _stablize_value(m(2, 0)),
                      _stablize_value(m(2, 1)),
                      _stablize_value(m(2, 2)),
                      _stablize_value(m(2, 3)),
                      _stablize_value(m(3, 0)),
                      _stablize_value(m(3, 1)),
                      _stablize_value(m(3, 2)),
                      _stablize_value(m(3, 3)));
}

QMatrix4x4 window_view::matrix_world() const
{
    return _stablize_matrix
        (d->inverseEyeMatrix * d->modelViewMatrix);
}
QMatrix3x3 window_view::matrix_normal() const
{
    if (!d)
        return QMatrix3x3();
    return d->modelViewMatrix.normalMatrix();
}
QMatrix4x4 window_view::matrix_projection() const
{
    return d->projectionMatrix;
}
QMatrix4x4 window_view::matrix_mode_view() const
{
    return d->modelViewMatrix;
}
void window_view::screen_start(bool upward)
{
    QMatrix4x4 ortho;
    matrix_mode(MATRIX_PROJECTION);
    matrix_push();
    load_identity();
      if (upward)
          ortho.ortho(0, width(), 0, height(), 0.0, -1.0);
      else
          ortho.ortho(0, width(), height(), 0, 0.0, -1.0);

     load_matrix(ortho);

    matrix_mode(MATRIX_MODELVIEW);
    matrix_push();
    load_identity();
}
void window_view::screen_stop()
{
    matrix_mode(MATRIX_PROJECTION);
    matrix_pop();

    matrix_mode(MATRIX_MODELVIEW);
    matrix_pop();
}

void window_view::draw_select_rect()
{
  screen_start();
  polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_LINE);
  line_size(2);
  Line lint;
  lint.p0.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p0.vertex = QVector3D(d->rectangle.left(), d->rectangle.top(), 0);
  lint.p1.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p1.vertex = QVector3D(d->rectangle.left(), d->rectangle.bottom(), 0);
  line_draw(lint);

  lint.p0.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p0.vertex = QVector3D(d->rectangle.left(), d->rectangle.top(), 0);
  lint.p1.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p1.vertex = QVector3D(d->rectangle.right(), d->rectangle.top(), 0);
  line_draw(lint);

  lint.p0.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p0.vertex = QVector3D(d->rectangle.right(), d->rectangle.top(), 0);
  lint.p1.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p1.vertex = QVector3D(d->rectangle.right(), d->rectangle.bottom(), 0);
  line_draw(lint);

  lint.p0.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p0.vertex = QVector3D(d->rectangle.right(), d->rectangle.bottom(), 0);
  lint.p1.color = QColor(0.0, 0.0, 0.3f, 0.3f);
  lint.p1.vertex = QVector3D(d->rectangle.left(), d->rectangle.bottom(), 0);
  line_draw(lint);

  screen_stop();
  line_size(1);;
}

bool &window_view::show_texture_backgroud()
{
    return d->_s_tex_backgroud;
}
uint &window_view::texture_backgroud_id()
{
    return d->_t_backgroud_id;
}
QColor &window_view::color_backgroud ()
{
    return d->_c_backgroud;
}
SelectMode &window_view::mode_select()
{
    return d->_m_select;
}
void window_view::draw_backgroud()
{
    if (show_texture_backgroud() && texture_backgroud_id())
    {
        matrix_mode(MATRIX_PROJECTION);
        matrix_push();
        load_identity();
        QMatrix4x4 ortho;
        ortho.ortho(-1, 1, -1, 1, -1, 1);
        load_matrix(ortho);
        matrix_mode(MATRIX_MODELVIEW);
        matrix_push();
        load_identity();

        enable (ENABLE_TEXTURE_2D);
        bind_texture (BIND_TEXTURE_2D, texture_backgroud_id ());
        Quads a;
        a.t0.p0.color = QColor(255,255,255);
        a.t0.p0.coord = QVector2D(0,0);
        a.t0.p0.vertex = QVector3D(-1, -1, 0);

        a.t0.p1.color = QColor(255,255,255);
        a.t0.p1.coord = QVector2D(1,0);
        a.t0.p1.vertex = QVector3D(1, -1, 0);

        a.t0.p2.color = QColor(255,255,255);
        a.t0.p2.coord = QVector2D(0,1);
        a.t0.p2.vertex = QVector3D(-1, 1, 0);

        a.t1.p0 = a.t0.p2;
        a.t1.p1 = a.t0.p1;

        a.t1.p2.color = QColor(255,255,255);
        a.t1.p2.coord = QVector2D(1,1);
        a.t1.p2.vertex = QVector3D(1, 1, 0);

        quads_draw(a);

        bind_texture (BIND_TEXTURE_2D, 0);
        disable (DISABLE_TEXTURE_2D);

        matrix_mode(MATRIX_PROJECTION);
        matrix_pop();
        matrix_mode(MATRIX_MODELVIEW);
        matrix_pop();
    }
    else /// 颜色背景
    {
        clear_color (color_backgroud ());
    }
}
QImage window_view::load_image(const QString &name)
{
    QImage buff;
    if (buff.load (name))
    {
        //tex = QGLWidget::convertToGLFormat (buff);
        return buff;
    }
    return QImage();
}
