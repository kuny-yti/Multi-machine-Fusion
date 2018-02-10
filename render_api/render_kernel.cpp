
#include "render_kernel.h"
#include <qopenglfunctions.h>
#include <gl/glu.h>
#include <QSizeF>
#include <QMatrix4x4>

void (QOPENGLF_APIENTRYP ActivateTexture)(GLenum target);
void (QOPENGLF_APIENTRYP MultiTexCoord2f)(GLenum target, GLfloat s, GLfloat t);
void (QOPENGLF_APIENTRYP WindowPos2i)(GLuint x, GLuint y);
void (QOPENGLF_APIENTRYP WindowPos3f)(GLfloat x, GLfloat y, GLfloat z);
void *(QOPENGLF_APIENTRYP MapBuffer)(GLenum target, GLenum access);
void (QOPENGLF_APIENTRYP UnmapBuffer)(GLenum target);


render_kernel::render_kernel()
{
    MultiTexCoord2f= 0;
    WindowPos2i = 0;
    WindowPos3f = 0;
    ActivateTexture = 0;
    UnmapBuffer = 0;
    MapBuffer = 0;
}
render_kernel::~render_kernel()
{
}

void render_kernel::primitive_draw (const Primitive &p)
{
    glBegin(GL_POINTS);
        glTexCoord2d(p.coord.x (), p.coord.y ());
        glNormal3d (p.normal.x (), p.normal.y (), p.normal.z ());
        glColor3d (p.color.redF (), p.color.greenF (), p.color.blueF ());
        glVertex3d(p.vertex.x (), p.vertex.y (), p.vertex.z ());
    glEnd();
}
void render_kernel::primitive_draw (const arrayPrimitive& arr)
{
    for (int i = 0; i < arr.size (); i++)
    {
        primitive_draw (arr[i]);
    }
}

void render_kernel::line_draw (const Line& t)
{
    glBegin(GL_LINES);
        glTexCoord2d(t.p0.coord.x (), t.p0.coord.y ());
        glNormal3d (t.p0.normal.x (), t.p0.normal.y (), t.p0.normal.z ());
        glColor3d (t.p0.color.redF (), t.p0.color.greenF (), t.p0.color.blueF ());
        glVertex3d(t.p0.vertex.x (), t.p0.vertex.y (), t.p0.vertex.z ());

        glTexCoord2d(t.p1.coord.x (), t.p1.coord.y ());
        glNormal3d (t.p1.normal.x (), t.p1.normal.y (), t.p1.normal.z ());
        glColor3d (t.p1.color.redF (), t.p1.color.greenF (), t.p1.color.blueF ());
        glVertex3d(t.p1.vertex.x (), t.p1.vertex.y (), t.p1.vertex.z ());
    glEnd();
}
void render_kernel::line_draw (const arrayLine &arr)
{
    for (int i = 0; i < arr.size (); i++)
    {
        line_draw (arr[i]);
    }
}

void render_kernel::triangle_draw (const Triangle &t)
{
    glBegin(GL_POLYGON);
        glTexCoord2d(t.p0.coord.x (), t.p0.coord.y ());
        glNormal3d (t.p0.normal.x (), t.p0.normal.y (), t.p0.normal.z ());
        glColor3d (t.p0.color.redF (), t.p0.color.greenF (), t.p0.color.blueF ());
        glVertex3d(t.p0.vertex.x (), t.p0.vertex.y (), t.p0.vertex.z ());

        glTexCoord2d(t.p1.coord.x (), t.p1.coord.y ());
        glNormal3d (t.p1.normal.x (), t.p1.normal.y (), t.p1.normal.z ());
        glColor3d (t.p1.color.redF (), t.p1.color.greenF (), t.p1.color.blueF ());
        glVertex3d(t.p1.vertex.x (), t.p1.vertex.y (), t.p1.vertex.z ());

        glTexCoord2d(t.p2.coord.x (), t.p2.coord.y ());
        glNormal3d (t.p2.normal.x (), t.p2.normal.y (), t.p2.normal.z ());
        glColor3d (t.p2.color.redF (), t.p2.color.greenF (), t.p2.color.blueF ());
        glVertex3d(t.p2.vertex.x (), t.p2.vertex.y (), t.p2.vertex.z ());
    glEnd();
}
void render_kernel::triangle_draw (const arrayTriangle &arr)
{
    for (int i = 0; i < arr.size (); i++)
    {
        triangle_draw (arr[i]);
    }
}

void render_kernel::quads_draw (const Quads &q )
{
    triangle_draw(q.t0);
    triangle_draw(q.t1);
}
void render_kernel::quads_draw (const arrayQuads &arr)
{
    for (int i = 0; i < arr.size (); i++)
    {
        quads_draw (arr[i]);
    }
}

void render_kernel::polygon_mode(const SurfaceMode sur, const PolygonMode draw)
{
    glPolygonMode(sur, draw);
}
void render_kernel::point_size(const float v)
{
    glPointSize (v);
}
void render_kernel::line_size(const float v)
{
    glLineWidth (v);
}

void render_kernel::ray_pos(const QPoint &p, QVector3D &_near, QVector3D &_far)
{
    double o1x, o1y, o1z;
    double o2x, o2y, o2z;

    double mv[16] = {0};
    double prj[16] = {0};
    //更新矩阵
    glGetDoublev(GL_MODELVIEW_MATRIX , mv);
    glGetDoublev(GL_PROJECTION_MATRIX, prj);

    int viewPort[4] = {0};
    ::glGetIntegerv(GL_VIEWPORT, viewPort);

    gluUnProject(p.x(), (viewPort[3] - p.y() - 1), 0.0, mv, prj, viewPort, &o1x, &o1y, &o1z);
    gluUnProject(p.x(), (viewPort[3] - p.y() - 1), 1.0, mv, prj, viewPort, &o2x, &o2y, &o2z);

    _near = QVector3D(o1x, o1y, o1z);
    _far = QVector3D(o2x, o2y, o2z);
}
QVector3D render_kernel::screen_pos(const QPoint &p, float &zNear)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    ::glEnable(GL_DEPTH_TEST);

    ::glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    ::glGetDoublev(GL_PROJECTION_MATRIX, projection);
    ::glGetIntegerv(GL_VIEWPORT, viewport);
    winX = (float)p.x();
    winY = (float)viewport[3] - (float)p.y();
    if ((zNear < 0.00001))
    {
        ::glReadPixels(p.x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        zNear = winZ;
    }

    gluUnProject(winX, winY, zNear, modelview, projection, viewport, &posX, &posY, &posZ);

    return QVector3D(posX, posY, posZ);
}
void render_kernel::view_scissor(const QRect &r)
{
    glScissor(r.x(), r.y(), r.width(), r.height());
}
void render_kernel::matrix_mode(const MatrixMode &m)
{
    glMatrixMode(m);
}
void render_kernel::load_matrix(const QMatrix4x4 &matrix)
{
    glLoadMatrixf(reinterpret_cast<const GLfloat *>
                  (matrix.constData()));
}
void render_kernel::enable(Enable fun)
{
    glEnable(fun);
}
void render_kernel::depth_func(DepthFunc fun)
{
    glDepthFunc(fun);
}
void render_kernel::depth_mask(bool b)
{
    glDepthMask(b);
}
void render_kernel::depth_range(float a, float b)
{
#if defined(QT_OPENGL_ES)
    glDepthRangef(a, b);
#else
    glDepthRange(a, b);
#endif
}
void render_kernel::disable(Disable v)
{
    glDisable(v);
}
void render_kernel::viewport(const QRect &r)
{
    glViewport(r.x(), r.y(), r.width(), r.height());
}
void render_kernel::matrix_push()
{
    glPushMatrix();
}
void render_kernel::matrix_pop()
{
    glPopMatrix();
}
void render_kernel::load_identity()
{
    glLoadIdentity();
}
void render_kernel::load_identity(const MatrixMode &m)
{
    matrix_mode(m);
    glLoadIdentity();
}
void render_kernel::bind_texture(const Bind b, const uint id)
{
    ::glBindTexture (b, id);
}
void render_kernel::clear_color(const QColor &color)
{
    ::glClearColor (color.redF (),
                  color.greenF (),
                  color.blueF (),
                  color.alphaF ());
}
void render_kernel::clear()
{
    ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
void render_kernel::blend_func(const BlendFunc src,const BlendFunc dst)
{
    ::glBlendFunc(src, dst);
}
void render_kernel::alpha_func(const AlphaFunc func ,const float val)
{
    ::glAlphaFunc (func, val);
}
uint render_kernel::texture_gen(const QSize &size, PixelFormat iformat, DataFormat format , DataType type, uchar *data)
{
    GLuint id;
    if (size.isEmpty()) return 0;
    ::glEnable(GL_TEXTURE_2D);
    ::glGenTextures(1, &id);
    ::glBindTexture(GL_TEXTURE_2D, id);
    ::glTexImage2D(GL_TEXTURE_2D, 0, iformat,
               size.width(),
               size.height(), 0,
               format,
               type, data);

    return id;
}
void render_kernel::texture_wrap(TextureParameter s, TextureParameter t)
{
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}
void render_kernel::texture_filter(TextureParameter min, TextureParameter mag)
{
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
}
void render_kernel::texture_sub(uint id, const QSize &size,
                           DataFormat format,DataType type, uchar *data)
{
    ::glEnable(GL_TEXTURE_2D);
    ::glBindTexture(GL_TEXTURE_2D, id);
    ::glTexSubImage2D(GL_TEXTURE_2D, 0 , 0, 0, size.width(), size.height(),
                    format, type, data);
}
void render_kernel::texture_del(uint id)
{
    ::glDeleteTextures(1, &id);
}
void render_kernel::check_error(const char* op)
{
    GLenum error = ::glGetError();
    if (error == GL_NO_ERROR)
        return;
    qWarning("GL error %s (%#x): %s", op, error, ::glGetString(error));
}
void render_kernel::activate_texture(const TextureMulti texture)
{
    if (!ActivateTexture)
    {
        ActivateTexture = reinterpret_cast<void (QOPENGLF_APIENTRYP)
                (GLenum)>
                (QOpenGLContext::currentContext()->getProcAddress("glActiveTextureARB"));
    }

    ActivateTexture(texture);
}
void render_kernel::multi_texCoord_2f(const TextureMulti target, float s, float t)
{
    if (!MultiTexCoord2f)
    {
        MultiTexCoord2f = reinterpret_cast<void (QOPENGLF_APIENTRYP)
                (GLenum , GLfloat , GLfloat )>
                (QOpenGLContext::currentContext()->getProcAddress("glMultiTexCoord2fARB"));
    }

    MultiTexCoord2f(target, s, t);
}
void render_kernel::window_pos_2i(const uint x, const uint y)
{
    if (!WindowPos2i)
    {
        WindowPos2i = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint , GLuint)>
                (QOpenGLContext::currentContext()->getProcAddress("glWindowPos2iARB"));
    }

    WindowPos2i(x, y);
}
void render_kernel::window_pos_3f(const float x, const float y, const float z)
{
    if (!WindowPos3f)
    {
        WindowPos3f = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLfloat , GLfloat, GLfloat)>
                (QOpenGLContext::currentContext()->getProcAddress("glWindowPos3fARB"));
    }

    WindowPos3f(x, y, z);
}
void *render_kernel::map_buffer(GLenum target, GLenum access)
{
    if (!MapBuffer)
    {
        MapBuffer =
                reinterpret_cast<void *(QOPENGLF_APIENTRYP)
                (GLenum,GLenum)>(QOpenGLContext::currentContext()
                          ->getProcAddress("glMapBufferARB"));
    }
    if (!MapBuffer)
    {
        MapBuffer =
                reinterpret_cast<void *(QOPENGLF_APIENTRYP)
                (GLenum,GLenum)>(QOpenGLContext::currentContext()
                          ->getProcAddress("glMapBufferEXT"));
    }
    return MapBuffer(target, access);
}
void render_kernel::unmap_buffer(GLenum target)
{
    if (!UnmapBuffer)
    {
        UnmapBuffer =
                reinterpret_cast<void (QOPENGLF_APIENTRYP)
                (GLenum)>(QOpenGLContext::currentContext()
                          ->getProcAddress("glUnmapBufferARB"));
    }
    if (!UnmapBuffer)
    {
        UnmapBuffer =
                reinterpret_cast<void (QOPENGLF_APIENTRYP)
                (GLenum)>(QOpenGLContext::currentContext()
                          ->getProcAddress("glUnmapBufferEXT"));
    }
    UnmapBuffer(target);
}

