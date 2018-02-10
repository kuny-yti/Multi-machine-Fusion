#include "subtitle_handel.h"
#include "input_handel.h"
#include "color_handel.h"
#include "dispatch.h"
#include "dbg.h"

subtitle_handel::subtitle_handel(input_handel *in, color_handel *co):
    _renderer(),
    input(in),
    color(co)
{
    _is_init_opengl = false;
    _updater = 0;
    _tex[0] = 0;
    _tex[1] = 0;
    _tex_current[0] = false;
    _tex_current[1] = false;
    _fbo = 0;

    _updater = new subtitle_updater(&_renderer);

}
subtitle_handel::~subtitle_handel()
{
    deinit(0);
    deinit(1);
}
void subtitle_handel::init(const int index)
{
    if (!_is_init_opengl)
    {
        initializeOpenGLFunctions();
        _is_init_opengl = true;
    }
    glGenBuffers(1, &_pbo);
    glGenFramebuffers(1, &_fbo);

    if (_tex[index] == 0)
    {
        glGenTextures(1, &_tex[index]);
        glBindTexture(GL_TEXTURE_2D, _tex[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void subtitle_handel::deinit(const int index)
{
    if (_pbo)
        glDeleteBuffers(1, &_pbo);
    if (_fbo)
        glDeleteFramebuffers(1, &_fbo);
    if (_tex[index] != 0)
    {
        glDeleteTextures(1, _tex + index);
        _tex[index] = 0;
        _tex_current[index] = false;
    }
    _updater->reset();
}

void subtitle_handel::handel(const int index, const subtitle_box &subtitle)
{
    if (!color) return;

    _box[index] = subtitle;
    if (subtitle.is_valid())
    {
        init(index);
        Q_ASSERT(_renderer.is_initialized());
        int sub_outwidth, sub_outheight;
        if (_renderer.render_to_display_size(subtitle))
        {
            /// 屏幕尺寸
            sub_outwidth = input->_last_frame.width;
            sub_outheight = input->_last_frame.height;
        }
        else
        {
            sub_outwidth = input->_last_frame.width;
            sub_outheight = input->_last_frame.height;
        }
        _updater->set(subtitle, color->_frame[index].presentation_time,
                dispatch::parameters(), sub_outwidth, sub_outheight,
                (float)sub_outwidth / (float)sub_outheight);
        _updater->start();
    }

    if (subtitle.is_valid())
    {
        _updater->finish();
        int sub_outwidth, sub_outheight;
        void *ptr;
        int bb_x, bb_y, bb_w, bb_h;
        bool buffer_updated = _updater->get(
                &sub_outwidth, &sub_outheight,
                &ptr, &bb_x, &bb_y, &bb_w, &bb_h);
        bb_x = std::min(std::max(bb_x, 0), sub_outwidth - 1);
        bb_y = std::min(std::max(bb_y, 0), sub_outheight - 1);
        bb_w = std::min(std::max(bb_w, 0), sub_outwidth - bb_x);
        bb_h = std::min(std::max(bb_h, 0), sub_outheight - bb_y);
        if (buffer_updated || !_tex_current[index])
        {
            GLint tex_w, tex_h;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex[index]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);
            if (tex_w != sub_outwidth || tex_h != sub_outheight)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sub_outwidth, sub_outheight, 0,
                        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
            }
            // Clear the texture
            GLint framebuffer_bak;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_bak);
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex[index], 0);
            color->check_FBO(QString::fromStdString( HERE));
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_bak);
            // Get a PBO buffer of appropriate size for the bounding box.
            if (bb_w > 0 && bb_h > 0)
            {
                size_t size = bb_w * bb_h * sizeof(uint32_t);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, size, NULL, GL_STREAM_DRAW);
                void* pboptr = map_buffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if (!pboptr)
                    throw exc(("Cannot create a PBO buffer."));
                Q_ASSERT(reinterpret_cast<uintptr_t>(pboptr) % 4 == 0);
                // Copy the bounding box into the buffer
                std::memcpy(pboptr, ptr, size);
                // Update the appropriate part of the texture.
                unmap_buffer(GL_PIXEL_UNPACK_BUFFER);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, bb_w);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                glTexSubImage2D(GL_TEXTURE_2D, 0, bb_x, bb_y, bb_w, bb_h,
                        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            }
        }
        _tex_current[index] = true;
        if (buffer_updated)
        {
            _tex_current[index == 0 ? 1 : 0] = false;
        }
    }
}
