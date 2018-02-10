#ifndef COLOR_GUI_H
#define COLOR_GUI_H

#include "ui_color_gui.h"
#include "ctrl.h"

class color_gui : public QWidget, private Ui::color_gui, private ctrl
{
    Q_OBJECT

public:
    explicit color_gui(QWidget *parent = 0);

    void device_id(int);
    int device_id()const;
private:
    //! 接受命令
    virtual void command(const int& cmd);
    ColorConfig &current_color(const QString &arg1 = QString());
protected:
    void changeEvent(QEvent *e);
private slots:
    //! 以下-高级模式使用
    //! 选择设备id
    void on_color_device_id_activated(int index);
    //! 设置设备亮度
    void on_color_brightness_spin_valueChanged(double arg1);
    void on_color_brightness_reset_clicked();
    void on_color_brightness_slider_sliderMoved(int value);
    //! 设置设备对比度
    void on_color_contrast_slider_sliderMoved(int value);
    void on_color_contrast_spin_valueChanged(double arg1);
    void on_color_contrast_reset_clicked();
    //! 设置设备色调
    void on_color_hue_slider_sliderMoved(int value);
    void on_color_hue_spin_valueChanged(double arg1);
    void on_color_hue_reset_clicked();
    //! 设置设备饱和度
    void on_color_saturation_slider_sliderMoved(int value);
    void on_color_saturation_spin_valueChanged(double arg1);
    void on_color_saturation_reset_clicked();

    //! 设置设备基色
    void on_color_red_slider_sliderMoved(int value);
    void on_color_red_spin_valueChanged(double arg1);
    void on_color_red_reset_clicked();

    void on_color_green_slider_sliderMoved(int value);
    void on_color_green_spin_valueChanged(double arg1);
    void on_color_green_reset_clicked();

    void on_color_blue_slider_sliderMoved(int value);
    void on_color_blue_spin_valueChanged(double arg1);
    void on_color_blue_teset_clicked();

    void on_color_alpha_slider_sliderMoved(int value);
    void on_color_alpha_spin_valueChanged(double arg1);
    void on_color_alha_reset_clicked();
private:
     QVector <QVector <ColorConfig> > color_config;//! 颜色信息保存
};

#endif // COLOR_GUI_H
