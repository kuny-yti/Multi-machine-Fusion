#ifndef FBO_IMAGE_H
#define FBO_IMAGE_H
#include "type_define.h"
#include "render_kernel.h"
#define CHECK(a,b) if (!a) qDebug() << (b.toUtf8 ());

typedef enum
{
    INPUT_MEDIA = 1,
    INPUT_IMAGE = 1<<1,
    INPUT_BLEND = 1<<2,
    INPUT_MEDIA_BLEND = INPUT_MEDIA | INPUT_BLEND,
    INPUT_IMAGE_BLEND =INPUT_IMAGE | INPUT_BLEND,
}InputType;

class fbo_geomtry
{
public:
    QVector3D _vertex[4][4];
    QVector3D _coord[4][4];
    uint _tex[3][3][2];
    fbo_geomtry()
    {
        memset (&_tex, 0, sizeof(uint)*3*3*2);
        memset (&_vertex, 0, sizeof(QVector3D)*4*4);
        memset (&_coord, 0, sizeof(QVector3D)*4*4);
    }
};
class fbo_image_p;
class fbo_image : public render_kernel
{
private:
    fbo_image_p *d;
    void draw(const uint id);

public:
    fbo_image();
    ~fbo_image();

    //! [1]
    void out_texture(const uint id, const QSize size);
    //! [1]
    //! [2]
    void geomtry_set(const uint id, fbo_geomtry geom);
    fbo_geomtry &geomtry(const uint id);
    void media_texture(const QSize size, PixelFormat pixel, DataFormat mode, uchar *data, bool state);
    void media_texture(uint tex_id);
    void image_texture(const QString &);
    void media_back_texture(const QString &tex);
    void blend_texture(const uint id, float lum, float gamma,float exponent,
                       const int dir);
    void blend_texture(const uint id, const int dir, const QImage &image);
    //! [2]

    //! [3]
    void prepare(const QString &vfile, const QString &mfile);
    //! [3]

    //! [5]
    uint render(const uint id);
    InputType &render_type();
    //! [5]

    //! [4]
    void update_color(const float brightness ,
                      const float contrast,
                      const float hue ,
                      const float saturation);
    void update_primary(const float red ,
                        const float green,
                        const float blue,
                        const float alpha);

    void update_color(const uint id,
                      const float brightness= 0,
                      const float contrast= 0,
                      const float hue= 0,
                      const float saturation= 0);
    void update_primary(const uint id,
                        const float red = 1,
                        const float green = 1,
                        const float blue = 1,
                        const float alpha = 1);
    //! [4]

};

#endif // FBO_IMAGE_H
