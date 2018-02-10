#include "fbo_image.h"

#include <QOpenGLShader>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QString>
#include <QImage>
#include <cmath>
#include <QMutex>

#include <QGLWidget>

class fbo_data_p
{
public:
    QOpenGLFramebufferObject *_fbo;//混合输出纹理
    fbo_geomtry _data;
    QMatrix4x4 _color_matrix;
    QVector4D _color_primary;
    fbo_data_p()
    {
    }
};

class fbo_image_p : public render_kernel
{
public:
    QMutex _mutex_;
    QList<fbo_data_p>_fbo_data;
    QList<uint> _pro_id;
    uint _media_texture;//src纹理
    uint _back_video_texture;
    bool _back_video_state;
    QMap <QString, uint> _image_texture;//src纹理
    uint _current_image_id;
    InputType _render_type;

    //! shader
    QString _vertex_sourc;
    QString _fragment_sourc;
    QOpenGLShader *_vertex;
    QOpenGLShader *_fragment;
    QOpenGLShaderProgram  *_program;

    QOpenGLFramebufferObject *_input_fbo;//混合输出纹理
    uint _input_texture;

    uchar* fusion_zone(const int width, float lum, float factor, float gamma, int dir);
    float fusion(const int width, float Pox, float luminance, float gamma, float exponent);

    void _mt(float s, float t);
    void color_matrix(float brightness, float contrast, float hue,
                      float saturation, float matrix[16]);
    QString load_shader(const QString &file);
    void program();
    uint fusion_tex(const QSize &size, float lum, float gamma, float factor, const int dir);
    uint fusion_tex_up(int id, const QSize &size, float lum,float gamma, float factor, const int dir);

    void draw(const uint id);
    void begin(const uint id);
    void end(const uint id);
    void begin_shader();
    void end_shader();

    fbo_image_p()
    {
        _back_video_texture = 0;
        _back_video_state = false;
        _input_fbo = 0;
        _fbo_data.clear ();
        _pro_id.clear ();
        _media_texture = 0;
        _image_texture.clear ();
        _vertex = 0;
        _fragment = 0;
        _program = 0;
        _render_type = INPUT_MEDIA;
    }
    ~fbo_image_p()
    {
        _fbo_data.clear ();
        _pro_id.clear ();
        if (_vertex) delete _vertex;
        if (_fragment) delete _fragment;
        if (_program) delete _program;
    }

};

fbo_image::fbo_image()
{
    d = new fbo_image_p;
}
fbo_image::~fbo_image()
{
    delete d ;
}


//! [1]
void fbo_image::geomtry_set(const uint id, fbo_geomtry geom)
{
    QMutexLocker lock(&d->_mutex_);

    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (d->_pro_id[i] == id)
        {
            memcpy (d->_fbo_data[i]._data._vertex,
                    geom._vertex, sizeof(geom._vertex));
            memcpy (d->_fbo_data[i]._data._coord,
                    geom._coord, sizeof(geom._coord));
            //d->_fbo_data[i]._data = geom;
            return;
        }
    }
}
fbo_geomtry &fbo_image::geomtry(const uint id)
{
    QMutexLocker lock(&d->_mutex_);
    fbo_geomtry geom;

    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (d->_pro_id[i] == id)
        {
            return d->_fbo_data[i]._data;
        }
    }
    return geom;
}
void fbo_image::media_texture(uint tex_id)
{
    QMutexLocker lock(&d->_mutex_);
    d->_input_texture = tex_id;
    d->_back_video_state = false;
}
void fbo_image::media_texture(const QSize size, PixelFormat pixel,
                              DataFormat mode ,uchar *data, bool state)
{
    QMutexLocker lock(&d->_mutex_);

    if (state)
    {
        if (d->_media_texture)
        {
            d->texture_del (d->_media_texture);
        }
        d->_media_texture = texture_gen (size, pixel, mode, DATA_UNSIGNED_BYTE, data);
        texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
        texture_filter(FILTER_LINEAR, FILTER_LINEAR);
    }
    else
    {
        texture_sub (d->_media_texture, size,mode, DATA_UNSIGNED_BYTE, data);
    }
    d->_back_video_state = false;
}

void fbo_image::media_back_texture(const QString &file)
{
    QImage buff, tex;
    if (d->_back_video_texture)
    {
        d->_back_video_state = true;
        return;
    }
    if (file.isEmpty()) return;
    if (buff.load (file))
    {
        tex = QGLWidget::convertToGLFormat (buff);
        d->_back_video_texture =
        texture_gen (tex.size (), PIXEL_RGBA, DATA_RGBA,
                               DATA_UNSIGNED_BYTE, tex.bits ());
        texture_wrap(WRAP_REPEAT, WRAP_REPEAT);
        texture_filter(FILTER_LINEAR, FILTER_LINEAR);
        d->_back_video_state = true;
    }
}
void fbo_image::image_texture(const QString &path)
{
    /*QStringList keys_ = d->_image_texture.keys ();
    for (int i = 0; i < keys_.size (); i++)
    {
        if (keys_[i] == path)
        {
            d->_current_image_id = d->_image_texture.value (path);
            return;
        }
    }*/
    QImage buff, tex;
    if (buff.load (path))
    {
        tex = QGLWidget::convertToGLFormat (buff);
    }
    d->_current_image_id = texture_gen (tex.size (), PIXEL_RGBA, DATA_RGBA,
                                        DATA_UNSIGNED_BYTE,tex.bits ());
    texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
    texture_filter(FILTER_LINEAR, FILTER_LINEAR);
    d->_image_texture.insert (path, d->_current_image_id);
}

void fbo_image::blend_texture(const uint id, float lum, float gamma, float exponent,
                              const int dir)
{
    uint ids = 0;
    QMutexLocker lock(&d->_mutex_);

    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (id == d->_pro_id[i])
        {
            if (dir == FUSION_TOP)
            {
                if (d->_fbo_data[i]._data._tex[0][0][1])
                {
                    d->fusion_tex_up(d->_fbo_data[i]._data._tex[0][0][1],
                            QSize(50, 256),lum, gamma, exponent, dir);
                }
                else
                {
                    ids = d->fusion_tex(QSize(50, 256),lum, gamma, exponent, dir);
                    d->_fbo_data[i]._data._tex[0][0][1] = ids;
                    d->_fbo_data[i]._data._tex[0][1][1] = ids;
                    d->_fbo_data[i]._data._tex[0][2][1] = ids;
                }
            }
            if (dir == FUSION_BOTTOM)
            {
                if (d->_fbo_data[i]._data._tex[2][0][1])
                {
                    d->fusion_tex_up(d->_fbo_data[i]._data._tex[2][0][1],
                            QSize(50, 256),lum, gamma, exponent, dir);
                }
                else
                {
                    ids = d->fusion_tex(QSize(50, 256),lum, gamma, exponent, dir);
                    d->_fbo_data[i]._data._tex[2][0][1] = ids;
                    d->_fbo_data[i]._data._tex[2][1][1] = ids;
                    d->_fbo_data[i]._data._tex[2][2][1] = ids;
                }
            }
            if (dir == FUSION_LEFT)
            {
                if (d->_fbo_data[i]._data._tex[0][0][0])
                {
                    d->fusion_tex_up(d->_fbo_data[i]._data._tex[0][0][0],
                            QSize(256, 50),lum, gamma, exponent, dir);
                }
                else
                {
                    ids = d->fusion_tex(QSize(256, 50),lum, gamma, exponent, dir);
                    d->_fbo_data[i]._data._tex[0][0][0] = ids;
                    d->_fbo_data[i]._data._tex[1][0][0] = ids;
                    d->_fbo_data[i]._data._tex[2][0][0] = ids;
                }
            }
            if (dir == FUSION_RIGHT)
            {
                if (d->_fbo_data[i]._data._tex[0][2][0])
                {
                    d->fusion_tex_up(d->_fbo_data[i]._data._tex[0][2][0],
                            QSize(256, 50),lum, gamma, exponent, dir);
                }
                else
                {
                    ids = d->fusion_tex(QSize(256, 50),lum, gamma, exponent, dir);
                    d->_fbo_data[i]._data._tex[0][2][0] = ids;
                    d->_fbo_data[i]._data._tex[1][2][0] = ids;
                    d->_fbo_data[i]._data._tex[2][2][0] = ids;
                }
            }
            return;
        }
    }
}
void fbo_image::blend_texture(const uint id, const int dir, const QImage &image)
{
    uint ids = 0;
    QMutexLocker lock(&d->_mutex_);

    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (id == d->_pro_id[i])
        {
            if (dir == FUSION_TOP)
            {
                if (d->_fbo_data[i]._data._tex[0][0][1])
                    d->texture_del (d->_fbo_data[i]._data._tex[0][0][1]);
                ids = texture_gen(image.size(), PIXEL_RGBA, DATA_RGBA,
                                  DATA_UNSIGNED_BYTE,(uchar *)image.bits());
                d->_fbo_data[i]._data._tex[0][0][1] = ids;
                d->_fbo_data[i]._data._tex[0][1][1] = ids;
                d->_fbo_data[i]._data._tex[0][2][1] = ids;
            }
            if (dir == FUSION_BOTTOM)
            {
                if (d->_fbo_data[i]._data._tex[2][0][1])
                    d->texture_del (d->_fbo_data[i]._data._tex[2][0][1]);
                ids = texture_gen(image.size(), PIXEL_RGBA, DATA_RGBA,
                                  DATA_UNSIGNED_BYTE,(uchar *)image.bits());
                d->_fbo_data[i]._data._tex[2][0][1] = ids;
                d->_fbo_data[i]._data._tex[2][1][1] = ids;
                d->_fbo_data[i]._data._tex[2][2][1] = ids;
            }
            if (dir == FUSION_LEFT)
            {
                if (d->_fbo_data[i]._data._tex[0][0][0])
                    d->texture_del (d->_fbo_data[i]._data._tex[0][0][0]);
                ids = texture_gen(image.size(), PIXEL_RGBA, DATA_RGBA,
                                  DATA_UNSIGNED_BYTE,(uchar *)image.bits());
                d->_fbo_data[i]._data._tex[0][0][0] = ids;
                d->_fbo_data[i]._data._tex[1][0][0] = ids;
                d->_fbo_data[i]._data._tex[2][0][0] = ids;
            }
            if (dir == FUSION_RIGHT)
            {
                if (d->_fbo_data[i]._data._tex[0][2][0])
                    d->texture_del (d->_fbo_data[i]._data._tex[0][2][0]);
                ids = texture_gen(image.size(), PIXEL_RGBA, DATA_RGBA,
                                  DATA_UNSIGNED_BYTE,(uchar *)image.bits());
                d->_fbo_data[i]._data._tex[0][2][0] = ids;
                d->_fbo_data[i]._data._tex[1][2][0] = ids;
                d->_fbo_data[i]._data._tex[2][2][0] = ids;
            }
            return;
        }
    }
}
void fbo_image::out_texture(const uint id, const QSize size)
{
    QMutexLocker lock(&d->_mutex_);

    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (d->_pro_id[i] == id)
        {
            if (d->_fbo_data[i]._fbo)
            {
                delete d->_fbo_data[i]._fbo;
            }
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::Depth);
            format.setMipmap(true);
            //format.setSamples(4);

            d->_fbo_data[i]._fbo =
                    new QOpenGLFramebufferObject(size, format);
            return;
        }
    }
    d->_pro_id.append (id);

    fbo_data_p ta_p;
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    format.setMipmap(true);
    //format.setSamples(4);
    ta_p._fbo = new QOpenGLFramebufferObject(size, format);

    float _matrix[16] = {0.0};
    d->color_matrix(0, 0, 0, 0,
                 _matrix);
    QMatrix4x4 mat(_matrix);
    ta_p._color_matrix = mat;
    ta_p._color_primary = QVector4D(1, 1, 1, 1);

    d->_fbo_data.append (ta_p);
}
//! [1]

//! [2]
void fbo_image::prepare(const QString &vfile, const QString &mfile)
{
    QMutexLocker lock(&d->_mutex_);
    d->_vertex_sourc = d->load_shader(vfile);
    d->_fragment_sourc = d->load_shader(mfile);
    d->program();
}
//! [2]

//! [4]
//!
InputType &fbo_image::render_type()
{
    return d->_render_type;
}
uint fbo_image::render(const uint id)
{
    QMutexLocker lock(&d->_mutex_);
    int ids = 0;

    if (!d->_pro_id.size ()) return 0;

    if (d->_back_video_state)
    {
        d->_media_texture = d->_back_video_texture;
    }
    else
    {
        //begin_input_fbo();
        //render_input_fbo();
        //end_input_fbo();
        d->_media_texture = d->_input_texture;
    }

    glPushAttrib(GL_VIEWPORT_BIT);
    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (id == d->_pro_id[i])
        {
            ids = i;
            viewport(QRect(0,0, d->_fbo_data[i]._fbo->size().width(),
                           d->_fbo_data[i]._fbo->size().height()));
        }
    }

    matrix_mode(MATRIX_PROJECTION);
    matrix_push();
    load_identity();
    QMatrix4x4 ortho;
    ortho.ortho(0, 1, 0, 1, 0, 1);
    load_matrix(ortho);
    matrix_mode(MATRIX_MODELVIEW);
    matrix_push();
    load_identity();

    d->begin_shader();
    d->begin(id);
    clear();
    d->draw(id);
    d->end(id);
    d->end_shader();

    matrix_mode(MATRIX_PROJECTION);
    matrix_pop();
    matrix_mode(MATRIX_MODELVIEW);
    matrix_pop();
    glPopAttrib();

    return d->_fbo_data[ids]._fbo->texture ();
}
//! [4]

//! [3]
void fbo_image::update_color(const float brightness, const float contras,
                  const float hue, const float saturation)
{
    float _matrix[16] = {0.0};
    d->color_matrix(brightness, contras, hue, saturation,
                 _matrix);
    QMatrix4x4 mat(_matrix);

    QMutexLocker lock(&d->_mutex_);

    if (d->_pro_id.size ())
    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        d->_fbo_data[i]._color_matrix = mat;
    }
}
void fbo_image::update_primary(const float red, const float green,
                          const float blue, const float alpha)
{
    QVector4D mat(red, green, blue, alpha);

    QMutexLocker lock(&d->_mutex_);
    if (d->_pro_id.size ())
    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        d->_fbo_data[i]._color_primary = mat;
    }
}
void fbo_image::update_color(const uint id,
                  const float brightness,
                  const float contras,
                  const float hue,
                  const float saturation)
{
    float _matrix[16] = {0.0};
    d->color_matrix(brightness, contras, hue, saturation,
                 _matrix);
    QMatrix4x4 mat(_matrix);

    QMutexLocker lock(&d->_mutex_);
    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (id == d->_pro_id[i])
        {
            d->_fbo_data[i]._color_matrix= mat;
            return;
        }
    }
}
void fbo_image::update_primary(const uint id,
                    const float red ,
                    const float green ,
                    const float blue ,
                    const float alpha)
{
    QVector4D mat(red, green, blue, alpha);

    QMutexLocker lock(&d->_mutex_);
    for (int i = 0; i < d->_pro_id.size (); i++)
    {
        if (id == d->_pro_id[i])
        {
            d->_fbo_data[i]._color_primary = mat;
            return;
        }
    }
}
//! [3]
//!
uint fbo_image_p::fusion_tex(const QSize &size, float lum,float gamma, float factor, const int dir)
{
    uint id = 0;
    uchar *buff = 0;
    uchar *out = 0;
    switch (dir)
    {
        case FUSION_NO:
        return 0;
        case FUSION_LEFT:
            buff = fusion_zone(size.width(), lum, factor, gamma,  dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int i =0; i < size.height(); i++)
            {
                memcpy(out+i*size.width()*4, buff, size.width()*4);
            }
            id = this->texture_gen(size, PIXEL_RGBA, DATA_RGBA,
                                   DATA_UNSIGNED_BYTE,out);
            texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
            texture_filter(FILTER_LINEAR, FILTER_LINEAR);
            delete out;
            delete buff;
        return id;
        case FUSION_RIGHT:
            buff = fusion_zone(size.width(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int i =0; i < size.height(); i++)
            {
                memcpy(out+i*size.width()*4, buff, size.width()*4);
            }
            id = this->texture_gen(size, PIXEL_RGBA, DATA_RGBA,
                                   DATA_UNSIGNED_BYTE,out);
            texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
            texture_filter(FILTER_LINEAR, FILTER_LINEAR);
            delete out;
            delete buff;
        return id;
        case FUSION_BOTTOM:
            buff = fusion_zone(size.height(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int k = 0; k < size.height(); k++)//top
            {
                for (int l = 0; l < size.width()*4; l+=4)
                {
                    *(out+(k*size.width()*4)+l) = buff[k];
                    *(out+(k*size.width()*4)+l+1) = buff[k];
                    *(out+(k*size.width()*4)+l+2) = buff[k];
                    *(out+(k*size.width()*4)+l+3) = buff[k];
                }
            }

            id = this->texture_gen(size, PIXEL_RGBA, DATA_RGBA,
                              DATA_UNSIGNED_BYTE,out);
            texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
            texture_filter(FILTER_LINEAR, FILTER_LINEAR);
            delete out;
            delete buff;
        return id;
        case FUSION_TOP:
            uchar *buff = fusion_zone(size.height(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int k = 0; k < size.height(); k++)//bottom
            {
                for (int l = 0; l < size.width()*4; l+=4)
                {
                    *(out+(k*size.width()*4)+l) = buff[k];
                    *(out+(k*size.width()*4)+l+1) = buff[k];
                    *(out+(k*size.width()*4)+l+2) = buff[k];
                    *(out+(k*size.width()*4)+l+3) = buff[k];
                }
            }
            id = this->texture_gen(size, PIXEL_RGBA, DATA_RGBA,
                                   DATA_UNSIGNED_BYTE,out);
            texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
            texture_filter(FILTER_LINEAR, FILTER_LINEAR);
            delete out;
            delete buff;
        return id;
    }
    return 0;

}
uint fbo_image_p::fusion_tex_up(int id, const QSize &size, float lum,float gamma, float factor, const int dir)
{
    uchar *buff = 0;
    uchar *out = 0;
    switch (dir)
    {
        case FUSION_NO:
        return 0;
        case FUSION_LEFT:
            buff = fusion_zone(size.width(), lum, factor, gamma,  dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int i =0; i < size.height(); i++)
            {
                memcpy(out+i*size.width()*4, buff, size.width()*4);
            }
            texture_sub(id, size, DATA_RGBA, DATA_UNSIGNED_BYTE, out);
            delete out;
            delete buff;
        return id;
        case FUSION_RIGHT:
            buff = fusion_zone(size.width(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int i =0; i < size.height(); i++)
            {
                memcpy(out+i*size.width()*4, buff, size.width()*4);
            }
            texture_sub(id, size, DATA_RGBA, DATA_UNSIGNED_BYTE, out);
            delete out;
            delete buff;
        return id;
        case FUSION_BOTTOM:
            buff = fusion_zone(size.height(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int k = 0; k < size.height(); k++)//top
            {
                for (int l = 0; l < size.width()*4; l+=4)
                {
                    *(out+(k*size.width()*4)+l) = buff[k];
                    *(out+(k*size.width()*4)+l+1) = buff[k];
                    *(out+(k*size.width()*4)+l+2) = buff[k];
                    *(out+(k*size.width()*4)+l+3) = buff[k];
                }
            }

            texture_sub(id, size, DATA_RGBA, DATA_UNSIGNED_BYTE, out);
            delete out;
            delete buff;
        return id;
        case FUSION_TOP:
            uchar *buff = fusion_zone(size.height(), lum, factor,gamma, dir);
            out = new uchar[size.height() *size.width()*4];
            memset(out, 255, size.height() *size.width()*4);
            for (int k = 0; k < size.height(); k++)//bottom
            {
                for (int l = 0; l < size.width()*4; l+=4)
                {
                    *(out+(k*size.width()*4)+l) = buff[k];
                    *(out+(k*size.width()*4)+l+1) = buff[k];
                    *(out+(k*size.width()*4)+l+2) = buff[k];
                    *(out+(k*size.width()*4)+l+3) = buff[k];
                }
            }
            texture_sub(id, size, DATA_RGBA, DATA_UNSIGNED_BYTE, out);
            delete out;
            delete buff;
        return id;
    }
    return 0;

}
//! [4]
void fbo_image_p::draw(const uint id)
{
    polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_FILL);
    glColor3f (1, 1, 1);
    uint state = 0;

    for (int i = 0; i < _pro_id.size (); i++)
    {
        if (id == _pro_id[i])
        {
            fbo_geomtry geom = _fbo_data[i]._data;
            _program->setUniformValue ( "color_matrix", _fbo_data[i]._color_matrix);
            _program->setUniformValue ( "color_primary", _fbo_data[i]._color_primary);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    state =0;
                    if (geom._tex[i][j][0] && (_render_type & INPUT_BLEND))
                    {
                        activate_texture(MULTI_TEXTURE_1);
                        enable (ENABLE_TEXTURE_2D);
                        bind_texture (BIND_TEXTURE_2D, geom._tex[i][j][0]);
                        _program->setUniformValue ( "tex_dst_1", 1);
                        state |=1<< 1;
                    }
                    if (geom._tex[i][j][1]&& (_render_type & INPUT_BLEND))
                    {
                        activate_texture(MULTI_TEXTURE_2);
                        enable (ENABLE_TEXTURE_2D);
                        bind_texture (BIND_TEXTURE_2D, geom._tex[i][j][1]);
                        _program->setUniformValue ( "tex_dst_2", 2);
                        state |=1<< 2;
                    }
                    _program->setUniformValue ( "tex_dst_state", state);

                    glBegin (GL_QUADS);
                    multi_texCoord_2f(MULTI_TEXTURE_0,
                                   geom._coord[i][j].x (),
                                   geom._coord[i][j].y ());
                    if (_render_type & INPUT_BLEND)
                        _mt(0.01f,0.01f);
                    glVertex3f (geom._vertex[i][j].x (),
                                geom._vertex[i][j].y (),
                                geom._vertex[i][j].z ());

                    multi_texCoord_2f(MULTI_TEXTURE_0,
                                   geom._coord[i+1][j].x (),
                                   geom._coord[i+1][j].y ());
                    if (_render_type & INPUT_BLEND)
                    _mt( 0.01f, 0.99f);
                    glVertex3f (geom._vertex[i+1][j].x (),
                            geom._vertex[i+1][j].y (),
                            geom._vertex[i+1][j].z ());

                    multi_texCoord_2f(MULTI_TEXTURE_0,
                                   geom._coord[i+1][j+1].x (),
                                   geom._coord[i+1][j+1].y ());
                    if (_render_type & INPUT_BLEND)
                    _mt(0.99f, 0.99f);
                    glVertex3f (geom._vertex[i+1][j+1].x (),
                            geom._vertex[i+1][j+1].y (),
                            geom._vertex[i+1][j+1].z ());

                    multi_texCoord_2f(MULTI_TEXTURE_0,
                                   geom._coord[i][j+1].x (),
                                   geom._coord[i][j+1].y ());
                    if (_render_type & INPUT_BLEND)
                    _mt(0.99f, 0.01f);
                    glVertex3f (geom._vertex[i][j+1].x (),
                            geom._vertex[i][j+1].y (),
                            geom._vertex[i][j+1].z ());
                    glEnd();

                    if (geom._tex[i][j][0]&& (_render_type & INPUT_BLEND))
                    {
                        activate_texture(MULTI_TEXTURE_1);
                        enable (ENABLE_TEXTURE_2D);
                        bind_texture (BIND_TEXTURE_2D, 0);
                    }
                    if (geom._tex[i][j][1]&& (_render_type & INPUT_BLEND))
                    {
                        activate_texture(MULTI_TEXTURE_2);
                        enable (ENABLE_TEXTURE_2D);
                        bind_texture (BIND_TEXTURE_2D, 0);
                    }
                }
            }
        }
    }
}

void fbo_image_p::begin(const uint id)
{
    for (int i = 0; i < _pro_id.size (); i++)
    {
        if (id == _pro_id[i])
        {
            _fbo_data[i]._fbo->bind ();

            enable(ENABLE_BLEND);
            enable(ENABLE_ALPHA_TEST);
            alpha_func(ALPHA_GREATER ,0.1);
            blend_func(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            disable(DISABLE_DEPTH_TEST);
            depth_mask(false);

            activate_texture(MULTI_TEXTURE_0);
            enable (ENABLE_TEXTURE_2D);
            if (_render_type & INPUT_MEDIA)
            {
                bind_texture (BIND_TEXTURE_2D, _media_texture);
            }
            else if (_render_type & INPUT_IMAGE)
            {
                bind_texture (BIND_TEXTURE_2D, _current_image_id);
            }
            _program->setUniformValue ( "tex_src", 0);
        }
    }
}
void fbo_image_p::end(const uint id)
{
    for (int i = 0; i < _pro_id.size (); i++)
    {
        if (id == _pro_id[i])
        {
            activate_texture(MULTI_TEXTURE_0);
            disable (DISABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, 0);

            disable(DISABLE_BLEND);
            disable(DISABLE_ALPHA_TEST);
            enable(ENABLE_DEPTH_TEST);
            depth_mask(true);
            _fbo_data[i]._fbo->release ();
        }
    }

}
void fbo_image_p::begin_shader()
{
    _program->bind();
}
void fbo_image_p::end_shader()
{
    _program->release ();
}
//! [4]

void fbo_image_p::program()
{
    _program = new QOpenGLShaderProgram();

    _vertex = new QOpenGLShader(QOpenGLShader::Vertex, _program);
    CHECK( _vertex->compileSourceCode( _vertex_sourc ),
           _vertex->log( ));
    _fragment = new QOpenGLShader(QOpenGLShader::Fragment, _program);
    CHECK( _fragment->compileSourceCode( _fragment_sourc ),
           _fragment->log( ) );

    CHECK( _program->addShader( _vertex ),
           _program->log( ) );
    CHECK( _program->addShader( _fragment ),
           _program->log( ) );

    CHECK( _program->link( ), _program->log( ) );

}
QString fbo_image_p::load_shader(const QString &file)
{
    QString code ;
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        QTextStream in(&f);
        code = in.readAll();
        f.flush();
        f.close();
    }

    return code;
}
static void matmult(const float A[16], const float B[16], float result[16])
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            result[r * 4 + c] =
                    A[r * 4 + 0] * B[0 * 4 + c]
                    + A[r * 4 + 1] * B[1 * 4 + c]
                    + A[r * 4 + 2] * B[2 * 4 + c]
                    + A[r * 4 + 3] * B[3 * 4 + c];
        }
    }
}

void fbo_image_p::color_matrix(float brightness, float contrast, float hue,
                  float saturation, float matrix[16])
{
    const float wr = 0.3086f;
    const float wg = 0.6094f;
    const float wb = 0.0820f;

    // Brightness and Contrast
    float b = (brightness < 0.0f ? brightness : 4.0f * brightness) + 1.0f;
    float c = -contrast;
    float BC[16] = {
        b, 0.0f, 0.0f, 0.0f,
        0.0f,    b, 0.0f, 0.0f,
        0.0f, 0.0f,    b, 0.0f,
        c,    c,    c, 1.0f
    };
    // Saturation
    float s = saturation + 1.0f;
    float S[16] = {
        (1.0f - s) * wr + s, (1.0f - s) * wg    , (1.0f - s) * wb    , 0.0f,
        (1.0f - s) * wr    , (1.0f - s) * wg + s, (1.0f - s) * wb    , 0.0f,
        (1.0f - s) * wr    , (1.0f - s) * wg    , (1.0f - s) * wb + s, 0.0f,
        0.0f,                0.0f,                0.0f, 1.0f
    };
    // Hue
    float n = 1.0f / sqrtf(3.0f);       // normalized hue rotation axis: sqrt(3) * (1 1 1)
    float h = hue * M_PI;               // hue rotation angle
    float hc = cosf(h);
    float hs = sinf(h);
    float H[16] = {     // conversion of angle/axis representation to matrix representation
                        n * n * (1.0f - hc) + hc    , n * n * (1.0f - hc) - n * hs, n * n * (1.0f - hc) + n * hs, 0.0f,
                        n * n * (1.0f - hc) + n * hs, n * n * (1.0f - hc) + hc    , n * n * (1.0f - hc) - n * hs, 0.0f,
                        n * n * (1.0f - hc) - n * hs, n * n * (1.0f - hc) + n * hs, n * n * (1.0f - hc) + hc    , 0.0f,
                        0.0f,                         0.0f,                         0.0f, 1.0f
                  };

    /* I
        float I[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        */

    // matrix = B * C * S * H
    float T[16];
    matmult(BC, S, T);
    matmult(T, H, matrix);
}

uchar* fbo_image_p::fusion_zone(
        const int width, float lum, float factor, float gamma, int dir)
{
    float j=0;
    int k = 0;
    uchar * buff= 0;

    switch (dir)
    {
        case FUSION_LEFT://left
        {
            buff = new uchar[width *4];

            for (k =0, j=0.0;
                 k < (width *4);
                 k+=4, j+= (256.0/(float)width))
            {
                int val = (int)(256.*fusion(width, j, lum, gamma, factor));
                if (val > 255) val = 255;

                buff[k] = val;
                buff[k+1] = val;
                buff[k+2] = val;
                buff[k+3] = val;

            }
            return buff;
        }
        case FUSION_TOP://top
        {
            buff = new uchar[width];

            for (k =0, j=0.0;
                 k < (width);
                 k++, j+= (256.0/(float)width))
            {
                int val = (int)(256.*fusion(width, j, lum, gamma, factor));
                if (val > 255) val = 255;
                buff[k] = val;
            }
            return buff;
        }
        break;
        case FUSION_RIGHT://right
        {
            buff = new uchar[width*4];

            for (k =0, j=255.0;
                 (k < width*4);
                 k+=4, j-= (256./(float)width))
            {
                int val = (int)(256.*fusion(width, j, lum, gamma, factor));
                buff[k] = val;
                buff[k+1] = val;
                buff[k+2] = val;
                buff[k+3] = val;
            }
            return buff;
        }
        case FUSION_BOTTOM://bottom
        {
            buff = new uchar[width];

            for (k =0, j=255.0;
                 (k < width);
                 k++, j-= (256./(float)width))
            {
                int val = (int)(256.*fusion(width, j, lum, gamma, factor));
                if (val > 255) val = 255;
                buff[k] = val;
            }
            return buff;
        }
        break;
        default:
        break;
    }
    return 0;

}

float fbo_image_p::fusion(const int width, float Pox, float luminance, float gamma, float exponent)
{
    float a = Pox / width;
    float blend = (a < 0.5) ? (luminance * pow(2.0 * a, exponent))
            : 1.0 - (1.0 - luminance) * pow(2.0 * (1.0 - a), exponent);

    // gamma correction (for each of the 3 channels)
    blend = pow(blend, 1.0 / gamma);
    return blend;
}
void fbo_image_p::_mt(float s, float t)
{
    multi_texCoord_2f(MULTI_TEXTURE_1, s, t);
    multi_texCoord_2f(MULTI_TEXTURE_2, s, t);
}

