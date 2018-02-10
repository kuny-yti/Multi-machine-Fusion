#ifndef COMPUTER_GUI_H
#define COMPUTER_GUI_H

#include "ui_computer_gui.h"

class computer_gui : public QWidget, private Ui::computer_gui
{
    Q_OBJECT

public:
    explicit computer_gui(QWidget *parent = 0);

    void compute_layout(int);
    void compute_index(int);
    void geometry_control(int);
    void geometry_precision(int);
    int compute_layout()const;
    int compute_index()const;
    int geometry_control()const;
    int geometry_precision()const;
protected:
    void changeEvent(QEvent *e);
private slots:
    //! 设置计算机布局(默认1*1-高级模式使用)
    void on_device_computer_layout_v_activated(int index);
    void on_device_computer_layout_h_activated(int index);
    //! 设置计算机索引(默认1*1-高级模式使用)
    void on_device_computer_index_v_activated(int index);
    void on_device_computer_index_h_activated(int index);

    //! 设置控制点数(默认2*2)
    void on_geometry_control_v_activated(int index);
    void on_geometry_contro_h_activated(int index);
    //! 设置Mesh调试精度(默认8*8-高级模式使用)
    void on_geometry_precision_v_activated(int index);
    void on_geometry_precision_h_activated(int index);
};

#endif // COMPUTER_GUI_H
