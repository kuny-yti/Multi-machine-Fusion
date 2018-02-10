#include "subtitle_updater.h"

subtitle_updater::subtitle_updater(subtitle_renderer* renderer) :
    _renderer(renderer)
{
    reset();
}

void subtitle_updater::reset()
{
    _last_outwidth = -1;
    _last_outheight = -1;
    _last_timestamp = std::numeric_limits<int64_t>::min();
}

void subtitle_updater::set(
        const subtitle_box& subtitle, int64_t timestamp,
        const parameters& params, int outwidth, int outheight,
        float pixel_ar)
{
    _subtitle = subtitle;
    _timestamp = timestamp;
    _params = params;
    _outwidth = outwidth;
    _outheight = outheight;
    _pixel_ar = pixel_ar;
}

void subtitle_updater::run()
{
    _buffer_changed = false;
    if (_subtitle.is_valid()
            && (_subtitle != _last_subtitle
                || (!_subtitle.is_constant() && _timestamp != _last_timestamp)
                || _outwidth != _last_outwidth
                || _outheight != _last_outheight
                || (_pixel_ar < _last_pixel_ar || _pixel_ar > _last_pixel_ar)
                || _params.subtitle_encoding() != _last_params.subtitle_encoding()
                || _params.subtitle_font() != _last_params.subtitle_font()
                || _params.subtitle_size() != _last_params.subtitle_size()
                || (_params.subtitle_scale() < _last_params.subtitle_scale()
                    || _params.subtitle_scale() > _last_params.subtitle_scale())
                || _params.subtitle_color() != _last_params.subtitle_color()
                || _params.subtitle_shadow() != _last_params.subtitle_shadow())) {
        _buffer_changed = _renderer->prerender(
                _subtitle, _timestamp, _params,
                _outwidth, _outheight, _pixel_ar,
                _bb_x, _bb_y, _bb_w, _bb_h);
        _last_subtitle = _subtitle;
        _last_timestamp = _timestamp;
        _last_params = _params;
        _last_outwidth = _outwidth;
        _last_outheight = _outheight;
        _last_pixel_ar = _pixel_ar;
    }

    if (_buffer_changed) {
        size_t bufsize = _bb_w * _bb_h * sizeof(uint32_t);
        if (_buffer.size() < bufsize)
            _buffer.resize(bufsize);
        _renderer->render(_buffer.ptr<uint32_t>());
    }
}

bool subtitle_updater::get(int *outwidth, int* outheight,
        void** ptr, int* bb_x, int* bb_y, int* bb_w, int* bb_h)
{
    *outwidth = _outwidth;
    *outheight = _outheight;
    *ptr = _buffer.ptr();
    *bb_x = _bb_x;
    *bb_y = _bb_y;
    *bb_w = _bb_w;
    *bb_h = _bb_h;
    return _buffer_changed;
}

