#ifndef VIDEO_OUTPUT_H
#define VIDEO_OUTPUT_H

#include "blob.h"
#include "media_data.h"
#include "dispatch.h"

#include "render_handel.h"
#include "color_handel.h"
#include "input_handel.h"
#include "subtitle_handel.h"

class video_output : public controller
{
private:
    bool _initialized;

    int _viewport[2][4];
    float _tex_coords[2][4][2];
    int _active_index;

    input_handel *input;
    color_handel *color;
    subtitle_handel *subtitle;
    render_handel *render;
public:
    video_output();

    void init();
    void deinit();
    virtual void prepare_next_frame(const video_frame &frame,
                                    const subtitle_box &subtitle);

    virtual void activate_next_frame();
    virtual void process_events() {}
    virtual bool supports_stereo() const {return false;}
    void display_current_frame(int64_t display_frameno = 0);
};

#endif // VIDEO_OUTPUT_H
