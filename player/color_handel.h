#ifndef COLOR_HANDEL_H
#define COLOR_HANDEL_H


#include "media_data.h"
#include "render_kernel.h"
#include "subtitle_updater.h"
#include <QOpenGLShaderProgram>

/// 使用着色器
/// 使用fbo
/// 使用纹理对象
/// 作用是将输入的纹理进行颜色转换

class color_handel :public render_kernel, QOpenGLFunctions
{
private:
    class input_handel *input;
    parameters _last_params[2];
    QOpenGLShaderProgram *_prg[2];
    video_frame _frame[2];
    GLuint _fbo;
    GLuint _tex[2][2];
    bool _is_init_opengl;

    friend class subtitle_handel;
    friend class render_handel;

    bool srgb8_textures_are();
    bool check_FBO(const QString& where);
    void draw_quad(float x, float y, float w, float h,
            const float tex_coords[2][4][2] = NULL,
            const float more_tex_coords[4][2] = NULL) ;
public:
    color_handel(class input_handel *in);
    ~color_handel();

    void handel(const int index, const video_frame &frame);

//protected:
    void init(const int index, const parameters& params);
    void deinit(const int index);
    bool is_compatible(int index, const parameters &params,
                       const video_frame &frame);

};

#endif // COLOR_HANDEL_H
