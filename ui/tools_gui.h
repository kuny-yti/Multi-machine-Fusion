#ifndef GUI_H
#define GUI_H

#include "ui_tools_gui.h"
#include "ctrl.h"

class tools_gui_p;
class tools_gui : public QWidget, private Ui::tools_gui, private ctrl
{
    Q_OBJECT

public:
    explicit tools_gui(class gui_config *, QWidget *parent = 0);
    virtual ~tools_gui();

    void handle_exit();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *);

private slots:
    //! 随系统启动
    void on_system_append_start_stateChanged(int arg1);
    //! 启动后最小化工具栏
    void on_system_start_minimize_stateChanged(int arg1);
    //! 启动后隐藏鼠标
    void on_system_warp_cursor_stateChanged(int arg1);
    //! 设置工具栏语言
    void on_system_language_activated(int index);
    //! 设置为初级模式(默认)
    void on_system_junior_clicked();
    //! 设置为高级模式(默认)
    void on_system_senior_clicked();
    //! 设置为维护模式(默认)
    void on_system_maintenance_clicked();

    //! 设置投影布局(默认1*2)
    void on_device_projection_layout_v_activated(int index);
    void on_device_projection_layout_h_activated(int index);
    //! 设置设备分布率(默认XGA 1024*768)
    void on_device_resolution_activated(int index);
    //! 使用的算法策略(默认自动算法-高级模式使用)
    void on_device_strategy_activated(int index);
    //! 启动调试
    void on_device_start_clicked();

    //! 设置调试模式(默认FFD模式-高级模式使用)
    void on_geometry_mode_ffd_clicked();
    void on_geometry_mode_mesh_clicked();

    //! 设置渲染模式(默认Grid模式，网格模式下附加功能失效)
    void on_rendering_grid_clicked();
    void on_rendering_image_clicked();
    void on_rendering_media_clicked();
    void on_rendering_desktop_clicked();
    //! 设置渲染附加(默认Fusion-高级模式使用)
    void on_rendering_attach_fusion_stateChanged(int arg1);
    void on_rendering_attach_grid_stateChanged(int arg1);
    void on_rendering_attach_point_stateChanged(int arg1);

    //! 使用软件内部图像(默认Mesh)
    void on_multimedia_image_local_clicked();
    //! 选择使用的内部图像
    void on_multimedia_image_local_select_activated(int index);
    //! 使用自定义图像
    void on_multimedia_image_other_clicked();
    //! 选择自定义图像
    void on_multimedia_image_other_select_activated(int index);
    //! 选择序列图使用目录
    void on_multimedia_sequence_select_activated(int index);
    //! 设置序列图播放速率
    void on_multimedia_sequence_speed_valueChanged(int arg1);
    //! 选择媒体文件
    void on_multimedia_media_select_activated(int index);

    //! 开始播放/停止-重新播放媒体或序列图
    void on_control_play_stop_clicked();
    //! 暂停/继续播放媒体或序列图
    void on_control_pause_continue_clicked();
    //! 设置软件输出音量
    void on_control_volum_valueChanged(int value);
    //! 默认播放模式(默认为播放完成后停止)
    void on_control_default_clicked();
    //! 循环播放模式
    void on_control_loop_play_clicked();
    //! 命令播放模式
    void on_control_command_play_clicked();

    //! 附加调试窗口
    void on_attach_debug_clicked();
    void on_attach_debug_left_clicked();

    //! 保存和加载
    void on_save_geometry_clicked();
    void on_load_geometry_clicked();

    //! 步骤保存和加载
    void on_step_save_clicked();
    void on_step_load_activated(const QString &arg1);

    //! 解码器模式
    void on_decode_mode_cpu_clicked();
    void on_decoder_mode_gpu_clicked();
    void on_decoder_mode_opencl_clicked();

    //! 桌面启动
    void on_multimedia_start_desktop_clicked();
    void on_multimedia_frame_rate_valueChanged(int arg1);

    void on_multimedia_capture_desktop_clicked();

    void on_select_video_time();
    void on_system_data_load_clicked();
    void on_system_data_save_clicked();

    void on_hide_tools_clicked();
    void on_exit_system_clicked();

    void handle_start();
    void on_device_mouse_adjust_clicked();

    void on_device_keyboard_adjust_clicked();

    void on_device_move_pixel_activated(int index);

private:
    //! 接受命令
    virtual void command(const int& cmd);

    void gui_handle();
    void handle_gui();

    void handle_rendering_gui(int rd);
    void handle_rendering(int rd);
    void handle_attach(int aa);
    ColorConfig &current_color(const QString &arg1 = QString());
private:
    tools_gui_p *d;
};

#endif // GUI_H
