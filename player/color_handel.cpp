#include "color_handel.h"
#include "input_handel.h"
#include "dispatch.h"
#include "dbg.h"

#include <QFile>

static const float full_tex_coords[2][4][2] =
{
    { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }
};
void color_handel::draw_quad(float x, float y, float w, float h,
        const float tex_coords[2][4][2],
        const float more_tex_coords[4][2])
{
    const float (*my_tex_coords)[4][2] = (tex_coords ? tex_coords : full_tex_coords);

    glBegin(GL_QUADS);
    glTexCoord2f(my_tex_coords[0][0][0], my_tex_coords[0][0][1]);
    multi_texCoord_2f(MULTI_TEXTURE_1, my_tex_coords[1][0][0], my_tex_coords[1][0][1]);
    if (more_tex_coords) {
        multi_texCoord_2f(MULTI_TEXTURE_2, more_tex_coords[0][0], more_tex_coords[0][1]);
    }
    glVertex2f(x, y);
    glTexCoord2f(my_tex_coords[0][1][0], my_tex_coords[0][1][1]);
    multi_texCoord_2f(MULTI_TEXTURE_1, my_tex_coords[1][1][0], my_tex_coords[1][1][1]);
    if (more_tex_coords) {
        multi_texCoord_2f(MULTI_TEXTURE_2, more_tex_coords[1][0], more_tex_coords[1][1]);
    }
    glVertex2f(x + w, y);
    glTexCoord2f(my_tex_coords[0][2][0], my_tex_coords[0][2][1]);
    multi_texCoord_2f(MULTI_TEXTURE_1, my_tex_coords[1][2][0], my_tex_coords[1][2][1]);
    if (more_tex_coords) {
        multi_texCoord_2f(MULTI_TEXTURE_2, more_tex_coords[2][0], more_tex_coords[2][1]);
    }
    glVertex2f(x + w, y + h);
    glTexCoord2f(my_tex_coords[0][3][0], my_tex_coords[0][3][1]);
    multi_texCoord_2f(MULTI_TEXTURE_1, my_tex_coords[1][3][0], my_tex_coords[1][3][1]);
    if (more_tex_coords) {
        multi_texCoord_2f(MULTI_TEXTURE_2, more_tex_coords[3][0], more_tex_coords[3][1]);
    }
    glVertex2f(x, y + h);
    glEnd();
}

color_handel::color_handel(input_handel *in):
    input(in)
{
    _is_init_opengl = false;
    _frame[0] = video_frame();
    _frame[1] = video_frame();
    _last_params[0] = parameters();
    _last_params[1] = parameters();
    _prg[0] = 0;
    _prg[1] = 0;

    _fbo = 0;
    memset(_tex, 0, sizeof(_tex));
}
color_handel::~color_handel()
{
    deinit(0);
    deinit(1);
}
void color_handel::handel(const int index, const video_frame &frame)
{
    if (!input) return;

    _frame[index] = frame;
    if (!_prg[index] ||
            !is_compatible(index, dispatch::parameters(), frame))
    {
        deinit(index);
        init(index, dispatch::parameters());
    }

    int left = 0;
    int right = (frame.stereo_layout == parameters::layout_mono ? 0 : 1);

    // Backup GL state
    GLint framebuffer_bak;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_bak);
    GLint viewport_bak[4];
    glGetIntegerv(GL_VIEWPORT, viewport_bak);
    GLboolean scissor_bak = glIsEnabled(GL_SCISSOR_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, frame.width, frame.height);

    _prg[index]->bind();
    if (frame.layout == video_frame::bgra32)
    {
        _prg[index]->setUniformValue("srgb_tex", 0);
    }
    else
    {
        _prg[index]->setUniformValue("y_tex", 0);
        _prg[index]->setUniformValue("u_tex", 1);
        _prg[index]->setUniformValue("v_tex", 2);
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    if (frame.layout == video_frame::bgra32)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input->_bgra32_tex[left]);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input->_yuv_y_tex[left]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, input->_yuv_u_tex[left]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, input->_yuv_v_tex[left]);
    }
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    //                       _tex[index][0], 0);
    //check_FBO(QString::fromStdString(HERE));
    draw_quad(-1.0f, +1.0f, +2.0f, -2.0f);
    if (left != right)
    {
        if (frame.layout == video_frame::bgra32) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, input->_bgra32_tex[right]);
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, input->_yuv_y_tex[right]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, input->_yuv_u_tex[right]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, input->_yuv_v_tex[right]);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               _tex[index][1], 0);
        check_FBO(QString::fromStdString(HERE));
        draw_quad(-1.0f, +1.0f, +2.0f, -2.0f);
    }
    _prg[index]->release();

    // Restore GL state
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_bak);
    if (scissor_bak)
        glEnable(GL_SCISSOR_TEST);
    glViewport(viewport_bak[0], viewport_bak[1], viewport_bak[2], viewport_bak[3]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

}

void color_handel::init(const int index, const parameters &params)
{
    if (!_is_init_opengl)
    {
        initializeOpenGLFunctions();
        _is_init_opengl = true;
    }
    glGenFramebuffers(1, &_fbo);

    QString quality_str;
    QString layout_str;
    QString color_space_str;
    QString value_range_str;
    QString storage_str;
    QString chroma_offset_x_str;
    QString chroma_offset_y_str;

    quality_str = QString::number(params.quality());
    if (input->_last_frame.layout == video_frame::bgra32)
    {
        layout_str = "layout_bgra32";
        color_space_str = "color_space_srgb";
        value_range_str = "value_range_8bit_full";
        storage_str = "storage_srgb";
    }
    else
    {
        layout_str = "layout_yuv_p";
        if (input->_last_frame.color_space == video_frame::yuv709) {
            color_space_str = "color_space_yuv709";
        } else {
            color_space_str = "color_space_yuv601";
        }
        if (input->_last_frame.value_range == video_frame::u8_full) {
            value_range_str = "value_range_8bit_full";
            storage_str = "storage_srgb";
        } else if (input->_last_frame.value_range == video_frame::u8_mpeg) {
            value_range_str = "value_range_8bit_mpeg";
            storage_str = "storage_srgb";
        } else if (input->_last_frame.value_range == video_frame::u10_full) {
            value_range_str = "value_range_10bit_full";
            storage_str = "storage_linear_rgb";
        } else {
            value_range_str = "value_range_10bit_mpeg";
            storage_str = "storage_linear_rgb";
        }
        chroma_offset_x_str = "0.0";
        chroma_offset_y_str = "0.0";
        if (input->_last_frame.layout == video_frame::yuv422p)
        {
            if (input->_last_frame.chroma_location == video_frame::left)
            {
                chroma_offset_x_str = QString::number(0.5f / float(input->_last_frame.width
                            / input->_yuv_chroma.width()));
            }
            else if (input->_last_frame.chroma_location == video_frame::topleft)
            {
                chroma_offset_x_str = QString::number(0.5f / float(input->_last_frame.width
                            / input->_yuv_chroma.width()));
                chroma_offset_y_str = QString::number(0.5f / float(input->_last_frame.height
                            / input->_yuv_chroma.height()));
            }
        }
        else if (input->_last_frame.layout == video_frame::yuv420p)
        {
            if (input->_last_frame.chroma_location == video_frame::left)
            {
                chroma_offset_x_str = QString::number(0.5f / float(input->_last_frame.width
                            / input->_yuv_chroma.width()));
            } else if (input->_last_frame.chroma_location == video_frame::topleft)
            {
                chroma_offset_x_str = QString::number(0.5f / float(input->_last_frame.width
                            / input->_yuv_chroma.width()));
                chroma_offset_y_str = QString::number(0.5f / float(input->_last_frame.height
                            / input->_yuv_chroma.height()));
            }
        }
    }

    if (params.quality() == 0)
    {
        storage_str = "storage_srgb";   // SRGB data in GL_RGB8 texture.
    }
    else if (storage_str == "storage_srgb"
            && (!srgb8_textures_are()))
    {
        storage_str = "storage_linear_rgb";
    }

    QString color;
    QFile file(":/shader/video_output_color.fs.glsl");
    if (file.open(QIODevice::ReadOnly))
    {
        color = QString(file.readAll());
    }
    file.close();

    QString color_fs_src(color.toLatin1().data());
    color_fs_src.replace("$quality", quality_str);
    color_fs_src.replace("$layout", layout_str);
    color_fs_src.replace("$color_space", color_space_str);
    color_fs_src.replace("$value_range", value_range_str);
    color_fs_src.replace("$chroma_offset_x", chroma_offset_x_str);
    color_fs_src.replace("$chroma_offset_y", chroma_offset_y_str);
    color_fs_src.replace("$storage", storage_str);

    if (!_prg[index])
    {
        _prg[index] = new QOpenGLShaderProgram();
        _prg[index]->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                              color_fs_src);
        _prg[index]->link();
    }

    for (int i = 0; i < (input->_last_frame.stereo_layout == parameters::layout_mono ? 1 : 2); i++)
    {
        glGenTextures(1, &(_tex[index][i]));
        glBindTexture(GL_TEXTURE_2D, _tex[index][i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexImage2D(GL_TEXTURE_2D, 0,
                params.quality() == 0 ? GL_RGB
                : storage_str == "storage_srgb" ? GL_SRGB8 : GL_RGB16,
                input->_last_frame.width, input->_last_frame.height,
                     0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
    }
    _last_params[index] = params;
}
void color_handel::deinit(const int index)
{
    if (_fbo)
        glDeleteFramebuffers(1, &_fbo);

    if (_prg[index] != 0)
    {
        delete _prg[index];
    }
    for (int i = 0; i < 2; i++)
    {
        if (_tex[index][i] != 0)
        {
            glDeleteTextures(1, &(_tex[index][i]));
        }
    }
    _last_params[index] = parameters();
}
bool color_handel::is_compatible(int index, const parameters& params,
                                 const video_frame &frame)
{
    if (!input) return false;
    return (_last_params[index].quality() == params.quality()
            && input->is_compatible(frame));

}
bool color_handel::check_FBO(const QString& where)
{
    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE)
    {
        QString pfx = (where.length() > 0 ? where + ": " : "");
        throw (pfx + QString("OpenGL Framebuffer status error 0x")
                  +QString::number(e, 16));
        return false;
    }
    return true;
}
bool color_handel::srgb8_textures_are()
{
    bool retval = true;
    GLuint fbo;
    GLuint tex;

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, 2, 2, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
    GLint framebuffer_bak;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_bak);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE) {
        retval = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_bak);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tex);
    return retval;
}
