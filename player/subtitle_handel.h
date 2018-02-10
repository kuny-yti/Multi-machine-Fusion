#ifndef SUBTITLE_HANDEL_H
#define SUBTITLE_HANDEL_H
#include "media_data.h"
#include "subtitle_updater.h"
#include "render_kernel.h"

class subtitle_handel :public render_kernel, QOpenGLFunctions
{
private:
    subtitle_renderer _renderer;
    subtitle_updater *_updater;
    subtitle_box _box[2];

    GLuint _tex[2];
    bool _tex_current[2];
    GLuint _fbo;
    GLuint _pbo;
    bool _is_init_opengl;

    class input_handel *input;
    class color_handel *color;
    friend class render_handel;

public:
    subtitle_handel(class input_handel *in, class color_handel *color);
    ~subtitle_handel();

    void handel(const int index, const subtitle_box &subtitle);
    void init(const int index);
    void deinit(const int index);
};

#endif // SUBTITLE_HANDEL_H
