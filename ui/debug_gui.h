#ifndef DEBUG_GUI_H
#define DEBUG_GUI_H

#include "ui_debug_gui.h"
#include "ctrl.h"
#include "area_adjust.h"

class debug_gui : public QWidget, private Ui::debug_gui, private ctrl
{
    Q_OBJECT
public:
    explicit debug_gui(class gui_config *, QWidget *parent = 0);
    ~debug_gui();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *);

private slots:
    //! 设备序号选择
    void on_blend_device_id_activated(int index);
    //! 左融合带
    void on_blend_left_lume_spin_valueChanged(double arg1);
    void on_blend_left_lume_slider_sliderMoved(int value);
    void on_blend_left_exponent_spin_valueChanged(double arg1);
    void on_blend_left_exponent_slider_sliderMoved(int value);
    void on_blend_left_gamma_spin_valueChanged(double arg1);
    void on_blend_left_gamma_slider_sliderMoved(int value);

    //! 右融合带
    void on_blend_right_lume_spin_valueChanged(double arg1);
    void on_blend_right_lume_slider_sliderMoved(int value);
    void on_blend_right_exponent_spin_valueChanged(double arg1);
    void on_blend_right_exponent_slider_sliderMoved(int value);
    void on_blend_right_gamma_spin_valueChanged(double arg1);
    void on_blend_right_gamma_slider_sliderMoved(int value);

    //! 顶部融合带
    void on_blend_top_lume_spin_valueChanged(double arg1);
    void on_blend_top_lume_slider_sliderMoved(int value);
    void on_blend_top_exponent_spin_valueChanged(double arg1);
    void on_blend_top_exponent_slider_sliderMoved(int value);
    void on_blend_top_gamma_spin_valueChanged(double arg1);
    void on_blend_top_gamma_slider_sliderMoved(int value);

    //! 底部融合带
    void on_blend_bottom_lume_spin_valueChanged(double arg1);
    void on_blend_bottom_lume_slide_sliderMoved(int value);
    void on_blend_bottom_exponent_spin_valueChanged(double arg1);
    void on_blend_bottom_exponent_slider_sliderMoved(int value);
    void on_blend_bottom_gamma_spin_valueChanged(double arg1);
    void on_blend_bottom_gamma_slider_sliderMoved(int value);

    void on_ext_area_adjust_clicked();

private:
    int dock_direction;

    class gui_config *guicf;
    QVector<QVector <BlendConfig> > blend_config;

    area_adjust      *aadjust;
private:
    //! 接受命令
    virtual void command(const int& cmd);
    void dock_handle(int dir = 0);
    BlendConfig &current_data(const QString &arg1 = QString());
};

#endif // DEBUG_GUI_H
