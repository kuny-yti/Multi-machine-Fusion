#ifndef GUI_CONFIG_H
#define GUI_CONFIG_H

#include "system_config.h"
#include "ctrl.h"


class gui_config :private ctrl
{
public:
    gui_config(system_config *cf = NULL):
        syscf(cf)
    {
        if (!syscf)
        {
            syscf = system_config::config();
        }
    }
private:
    virtual void command(const int& cmd)
    {
        switch (cmd)
        {
        case Command_EditSave:
            break;
        case Command_EditLoad:
            break;
        case Command_ConfigSave:
            save();
            break;
        case Command_ConfigLoad:
            load();
            break;
        default:
            break;
        }

    }

    void load()
    {
        //memset((&raw_config()), 0, sizeof(RawConfig));
        config ()->begin ("system");
        QString tem = config ()->value ("language").toString();
        memcpy(&raw_config().system_language, tem.toUtf8().data(),
               tem.toUtf8().size());
        raw_config().system_language_index = config()->value("language_index").toInt();

        int val = config ()->value("warp").toInt();
        if (val > 0)
            raw_config().system_setup |= 1 << 2;
        else
            raw_config().system_setup &= ~(1<<2);

        val = config ()->value("minimize").toInt();
        if (val > 0)
            raw_config().system_setup |= 1 << 1;
        else
            raw_config().system_setup &= ~(1<<1);

        val = config ()->value("start").toInt();
        if (val > 0)
            raw_config().system_setup |= 1;
        else
            raw_config().system_setup &= ~(1);

        raw_config().widget_alpha = config ()->value("alpha").toFloat();
        config ()->end ();

        config ()->begin ("logs");
        tem = config ()->value("file").toString();
        memcpy(raw_config().logs_filename, tem.toUtf8().data(), tem.toUtf8().size());
        val = config ()->value("level").toInt();
        raw_config().logs_level = val;
        config ()->end ();

        config ()->begin ("display");
        val = config ()->value("window.x").toInt();
        raw_config().display_window[0] = val;
        val = config ()->value("window.y").toInt();
        raw_config().display_window[1] = val;
        val = config ()->value("window.w").toInt();
        raw_config().display_window[2] = val;
        val = config ()->value("window.h").toInt();
        raw_config().display_window[3] = val;

        raw_config().display_mode = config ()->value("mode").toInt();
        config ()->end ();

        config ()->begin ("player");
        val = config ()->value("decoder").toInt();
        raw_config().decoder_mode = (DecoderMode)val;
        config ()->end ();

        config ()->begin ("catalog");
        tem = config ()->value("config").toString();
        memcpy(&raw_config().dir_config, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.language").toString();
        memcpy(&raw_config().dir_config_language, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.log").toString();
        memcpy(&raw_config().dir_config_log, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.media").toString();
        memcpy(&raw_config().dir_config_media, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.data").toString();
        memcpy(&raw_config().dir_config_data, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.capture").toString();
        memcpy(&raw_config().dir_config_capture, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.capture.desktop").toString();
        memcpy(&raw_config().dir_config_capture_desktop, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.capture.media").toString();
        memcpy(&raw_config().dir_config_capture_media, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.capture.device").toString();
        memcpy(&raw_config().dir_config_capture_device, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.data.step").toString();
        memcpy(&raw_config().dir_config_data_step, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.media.image").toString();
        memcpy(&raw_config().dir_config_media_image, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.media.sequence").toString();
        memcpy(&raw_config().dir_config_media_sequence, tem.toUtf8().data(),
               tem.toUtf8().size());
        tem = config ()->value("config.media.video").toString();
        memcpy(&raw_config().dir_config_media_video, tem.toUtf8().data(),
               tem.toUtf8().size());
        config ()->end();

        config ()->begin ("device");
        tem = config ()->value("computer.layout").toString();
        raw_config().computer_layout = tem.toInt(0, 16);
        tem = config ()->value("computer.index").toString();
        raw_config().computer_index = tem.toInt(0, 16) - 0x101;
        tem = config ()->value("device.layout").toString();
        raw_config().device_layout = tem.toInt(0, 16);
        raw_config().device_resolution =(DeviceResolutionMode)
                config ()->value("resolution").toInt();
        raw_config().device_strategy = (DeviceStrategy)
                config ()->value("strategy").toInt();
        config ()->end();

        config ()->begin ("geometry");
        tem = config ()->value("control").toString();
        raw_config().geometry_control = tem.toInt(0, 16);
        tem = config ()->value("precision").toString();
        raw_config().geometry_precision = tem.toInt(0, 16);
        raw_config().adjust_mode = (MeshMode)config ()->value("adjust").toInt();
        tem = config ()->value("step.save").toString();
        memcpy(raw_config().data_step_save, tem.toUtf8().data(),
               tem.toUtf8().size());
        raw_config().data_step_index = config ()->value("step.indxe").toInt();
        config ()->end();

        config ()->begin ("media");
        raw_config().media_image_mode = config ()->value("image.mode").toInt();
        raw_config().media_image_local_index = config ()->value("image.local").toInt();
        raw_config().media_image_other_index = config ()->value("image.other").toInt();
        raw_config().media_sequence_index =
                config ()->value("sequence.index").toInt();
        raw_config().media_sequence_speed =
                config ()->value("sequence.speed").toInt();
        raw_config().media_video_index =
                config ()->value("video.index").toInt();
        raw_config().desktop_frame_rate =
                config ()->value("desktop.rate").toInt();
        raw_config().media_volum = config ()->value("volume").toFloat();
        config ()->end();

        config ()->begin ("control");
        raw_config().media_play_mode = (MediaPlayMode)
                config ()->value("play.mode").toInt();
        config ()->end();

        config ()->begin ("color");
        raw_config().color_device_id = config ()->value("device.id").toInt();
        config ()->end();

        config ()->begin("adjust");
        raw_config().adjust_device_id = config ()->value("device.id").toInt();
        config ()->end();

    }
    void save()
    {
        config ()->begin ("system");
        config ()->value ("language", raw_config().system_language);
        config()->value("language_index", raw_config().system_language_index);
        int val = (raw_config().system_setup & 1<<2) ? 2:0;
        config ()->value("warp", val);
        val = (raw_config().system_setup & 1<<1) ? 2:0;
        config ()->value("minimize", val);
        val = (raw_config().system_setup & 1) ? 2:0;
        config ()->value("start", val);
        config ()->value("alpha", raw_config().widget_alpha);
        config ()->end ();

        config ()->begin ("logs");
        config ()->value("file", raw_config().logs_filename);
        config ()->value("level", raw_config().logs_level);
        config ()->end ();

        config ()->begin ("display");
        config ()->value("window.x", raw_config().display_window[0]);
        config ()->value("window.y", raw_config().display_window[1]);
        config ()->value("window.w", raw_config().display_window[2]);
        config ()->value("window.h", raw_config().display_window[3]);
        config ()->value("mode", raw_config().display_mode);
        config ()->end ();

        config ()->begin ("player");
        config ()->value("decoder", raw_config().decoder_mode);
        config ()->end ();

        config ()->begin ("catalog");
        config ()->value("config", raw_config().dir_config);
        config ()->value("config.language", raw_config().dir_config_language);
        config ()->value("config.log", raw_config().dir_config_log);
        config ()->value("config.media", raw_config().dir_config_media);
        config ()->value("config.data", raw_config().dir_config_data);
        config ()->value("config.capture", raw_config().dir_config_capture);
        config ()->value("config.capture.desktop",
                         raw_config().dir_config_capture_desktop);
        config ()->value("config.capture.media",
                         raw_config().dir_config_capture_media);
        config ()->value("config.capture.device",
                         raw_config().dir_config_capture_device);
        config ()->value("config.data.step",
                         raw_config().dir_config_data_step);
        config ()->value("config.media.image",
                         raw_config().dir_config_media_image);
        config ()->value("config.media.sequence",
                         raw_config().dir_config_media_sequence);
        config ()->value("config.media.video",
                         raw_config().dir_config_media_video);
        config ()->end();

        config ()->begin ("device");
        config ()->value("computer.layout",
                         QString::number(raw_config().computer_layout, 16));
        config ()->value("computer.index", QString::number(raw_config().computer_index
                                                           +0x101, 16));
        config ()->value("device.layout",
                         QString::number(raw_config().device_layout, 16));
        config ()->value("resolution", raw_config().device_resolution);
        config ()->value("strategy", raw_config().device_strategy);
        config ()->end();

        config ()->begin ("geometry");
        config ()->value("control", QString::number(raw_config().geometry_control, 16));
        config ()->value("precision", QString::number(raw_config().geometry_precision, 16));
        config ()->value("adjust", raw_config().adjust_mode);
        config ()->value("step.indxe", raw_config().data_step_index);
        config ()->value("step.save", raw_config().data_step_save);
        config ()->end();

        config ()->begin ("media");
        config ()->value("image.mode", raw_config().media_image_mode);
        config ()->value("image.local", raw_config().media_image_local_index);
        config ()->value("image.other", raw_config().media_image_other_index);
        config ()->value("sequence.index", raw_config().media_sequence_index);
        config ()->value("sequence.speed", raw_config().media_sequence_speed);
        config ()->value("video.index", raw_config().media_video_index);
        config ()->value("desktop.rate", raw_config().desktop_frame_rate);
        config ()->value("volume", raw_config().media_volum);
        config ()->end();

         config ()->begin ("control");
         config ()->value("play.mode", raw_config().media_play_mode);
         config ()->end();

         config ()->begin ("color");
         config ()->value("device.id", raw_config().color_device_id);
         config ()->end();

         config ()->begin ("adjust");
         config ()->value("device.id", raw_config().adjust_device_id);
         config ()->end();
    }
    void sync()
    {

    }
    bool check()
    {
        // 比如少配置。或是配置错误
    }
private:
    system_config *syscf;
};

#endif // GUI_CONFIG

