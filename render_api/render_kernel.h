#ifndef RENDER_KERNEL_H
#define RENDER_KERNEL_H
#include "type_define.h"
#include <QSizeF>
#include <QOpenGLFunctions>

class render_kernel
{
public:
    render_kernel();
    virtual ~render_kernel();

    virtual void primitive_draw (const Primitive&);
    virtual void primitive_draw (const arrayPrimitive&);

    virtual void line_draw (const Line&);
    virtual void line_draw (const arrayLine&);

    virtual void triangle_draw (const Triangle&);
    virtual void triangle_draw (const arrayTriangle&);

    virtual void quads_draw (const Quads &);
    virtual void quads_draw (const arrayQuads&);

    static QVector3D screen_pos(const QPoint &p, float &zNear);
    static void ray_pos(const QPoint &p, QVector3D &_near, QVector3D &_far);

    virtual void polygon_mode(const SurfaceMode, const PolygonMode);
    virtual void point_size(const float);
    virtual void line_size(const float);

    virtual void matrix_mode(const MatrixMode&);
    virtual void load_matrix(const QMatrix4x4&);
    virtual void view_scissor(const QRect &rect);
    virtual void enable(Enable);
    virtual void depth_func(DepthFunc);
    virtual void depth_mask(bool );
    virtual void depth_range(float, float);
    virtual void disable(Disable);
    virtual void viewport(const QRect &r);

    virtual void matrix_push();
    virtual void matrix_pop();
    virtual void load_identity();
    virtual void load_identity(const MatrixMode &m);
    virtual void bind_texture(const Bind , const uint );
    virtual void clear_color(const QColor &);
    virtual void clear();

    virtual void blend_func(const BlendFunc, const BlendFunc);
    virtual void alpha_func(const AlphaFunc func ,const float val);

    static uint texture_gen(const QSize &size, PixelFormat iformat,
                                DataFormat format,DataType type, uchar *data);
    static void texture_filter(TextureParameter min, TextureParameter mag);
    static void texture_wrap(TextureParameter s, TextureParameter t);
    static void texture_sub(uint id, const QSize &size, DataFormat format, DataType type, uchar *data);
    static void texture_del(uint id);
    static void check_error(const char* op = 0);

    virtual void activate_texture(const TextureMulti texture);
    virtual void multi_texCoord_2f(const TextureMulti target, float s, float t);
    virtual void window_pos_3f(const float x,const float y,const float z);
    virtual void window_pos_2i(const uint x, const uint y);
    void *map_buffer(GLenum target, GLenum access);
    void unmap_buffer(GLenum target);

};

#endif // RENDER_KERNEL_H
