#include "render_handel.h"
#include "input_handel.h"
#include "color_handel.h"
#include "subtitle_handel.h"
#include "dispatch.h"
#include "color_matrix.h"

#include <QFile>

render_handel::render_handel(input_handel *i,
                             color_handel *c,
                             subtitle_handel *s):
    input(i),
    color(c),
    subitle(s)
{
    _params = parameters ();
    _last_params= parameters ();
    _last_frame = video_frame();

    _dummy_tex = 0;
    _mask_tex = 0;
    _3d_ready_sync_buf = blob();

    _prg = 0;
    _is_init_opengl = false;

}
void render_handel::init()
{
    if (!_is_init_opengl)
    {
        _is_init_opengl = true;
        initializeOpenGLFunctions();
    }
    QString quality_str = QString::number(_params.quality());
    QString mode_str = (
            _params.stereo_mode() == parameters::mode_even_odd_rows ? "mode_even_odd_rows"
            : _params.stereo_mode() == parameters::mode_even_odd_columns ? "mode_even_odd_columns"
            : _params.stereo_mode() == parameters::mode_checkerboard ? "mode_checkerboard"
            : _params.stereo_mode() == parameters::mode_red_cyan_monochrome ? "mode_red_cyan_monochrome"
            : _params.stereo_mode() == parameters::mode_red_cyan_half_color ? "mode_red_cyan_half_color"
            : _params.stereo_mode() == parameters::mode_red_cyan_full_color ? "mode_red_cyan_full_color"
            : _params.stereo_mode() == parameters::mode_red_cyan_dubois ? "mode_red_cyan_dubois"
            : _params.stereo_mode() == parameters::mode_green_magenta_monochrome ? "mode_green_magenta_monochrome"
            : _params.stereo_mode() == parameters::mode_green_magenta_half_color ? "mode_green_magenta_half_color"
            : _params.stereo_mode() == parameters::mode_green_magenta_full_color ? "mode_green_magenta_full_color"
            : _params.stereo_mode() == parameters::mode_green_magenta_dubois ? "mode_green_magenta_dubois"
            : _params.stereo_mode() == parameters::mode_amber_blue_monochrome ? "mode_amber_blue_monochrome"
            : _params.stereo_mode() == parameters::mode_amber_blue_half_color ? "mode_amber_blue_half_color"
            : _params.stereo_mode() == parameters::mode_amber_blue_full_color ? "mode_amber_blue_full_color"
            : _params.stereo_mode() == parameters::mode_amber_blue_dubois ? "mode_amber_blue_dubois"
            : _params.stereo_mode() == parameters::mode_red_green_monochrome ? "mode_red_green_monochrome"
            : _params.stereo_mode() == parameters::mode_red_blue_monochrome ? "mode_red_blue_monochrome"
            : "mode_onechannel");
    QString subtitle_str = (needs_subtitle(_params) ? "subtitle_enabled" : "subtitle_disabled");
    QString coloradjust_str = (needs_coloradjust(_params) ? "coloradjust_enabled" : "coloradjust_disabled");
    QString ghostbust_str = (needs_ghostbust(_params) ? "ghostbust_enabled" : "ghostbust_disabled");

    QString render;
    QFile file("./video_output.fs.glsl");
    file.open(QIODevice::ReadOnly);
    render = file.readAll();
    file.close();

    QString render_fs_src(render.toLatin1().data());//(VIDEO_OUTPUT_FS_GLSL_STR);
    render_fs_src.replace("$quality", quality_str);
    render_fs_src.replace("$mode", mode_str);
    render_fs_src.replace("$subtitle", subtitle_str);
    render_fs_src.replace("$coloradjust", coloradjust_str);
    render_fs_src.replace("$ghostbust", ghostbust_str);

    if (!_prg)
    {
        _prg = new QOpenGLShaderProgram();
        _prg->addShaderFromSourceCode(QOpenGLShader::Fragment, render_fs_src);
        _prg->link();
    }

    uint32_t dummy_texture = 0;
    glGenTextures(1, &_dummy_tex);
    glBindTexture(GL_TEXTURE_2D, _dummy_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
            GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &dummy_texture);
    if (_params.stereo_mode() == parameters::mode_even_odd_rows
            || _params.stereo_mode() == parameters::mode_even_odd_columns
            || _params.stereo_mode() == parameters::mode_checkerboard)
    {
        GLubyte even_odd_rows_mask[4] = { 0xff, 0xff, 0x00, 0x00 };
        GLubyte even_odd_columns_mask[4] = { 0xff, 0x00, 0xff, 0x00 };
        GLubyte checkerboard_mask[4] = { 0xff, 0x00, 0x00, 0xff };
        glGenTextures(1, &_mask_tex);
        glBindTexture(GL_TEXTURE_2D, _mask_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                _params.stereo_mode() == parameters::mode_even_odd_rows ? even_odd_rows_mask
                : _params.stereo_mode() == parameters::mode_even_odd_columns ? even_odd_columns_mask
                : checkerboard_mask);
    }
}
void render_handel::deinit()
{
    if (_prg != 0)
    {
        delete (_prg);
    }
    _prg = 0;

    if (_dummy_tex != 0)
    {
        glDeleteTextures(1, &_dummy_tex);
        _dummy_tex = 0;
    }
    if (_mask_tex != 0) {
        glDeleteTextures(1, &_mask_tex);
        _mask_tex = 0;
    }
    _last_params = parameters();
    _last_frame = video_frame();
}
bool render_handel::is_compatible()
{
    return (_last_params.quality() == _params.quality()
            && _last_params.stereo_mode() == _params.stereo_mode()
            && needs_subtitle(_last_params) == needs_subtitle(_params)
            && needs_coloradjust(_last_params) == needs_coloradjust(_params)
            && needs_ghostbust(_last_params) == needs_ghostbust(_params));
}
bool render_handel::needs_subtitle(const parameters& params)
{
    return params.subtitle_stream() != -1;
}
bool render_handel::needs_coloradjust(const parameters& params)
{
    return params.contrast() < 0.0f || params.contrast() > 0.0f
        || params.brightness() < 0.0f || params.brightness() > 0.0f
        || params.hue() < 0.0f || params.hue() > 0.0f
        || params.saturation() < 0.0f || params.saturation() > 0.0f;
}
bool render_handel::needs_ghostbust(const parameters& params)
{
    return params.ghostbust() > 0.0f;
}
bool render_handel::context_is_stereo()
{
    return false;
}
void render_handel::handel(const int index, int64_t display_frameno,
                           bool mono_right_instead_of_left,
                           const QRectF &rect,
                           const GLint viewport[2][4],
                            const float tex_coords[2][4][2],
                            parameters::stereo_mode_t stereo_mode)
{
    const video_frame &frame = input->_frame[index];
    if (!frame.is_valid())
        return;

    _params = dispatch::parameters();
    _params.set_quality(color->_last_params[index].quality());

    _params.set_stereo_mode(stereo_mode);
    bool context_needs_stereo = (_params.stereo_mode()
                                 == parameters::mode_stereo);
    if (context_needs_stereo != context_is_stereo())
    {
        recreate_context(context_needs_stereo);
        return;
    }
    if (!_prg || !is_compatible())
    {
        deinit();
        init();
    }
    _last_params = _params;
    _last_frame = frame;

    int left = 0;
    int right = (frame.stereo_layout == parameters::layout_mono ? 0 : 1);
    if (_params.stereo_mode_swap())
    {
        std::swap(left, right);
    }
    if ((_params.stereo_mode() == parameters::mode_even_odd_rows
                || _params.stereo_mode() == parameters::mode_checkerboard))
    {
        std::swap(left, right);
    }
    if ((_params.stereo_mode() == parameters::mode_even_odd_columns
         || _params.stereo_mode() == parameters::mode_checkerboard))
    {
        std::swap(left, right);
    }

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    float my_tex_coords[2][4][2];
    std::memcpy(my_tex_coords, tex_coords, sizeof(my_tex_coords));

    _prg->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color->_tex[index][left]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, color->_tex[index][right]);

    _prg->setUniformValue("rgb_l", 0);
    _prg->setUniformValue("rgb_r", 1);
    _prg->setUniformValue( "parallax",
            _params.parallax() * 0.05f
            * (_params.stereo_mode_swap() ? -1 : +1));

    if (needs_subtitle(_params))
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, (subitle->_box[index].is_valid()
                    ? subitle->_tex[index] : _dummy_tex));
        _prg->setUniformValue("subtitle", 2);
        _prg->setUniformValue( "subtitle_parallax",
                _params.subtitle_parallax() * 0.05f
                * (_params.stereo_mode_swap() ? -1 : +1));
    }
    if (needs_coloradjust(_params))
    {
        float color_matrix[16];
        get_color_matrix(_params.brightness(), _params.contrast(),
                _params.hue(), _params.saturation(), color_matrix);
        _prg->setUniformValue("color_matrix", QMatrix4x4(color_matrix));
    }
    if (_params.stereo_mode() != parameters::mode_red_green_monochrome
            && _params.stereo_mode() != parameters::mode_red_cyan_half_color
            && _params.stereo_mode() != parameters::mode_red_cyan_full_color
            && _params.stereo_mode() != parameters::mode_red_cyan_dubois
            && _params.stereo_mode() != parameters::mode_green_magenta_monochrome
            && _params.stereo_mode() != parameters::mode_green_magenta_half_color
            && _params.stereo_mode() != parameters::mode_green_magenta_full_color
            && _params.stereo_mode() != parameters::mode_green_magenta_dubois
            && _params.stereo_mode() != parameters::mode_amber_blue_monochrome
            && _params.stereo_mode() != parameters::mode_amber_blue_half_color
            && _params.stereo_mode() != parameters::mode_amber_blue_full_color
            && _params.stereo_mode() != parameters::mode_amber_blue_dubois
            && _params.stereo_mode() != parameters::mode_red_blue_monochrome
            && _params.stereo_mode() != parameters::mode_red_cyan_monochrome
            && needs_ghostbust(_params))
    {

        _prg->setUniformValue("crosstalk",
                _params.crosstalk_r() * _params.ghostbust(),
                _params.crosstalk_g() * _params.ghostbust(),
                _params.crosstalk_b() * _params.ghostbust());
    }
    if (_params.stereo_mode() == parameters::mode_even_odd_rows
            || _params.stereo_mode() == parameters::mode_even_odd_columns
            || _params.stereo_mode() == parameters::mode_checkerboard) {
        _prg->setUniformValue("mask_tex", 3);
        _prg->setUniformValue("step_x", 1.0f / float(viewport[0][2]));
        _prg->setUniformValue("step_y", 1.0f / float(viewport[0][3]));
    }
    if (_params.stereo_mode() == parameters::mode_stereo)
    {
        _prg->setUniformValue("channel", 0.0f);
        glDrawBuffer(GL_BACK_LEFT);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
        _prg->setUniformValue("channel", 1.0f);
        glDrawBuffer(GL_BACK_RIGHT);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
    }
    else if (_params.stereo_mode() == parameters::mode_even_odd_rows
            || _params.stereo_mode() == parameters::mode_even_odd_columns
            || _params.stereo_mode() == parameters::mode_checkerboard)
    {
        float vpw = static_cast<float>(viewport[0][2]);
        float vph = static_cast<float>(viewport[0][3]);
        float more_tex_coords[4][2] =
        {
            { 0.0f, 0.0f }, { vpw / 2.0f, 0.0f }, { vpw / 2.0f, vph / 2.0f }, { 0.0f, vph / 2.0f }
        };
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _mask_tex);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords, more_tex_coords);
    } else if (_params.stereo_mode() == parameters::mode_red_cyan_monochrome
            || _params.stereo_mode() == parameters::mode_red_cyan_half_color
            || _params.stereo_mode() == parameters::mode_red_cyan_full_color
            || _params.stereo_mode() == parameters::mode_red_cyan_dubois
            || _params.stereo_mode() == parameters::mode_green_magenta_monochrome
            || _params.stereo_mode() == parameters::mode_green_magenta_half_color
            || _params.stereo_mode() == parameters::mode_green_magenta_full_color
            || _params.stereo_mode() == parameters::mode_green_magenta_dubois
            || _params.stereo_mode() == parameters::mode_amber_blue_monochrome
            || _params.stereo_mode() == parameters::mode_amber_blue_half_color
            || _params.stereo_mode() == parameters::mode_amber_blue_full_color
            || _params.stereo_mode() == parameters::mode_amber_blue_dubois
            || _params.stereo_mode() == parameters::mode_red_green_monochrome
            || _params.stereo_mode() == parameters::mode_red_blue_monochrome)
    {
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
    }
    else if ((_params.stereo_mode() == parameters::mode_mono_left && !mono_right_instead_of_left)
            || (_params.stereo_mode() == parameters::mode_alternating && display_frameno % 2 == 0))
    {
        _prg->setUniformValue("channel", 0.0f);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
    }
    else if (_params.stereo_mode() == parameters::mode_mono_right
            || (_params.stereo_mode() == parameters::mode_mono_left && mono_right_instead_of_left)
            || (_params.stereo_mode() == parameters::mode_alternating && display_frameno % 2 == 1))
    {
        _prg->setUniformValue("channel", 1.0f);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
    } else if (_params.stereo_mode() == parameters::mode_left_right
            || _params.stereo_mode() == parameters::mode_left_right_half
            || _params.stereo_mode() == parameters::mode_top_bottom
            || _params.stereo_mode() == parameters::mode_top_bottom_half
            || _params.stereo_mode() == parameters::mode_hdmi_frame_pack)
    {
        _prg->setUniformValue("channel", 0.0f);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
        //***************上层加入FBO此时在上层设置视口*********************** //
        glViewport(viewport[1][0], viewport[1][1], viewport[1][2], viewport[1][3]);
        //************************************** //
        _prg->setUniformValue("channel", 1.0f);
        color->draw_quad(rect.x(), rect.y(), rect.width(), rect.height(), my_tex_coords);
    }
    _prg->release();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    if (_params.fullscreen() && _params.fullscreen_3d_ready_sync()
            && (_params.stereo_mode() == parameters::mode_left_right
                || _params.stereo_mode() == parameters::mode_left_right_half
                || _params.stereo_mode() == parameters::mode_top_bottom
                || _params.stereo_mode() == parameters::mode_top_bottom_half
                || _params.stereo_mode() == parameters::mode_alternating))
    {

        const uint32_t R = 0xffu << 16u;
        const uint32_t G = 0xffu << 8u;
        const uint32_t B = 0xffu;
        int width = input->_last_frame.width;
        int height = input->_last_frame.height;
        // Make space in the buffer for the pixel data
        size_t req_size = width * sizeof(uint32_t);
        if (_3d_ready_sync_buf.size() < req_size)
            _3d_ready_sync_buf.resize(req_size);
        if (_params.stereo_mode() == parameters::mode_left_right
                || _params.stereo_mode() == parameters::mode_left_right_half)
        {
           uint32_t color = (display_frameno % 2 == 0 ? R : G | B);
           for (int i = 0; i < width; i++)
               _3d_ready_sync_buf.ptr<uint32_t>()[i] = color;
           window_pos_2i(0, 0);
           glDrawPixels(width, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _3d_ready_sync_buf.ptr());
        } else if (_params.stereo_mode() == parameters::mode_top_bottom
                || _params.stereo_mode() == parameters::mode_top_bottom_half) {
           uint32_t color = (display_frameno % 2 == 0 ? B : R | G);
           for (int i = 0; i < width; i++)
               _3d_ready_sync_buf.ptr<uint32_t>()[i] = color;
           window_pos_2i(0, 0);
           glDrawPixels(width, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _3d_ready_sync_buf.ptr());
           window_pos_2i(0, height / 2);
           glDrawPixels(width, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _3d_ready_sync_buf.ptr());
        } else if (_params.stereo_mode() == parameters::mode_alternating) {
           uint32_t color = (display_frameno % 4 < 2 ? G : R | B);
           for (int i = 0; i < width; i++)
               _3d_ready_sync_buf.ptr<uint32_t>()[i] = color;
           window_pos_2i(0, 0);
           glDrawPixels(width, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _3d_ready_sync_buf.ptr());
        }
    }
}

void render_handel::clears()
{
    if (context_is_stereo())
    {
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
