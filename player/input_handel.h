#ifndef INPUT_HANDEL_H
#define INPUT_HANDEL_H
#include "render_kernel.h"
#include "media_data.h"

/// 使用pbo
/// 使用yuv 纹理
/// 使用bgra 纹理
/// 作用是将纹理上传到指定纹理对象中

class input_handel : public render_kernel ,QOpenGLFunctions
{
private:
    GLuint _pbo;
    GLuint _yuv_y_tex[2];
    GLuint _yuv_u_tex[2];
    GLuint _yuv_v_tex[2];
    GLuint _bgra32_tex[2];
    QSize _yuv_chroma;

    video_frame _last_frame;
    video_frame _frame[2];

    bool _is_init_opengl;

    friend class color_handel;
    friend class subtitle_handel;
    friend class render_handel;
    friend class video_output;

public:
    input_handel();
    ~input_handel();

    void handel(const int index, const video_frame &frame);

    void init(const video_frame &last_frame);
    void deinit();
    bool is_compatible(const video_frame &current_frame);
};

#endif // INPUT_HANDEL_H
