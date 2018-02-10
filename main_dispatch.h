#ifndef MAIN_DISPATCH_H
#define MAIN_DISPATCH_H

#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTimer>

#include "system_config.h"
//#include "dispatch.h"

#include "tools_gui.h"
#include "debug_gui.h"
#include "ctrl.h"
#include "opengl_viewer.h"
#include "message.h"
#include "gui_config.h"

class trayicon :public QWidget
{
    Q_OBJECT
public:
    trayicon(QWidget *parent = NULL):
        QWidget(parent),
        tray(NULL),
        tray_menu(NULL)
    {

    }
    ~trayicon()
    {
        uninstall();
    }
    void install(QWidget *parent = NULL);
    void uninstall();
    QTimer licesne_time;
private slots:
    void tray_actived(QSystemTrayIcon::ActivationReason ar);
    void exit_system();
    void display_desktop();
    void reduction_display();
    void show_tools();
    void hide_tools();

private:
    QSystemTrayIcon *tray;
    QMenu *tray_menu;

    QAction *tray_exit;//! 退出系统
    QAction *tray_show;//! 显示上一次的状态
    QAction *tray_hide;//! 隐藏当前在显示的

    QAction *tray_show_tools; //! 显示工具栏
    QAction *tray_hide_tools; //! 隐藏工具栏

};
class main_dispatch : public ctrl
{
public:
    main_dispatch(ctrl_core *cc);
    ~main_dispatch();

    void start();
    void exit();
    virtual void command(const int& cmd);
private:
    void save_data();
    void load_data();
private:
    tools_gui *tools;
    debug_gui *adjust;
    opengl_viewer *opengl;
    system_config syscf;
    gui_config guicf;
    message *logs;
    ctrl_core *control_core;
    //dispatch  media_dispatch;
    trayicon tray;

    bool tools_show;
    bool adjust_show;
    bool show_state;
};
#endif // MAIN_DISPATCH_H
