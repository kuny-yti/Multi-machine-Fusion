#ifndef RENDER_HANDEL_H
#define RENDER_HANDEL_H
#include "render_kernel.h"
#include "media_data.h"
#include "blob.h"

#include <QOpenGLShaderProgram>

class render_handel :public render_kernel, QOpenGLFunctions
{
private:
    parameters _params;
    parameters _last_params;
    video_frame _last_frame;

    GLuint _dummy_tex;
    GLuint _mask_tex;
    blob _3d_ready_sync_buf;

    QOpenGLShaderProgram *_prg;
    bool _is_init_opengl;

    class input_handel *input;
    class color_handel *color;
    class subtitle_handel *subitle;

    bool needs_subtitle(const parameters& params);
    bool needs_coloradjust(const parameters& params);
    bool needs_ghostbust(const parameters& params);

public:
    render_handel(class input_handel *input,
                    class color_handel *color,
                    class subtitle_handel *subitle);

    void clears();
    void init();
    void deinit();
    void handel(const int index, int64_t display_frameno,
                bool mono_right_instead_of_left,
                const QRectF &rect,
                const GLint viewport[2][4],
                const float tex_coords[2][4][2],
                parameters::stereo_mode_t stereo_mode);
    bool is_compatible();
    virtual bool context_is_stereo();
    virtual bool recreate_context(bool ) {}
};

#endif // RENDER_HANDEL_H
