
#ifndef VIDEO_OUTPUT_H
#define VIDEO_OUTPUT_H

#include <vector>
#include <string>

#include <GL/glew.h>

#include "blob.h"

#include "media_data.h"
#include "subtitle_renderer.h"
#include "dispatch.h"

class subtitle_updater;
#if HAVE_LIBXNVCTRL
class CNvSDIout;
#endif // HAVE_LIBXNVCTRL

class video_output : public controller
{
private:
    bool _initialized;


    video_frame _frame[2];
    video_frame _input_last_frame;
    GLuint _input_pbo;
    GLuint _input_fbo;
    GLuint _input_yuv_y_tex[2];
    GLuint _input_yuv_u_tex[2];
    GLuint _input_yuv_v_tex[2];
    GLuint _input_bgra32_tex[2];
    int _input_yuv_chroma_width_divisor;
    int _input_yuv_chroma_height_divisor;
    subtitle_box _subtitle[2];
    GLuint _subtitle_tex[2];
    bool _subtitle_tex_current[2];

    parameters _color_last_params[2];
    video_frame _color_last_frame[2];
    GLuint _color_prg[2];
    GLuint _color_fbo;


    parameters _render_params;
    parameters _render_last_params;
    video_frame _render_last_frame;
    GLuint _render_prg;
    GLuint _render_dummy_tex;
    GLuint _render_mask_tex;
    blob _3d_ready_sync_buf;

    GLint _full_viewport[4];
    GLint _viewport[2][4];
    float _tex_coords[2][4][2];

    subtitle_updater *_subtitle_updater;
#if HAVE_LIBXNVCTRL
    CNvSDIout *_nv_sdi_output;
    int64_t _last_nv_sdi_displayed_frameno;
#endif // HAVE_LIBXNVCTRL

    bool xglCheckError(const std::string& where = std::string()) const;
    bool xglCheckFBO(const std::string& where = std::string()) const;
    GLuint xglCompileShader(const std::string& name, GLenum type,
                            const std::string& src) const;
    GLuint xglCreateProgram(GLuint vshader, GLuint fshader) const;
    GLuint xglCreateProgram(const std::string& name,
                            const std::string& vshader_src,
                            const std::string& fshader_src) const;
    void xglLinkProgram(const std::string& name, const GLuint prg) const;
    void xglDeleteProgram(GLuint prg) const;

    bool srgb8_textures_are_color_renderable();

    void draw_quad(float x, float y, float w, float h,
            const float tex_coords[2][4][2] = NULL,
            const float more_tex_coords[4][2] = NULL) const;

    void input_init(const video_frame &frame);
    void input_deinit();
    bool input_is_compatible(const video_frame &current_frame);
    void subtitle_init(int index);
    void subtitle_deinit(int index);

    void color_init(int index, const parameters& params,
                    const video_frame &frame);
    void color_deinit(int index);
    bool color_is_compatible(int index, const parameters& params,
                             const video_frame &current_frame);

    void render_init();
    void render_deinit();
    bool render_needs_subtitle(const parameters& params);
    bool render_needs_coloradjust(const parameters& params);
    bool render_needs_ghostbust(const parameters& params);
    bool render_is_compatible();

protected:
    int _active_index;
    GLuint _color_tex[2][2];
    subtitle_renderer _subtitle_renderer;

    virtual GLEWContext* glewGetContext() const = 0;

    int full_display_width() const;
    int full_display_height() const;

    virtual int video_display_width() const;
    virtual int video_display_height() const;

    virtual bool context_is_stereo() const = 0;
    virtual void recreate_context(bool stereo) = 0;
    virtual void trigger_resize(int w, int h) = 0;

    void clear() const;
    void reshape(int w, int h,
                 const parameters& params = dispatch::parameters());

    virtual int screen_width() const = 0;
    virtual int screen_height() const = 0;
    virtual float screen_pixel_aspect_ratio() const = 0;

    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual int pos_x() const = 0;
    virtual int pos_y() const = 0;

    void display_current_frame(int64_t display_frameno,
                               bool keep_viewport,
                               bool mono_right_instead_of_left,
                               float x, float y, float w, float h,
                               const GLint viewport[2][4],
                               const float tex_coords[2][4][2],
                               int dst_width, int dst_height,
                               parameters::stereo_mode_t stereo_mode);
    void display_current_frame(int64_t display_frameno,
                               int dst_width, int dst_height,
                               parameters::stereo_mode_t stereo_mode)
    {
        display_current_frame(display_frameno,
                              false, false, -1.0f, -1.0f, 2.0f, 2.0f,
                              _viewport, _tex_coords,
                              dst_width, dst_height, stereo_mode);
    }
    void display_current_frame(int64_t display_frameno = 0)
    {
        display_current_frame(display_frameno,
                              false, false, -1.0f, -1.0f, 2.0f, 2.0f,
                              _viewport, _tex_coords,
                              full_display_width(), full_display_height(),
                              dispatch::parameters().stereo_mode());
    }

#if HAVE_LIBXNVCTRL
    void sdi_output(int64_t display_frameno = 0);
#endif // HAVE_LIBXNVCTRL

public:
    video_output();
    virtual ~video_output();

    virtual void init();
    virtual int64_t wait_for_subtitle_renderer() = 0;
    virtual void deinit();

    void set_suitable_size(int w, int h, float ar,
                           parameters::stereo_mode_t stereo_mode);

    virtual bool supports_stereo() const = 0;

    virtual void center() = 0;

    virtual void enter_fullscreen() = 0;
    virtual void exit_fullscreen() = 0;

    virtual void process_events() = 0;

    virtual void prepare_next_frame(const video_frame &frame,
                                    const subtitle_box &subtitle);

    virtual void activate_next_frame();
    virtual int64_t time_to_next_frame_presentation() const;
};

#endif
