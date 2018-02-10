#include "ctrl.h"
#include <QMap>
#include <QStringList>

//程序启动后创建全局控制核心
ctrl_core *ctrl_core::_this = NULL;

ctrl_core::ctrl_core()
{
    _this = this;
    memset(&_this->_raw_config, 0, sizeof(RawConfig));
}
ctrl_core::~ctrl_core()
{
}
ctrl_core* ctrl_core::core()
{
    return _this;
}
QByteArray &ctrl_core::parameter()
{
    return _this->_parameters;
}
RawConfig &ctrl_core::raw_config()
{
    return _this->_raw_config;
}
void ctrl_core::control(const int &cmd)
{
    for (int i = 0; i < _this->_ctrl.size(); i++)
    {
        _this->_ctrl[i]->command(cmd);
    }
}
void ctrl_core::registers(ctrl* c)
{
    foreach (ctrl* var, _this->_ctrl)
    {
        if (var == c)
        {
            return;
        }
    }
    _this->_ctrl.append(c);
}
void ctrl_core::deregisters(ctrl *c)
{
    for (int i = 0; i < _this->_ctrl.size(); ++i)
    {
        if (_this->_ctrl[i] == c)
        {
            _this->_ctrl.remove(i);
        }
    }
}
ctrl::ctrl()
{
    ctrl_core::registers(this);
}
ctrl::~ctrl()
{
    ctrl_core::deregisters(this);
}
void ctrl::control(const int &cmd)
{
    ctrl_core::core()->control(cmd);
}
QByteArray &ctrl::parameter()
{
    return ctrl_core::parameter();
}
RawConfig &ctrl::raw_config()
{
    return ctrl_core::raw_config();
}
void ctrl::command(const int & )
{

}

QByteArray ctrl::serialize()
{
    QByteArray ser;
    QDataStream write(&ser, QIODevice::ReadWrite);

    write << raw_config().adjust_device_id;
    write << raw_config().adjust_mode;
    write << raw_config().admin_mode;
    write << raw_config().color_device_id;
    write << raw_config().computer_index;
    write << raw_config().computer_layout;
    write << raw_config().data_step_index;
    write << QString(raw_config().data_step_save);
    write << raw_config().decoder_mode;
    write << raw_config().desktop_frame_rate;
    write << raw_config().device_layout;
    write << raw_config().device_resolution;
    write << raw_config().device_strategy;

    write << QString(raw_config().dir_config);
    write << QString(raw_config().dir_config_capture);
    write << QString(raw_config().dir_config_capture_desktop);
    write << QString(raw_config().dir_config_capture_device);
    write << QString(raw_config().dir_config_capture_media);
    write << QString(raw_config().dir_config_data);
    write << QString(raw_config().dir_config_data_step);
    write << QString(raw_config().dir_config_language);
    write << QString(raw_config().dir_config_log);
    write << QString(raw_config().dir_config_media);
    write << QString(raw_config().dir_config_media_image);
    write << QString(raw_config().dir_config_media_sequence);
    write << QString(raw_config().dir_config_media_video);

    write << raw_config().display_mode;
    write << raw_config().display_window[0];
    write << raw_config().display_window[1];
    write << raw_config().display_window[2];
    write << raw_config().display_window[3];
    write << raw_config().geometry_control;
    write << raw_config().geometry_precision;
    write << QString(raw_config().logs_filename);
    write << raw_config().logs_level;
    write << (uint)((raw_config().media_duration >> 32) & 0xffffffff);
    write << (uint)((raw_config().media_duration ) & 0xffffffff);
    write << raw_config().media_image_local_index;
    write << QString(raw_config().media_image_local_select);
    write << raw_config().media_image_mode;
    write << raw_config().media_image_other_index;
    write << QString(raw_config().media_image_other_select);
    write << raw_config().media_play_mode;
    write << (uint)((raw_config().media_progress >> 32) & 0xffffffff);
    write << (uint)((raw_config().media_progress ) & 0xffffffff);
    write << QString(raw_config().media_sequence_dir);
    write << raw_config().media_sequence_index;
    write << raw_config().media_sequence_speed;
    write << raw_config().media_state;
    write << raw_config().media_video_index;
    write << QString(raw_config().media_video_select);
    write << raw_config().media_volum;
    write << QString(raw_config().system_language);
    write << raw_config().system_language_index;
    write << raw_config().system_setup;
    write << raw_config().widget_alpha;

    return ser;
}
bool ctrl::unserialize(const QByteArray &in)
{
    int tenum, bits;
    QString tchar;
    QByteArray ser = in;
    QDataStream read(&ser, QIODevice::ReadWrite);

    read >> bits; raw_config().adjust_device_id = bits;
    read >> tenum; raw_config().adjust_mode = (MeshMode)tenum;
    read >> tenum; raw_config().admin_mode = (AdministratorMode)tenum;
    read >> bits; raw_config().color_device_id =bits;
    read >> bits; raw_config().computer_index =bits;
    read >> bits; raw_config().computer_layout =bits;
    read >> bits; raw_config().data_step_index;
    read >> tchar; memcpy(raw_config().data_step_save, tchar.data(), tchar.size());
    read >> tenum; raw_config().decoder_mode = (DecoderMode)tenum;
    read >>  raw_config().desktop_frame_rate;
    read >> bits; raw_config().device_layout =bits;
    read >> tenum; raw_config().device_resolution = (DeviceResolutionMode)tenum;
    read >> tenum; raw_config().device_strategy = (DeviceStrategy)tenum;

    read >> tchar; memcpy(raw_config().dir_config, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_capture, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_capture_desktop,
                          tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_capture_device,
                          tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_capture_media, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_data, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_data_step, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_language, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_log, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_media, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_media_image, tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_media_sequence,
                          tchar.data(), tchar.size());
    read >> tchar; memcpy(raw_config().dir_config_media_video, tchar.data(), tchar.size());

    read >> bits; raw_config().display_mode =bits;
    read >>  raw_config().display_window[0];
    read >>  raw_config().display_window[1];
    read >>  raw_config().display_window[2];
    read >>  raw_config().display_window[3];
    read >> bits; raw_config().geometry_control =bits;
    read >> bits; raw_config().geometry_precision =bits;
    read >> tchar; memcpy(raw_config().logs_filename, tchar.data(), tchar.size());
    read >>  raw_config().logs_level;
    read >> tenum;
    read >> bits;
    raw_config().media_duration = ((tenum << 32) &0xffffffff00000000) | bits;
    read >>  raw_config().media_image_local_index;
    read >> tchar; memcpy(raw_config().media_image_local_select, tchar.data(), tchar.size());
    read >> bits; raw_config().media_image_mode = bits;
    read >>  raw_config().media_image_other_index;
    read >> tchar; memcpy(raw_config().media_image_other_select, tchar.data(), tchar.size());
    read >> tenum; raw_config().media_play_mode = (MediaPlayMode)tenum;
    read >> tenum;
    read >> bits;
    raw_config().media_progress = ((tenum << 32) &0xffffffff00000000) | bits;
    read >> tchar; memcpy(raw_config().media_sequence_dir, tchar.data(), tchar.size());
    read >>  raw_config().media_sequence_index;
    read >>  raw_config().media_sequence_speed;
    read >> bits; raw_config().media_state =bits;
    read >>  raw_config().media_video_index;
    read >> tchar; memcpy(raw_config().media_video_select, tchar.data(), tchar.size());
    read >>  raw_config().media_volum;
    read >> tchar; memcpy(raw_config().system_language, tchar.data(), tchar.size());
    read >>  raw_config().system_language_index;
    read >> bits; raw_config().system_setup = bits;
    read >>  raw_config().widget_alpha;

    return true;
}
