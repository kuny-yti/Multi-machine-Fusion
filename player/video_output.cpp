#include "video_output.h"

video_output::video_output()
{
    _initialized = false;
    _active_index = 0;
    input = 0;
    color = 0;
    subtitle = 0;
    render = 0;

    if (!input)
        input = new input_handel();
    if (!color)
        color = new color_handel(input);
    if (!subtitle)
        subtitle = new subtitle_handel(input, color);
    ///if (!render)
    ///    render = new render_handel(input, color, subtitle);
}

void video_output::display_current_frame(int64_t display_frameno )
{
    render->handel(_active_index, display_frameno,
                          false, QRectF(-1.0f, -1.0f, 2.0f, 2.0f),
                          _viewport, _tex_coords,
                          dispatch::parameters().stereo_mode());
}
void video_output::activate_next_frame()
{
    _active_index = (_active_index == 0 ? 1 : 0);
}
void video_output::prepare_next_frame(const video_frame &frame,
                                      const subtitle_box &subt)
{
    int index = (_active_index == 0 ? 1 : 0);
    if (!frame.is_valid())
    {
        input->_frame[index] = frame;
        return;
    }

    input->handel(index, frame);
    color->handel(index, frame);
    subtitle->handel(index, subt);
}
void video_output::deinit()
{
    if (_initialized)
    {
        //render->clear();
        //input->deinit();
        //subtitle->deinit(0);
        //subtitle->deinit(1);
        //color->deinit(0);
        //color->deinit(1);
        //render->deinit();
        _initialized = false;
    }
}
void video_output::init()
{
    if (!_initialized)
    {
        _initialized = true;

    }
}
