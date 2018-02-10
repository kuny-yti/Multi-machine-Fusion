#include "input_handel.h"

static int next_multiple_of_4(int x)
{
    return (x / 4 + (x % 4 == 0 ? 0 : 1)) * 4;
}

input_handel::input_handel()
{
    _is_init_opengl = false;
    _pbo = 0;
    _yuv_y_tex[0] = 0;
    _yuv_y_tex[1] = 0;
    _yuv_u_tex[0] = 0;
    _yuv_u_tex[1] = 0;
    _yuv_v_tex[0] = 0;
    _yuv_v_tex[1] = 0;
    _bgra32_tex[0] = 0;
    _bgra32_tex[1] = 0;
    _frame[0] = video_frame();
    _frame[1] = video_frame();
    _yuv_chroma = QSize(0,0);

    _last_frame = video_frame();
}
input_handel::~input_handel()
{
    deinit();
}
void input_handel::init(const video_frame &last_frame)
{
    if (!_is_init_opengl)
    {
        initializeOpenGLFunctions();
        _is_init_opengl = true;
    }

    _last_frame = last_frame;
    glGenBuffers(1, &_pbo);

    if (last_frame.layout == video_frame::bgra32)
    {
        for (int i = 0; i < (last_frame.stereo_layout == parameters::layout_mono ? 1 : 2); i++)
        {
            glGenTextures(1, &(_bgra32_tex[i]));
            glBindTexture(GL_TEXTURE_2D, _bgra32_tex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, last_frame.width, last_frame.height,
                    0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
        }
    }
    else
    {
        _yuv_chroma = QSize(1,1);
        bool need_chroma_filtering = false;
        if (last_frame.layout == video_frame::yuv422p)
        {
            _yuv_chroma.setWidth(2);
            need_chroma_filtering = true;
        }
        else if (last_frame.layout == video_frame::yuv420p)
        {
            _yuv_chroma = QSize(2,2);
            need_chroma_filtering = true;
        }
        bool type_u8 = (last_frame.value_range == video_frame::u8_full ||
                        last_frame.value_range == video_frame::u8_mpeg);
        GLint internal_format = type_u8 ? GL_LUMINANCE8 : GL_LUMINANCE16;
        GLint type = type_u8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
        for (int i = 0; i < (last_frame.stereo_layout == parameters::layout_mono ? 1 : 2); i++)
        {
            glGenTextures(1, &(_yuv_y_tex[i]));
            glBindTexture(GL_TEXTURE_2D, _yuv_y_tex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                    last_frame.width,
                    last_frame.height,
                    0, GL_LUMINANCE, type, NULL);
            glGenTextures(1, &(_yuv_u_tex[i]));
            glBindTexture(GL_TEXTURE_2D, _yuv_u_tex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, need_chroma_filtering ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, need_chroma_filtering ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                    last_frame.width / _yuv_chroma.width(),
                    last_frame.height / _yuv_chroma.height(),
                    0, GL_LUMINANCE, type, NULL);
            glGenTextures(1, &(_yuv_v_tex[i]));
            glBindTexture(GL_TEXTURE_2D, _yuv_v_tex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, need_chroma_filtering ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, need_chroma_filtering ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                    last_frame.width / _yuv_chroma.width(),
                    last_frame.height / _yuv_chroma.height(),
                    0, GL_LUMINANCE, type, NULL);
        }
    }
}
void input_handel::deinit()
{
    if (_pbo)
        glDeleteBuffers(1, &_pbo);
    _pbo = 0;

    for (int i = 0; i < 2; i++)
    {
        if (_yuv_y_tex[i] != 0)
        {
            glDeleteTextures(1, &(_yuv_y_tex[i]));
        }
        if (_yuv_u_tex[i] != 0)
        {
            glDeleteTextures(1, &(_yuv_u_tex[i]));
        }
        if (_yuv_v_tex[i] != 0)
        {
            glDeleteTextures(1, &(_yuv_v_tex[i]));
        }
        if (_bgra32_tex[i] != 0)
        {
            glDeleteTextures(1, &(_bgra32_tex[i]));
        }
        _yuv_y_tex[i] = 0;
        _yuv_u_tex[i] = 0;
        _yuv_v_tex[i] = 0;
        _bgra32_tex[i] = 0;
    }

}
bool input_handel::is_compatible(const video_frame &current_frame)
{
    return (_last_frame.width == current_frame.width
            && _last_frame.height == current_frame.height
            && _last_frame.layout == current_frame.layout
            && _last_frame.color_space == current_frame.color_space
            && _last_frame.value_range == current_frame.value_range
            && _last_frame.chroma_location == current_frame.chroma_location
            && _last_frame.stereo_layout == current_frame.stereo_layout);
}

void input_handel::handel(const int index, const video_frame &frame)
{
    int bytes_per_pixel = 4;
    GLenum format = GL_BGRA;
    GLenum type = GL_UNSIGNED_INT_8_8_8_8_REV;

    if (!is_compatible(frame))
    {
        deinit();
        init(frame);
    }
    _frame[index] = frame;

    if (frame.layout != video_frame::bgra32)
    {
        bool type_u8 = (frame.value_range == video_frame::u8_full ||
                        frame.value_range == video_frame::u8_mpeg);
        bytes_per_pixel = type_u8 ? 1 : 2;
        format = GL_LUMINANCE;
        type = type_u8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
    }

    for (int i = 0;
         i < (frame.stereo_layout == parameters::layout_mono ? 1 : 2);
         i++)
    {
        for (int plane = 0;
             plane < (frame.layout == video_frame::bgra32 ? 1 : 3);
             plane++)
        {
            // Determine the texture and the dimensions
            int w = frame.width;
            int h = frame.height;
            GLuint tex;
            int row_size;
            if (frame.layout == video_frame::bgra32)
            {
                tex = _bgra32_tex[i];
            }
            else
            {
                if (plane != 0)
                {
                    w /= _yuv_chroma.width();
                    h /= _yuv_chroma.height();
                }
                tex = (plane == 0 ? _yuv_y_tex[i]
                        : plane == 1 ? _yuv_u_tex[i]
                        : _yuv_v_tex[i]);
            }
            row_size = next_multiple_of_4(w * bytes_per_pixel);
            // Get a pixel buffer object buffer for the data
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, row_size * h, NULL, GL_STREAM_DRAW);
            void *pboptr = map_buffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
            if (!pboptr)
                throw exc(("Cannot create a PBO buffer."));
            Q_ASSERT(reinterpret_cast<uintptr_t>(pboptr) % 4 == 0);
            // Get the plane data into the pbo
            frame.copy_plane(i, plane, pboptr);

            unmap_buffer(GL_PIXEL_UNPACK_BUFFER);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, row_size / bytes_per_pixel);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, type, NULL);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
    }
}
