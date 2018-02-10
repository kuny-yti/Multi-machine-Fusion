#include "main_dispatch.h"

const QRect toDesktopGeometry()
{
    uint screenNumber = 0;
    uint screenHeightMax = 0;
    uint screenHeightMaxBak = 0;

    QRect rect;
    QDesktopWidget *desktopWidget = QApplication::desktop();
    if (desktopWidget->screenCount())
    {
       screenNumber =  desktopWidget->screenCount();
    }
    else
    {
        return QRect();
    }

    for (uint i = 0; i < screenNumber; i++)
    {
        screenHeightMax = desktopWidget->screenGeometry(i).height();
        if (screenHeightMax > screenHeightMaxBak)
        {
            screenHeightMaxBak = screenHeightMax;
            rect.setHeight(screenHeightMax);
        }
        rect.setWidth(desktopWidget->screenGeometry(i).width() + rect.width());
    }
    return rect;
}

void trayicon::install(QWidget *parent)
{
    if (tray)
        return;

    tray = new QSystemTrayIcon(parent);
    tray->setIcon(QIcon(":/image/yantu.ico"/*":/image/yantu.ico"*/));
    tray_menu = new QMenu(parent);

    tray_exit = new QAction(tr("Exit System"), parent);
    tray_show = new QAction(tr("Display Desktop"), parent);
    tray_hide = new QAction(tr("Reduction Display"), parent);

    tray_show_tools = new QAction(tr("Show Tools"), parent);
    tray_hide_tools = new QAction(tr("Hide Tools"), parent);

    tray_menu->addAction(tray_exit);
    tray_menu->addSeparator();
    tray_menu->addAction(tray_show);
    tray_menu->addAction(tray_hide);
    tray_menu->addSeparator();
    tray_menu->addAction(tray_show_tools);
    tray_menu->addAction(tray_hide_tools);

    connect(tray_exit, &QAction::triggered, this, &trayicon::exit_system);
    connect(tray_show, &QAction::triggered, this, &trayicon::display_desktop);
    connect(tray_hide, &QAction::triggered, this, &trayicon::reduction_display);
    connect(tray_show_tools, &QAction::triggered, this, &trayicon::show_tools);
    connect(tray_hide_tools, &QAction::triggered, this, &trayicon::hide_tools);

    tray->setContextMenu(tray_menu);
    connect(tray, &QSystemTrayIcon::activated, this, &trayicon::tray_actived);

    tray->show();

    lic_count = 0;
    connect(&licesne_time, &QTimer::timeout, this, &trayicon::on_license_time);

    log_information("Tray install success.");
}
void trayicon::uninstall()
{
    if (tray_exit)
        delete tray_exit;
    if (tray_show)
        delete tray_show;
    if (tray_hide)
        delete tray_hide;
    if (tray_show_tools)
        delete tray_show_tools;
    if (tray_hide_tools)
        delete tray_hide_tools;
    if (tray_menu)
        delete tray_menu;
    if (tray)
        delete tray;

    tray_exit = NULL;
    tray_show = NULL;
    tray_hide = NULL;
    tray_show_tools = NULL;
    tray_hide_tools = NULL;
    tray_menu = NULL;
    tray = NULL;
    lic_count = 0;
    log_information("Tray uninstall success.");
}
void trayicon::on_license_time()
{

}
void trayicon::tray_actived(QSystemTrayIcon::ActivationReason ar)
{
    switch(ar)
    {
    //点击托盘显示窗口
    case QSystemTrayIcon::Trigger:
    {
        controls(Command_ShowTools);
        log_information("Tray trigger.");
        break;
    }
        //双击托盘显示窗口
    case QSystemTrayIcon::DoubleClick:
    {
        controls(Command_HideTools);
        log_information("Tray double click.");
        break;
    }
    default:
        break;
    }
}
void trayicon::exit_system()
{
    controls(Command_ExitSystem);
}
void trayicon::display_desktop()
{
    controls(Command_ShowDesktop);
}
void trayicon::reduction_display()
{
    controls(Command_ReductionShow);
}
void trayicon::show_tools()
{
     controls(Command_ShowTools);
}
void trayicon::hide_tools()
{
    controls(Command_HideTools);
}

main_dispatch::main_dispatch(ctrl_core *cc):
    syscf(QSettings::IniFormat),
    guicf(&syscf),
    control_core(cc),
    opengl(NULL),
    tools(NULL),
    adjust(NULL),
    logs(NULL),
    tray()
{

}
main_dispatch::~main_dispatch()
{
}


void main_dispatch::start()
{
    //! 所有配置这里进行原始数据初始化
    control(Command_ConfigLoad);

    logs = new message(raw_config().dir_config_log + QString(raw_config().logs_filename));
    logs->print_level((message::LogLevel) raw_config().logs_level);

    logs->information("-----------------------------------");
    logs->information("           Fusion System.          ");
    logs->information("          vision ("+QString(FusionVersionStr())+")     ");
    logs->information("Shenzhen YanTu Technology Co., Ltd.");
    logs->information("          www.ytsmart.net          ");
    logs->information("-----------logging begin-----------");


    tools = new tools_gui(&guicf);
    adjust = new debug_gui(&guicf);
    tools->show();
    adjust->show();
    adjust->hide();

    tray.install();
}
void main_dispatch::exit()
{
    tools->handle_exit();

    if (adjust)
        delete adjust;
    if (tools)
        delete tools;
    if (opengl)
        delete opengl;
    adjust = NULL;
    tools = NULL;
    opengl = NULL;

    tray.uninstall();
    logs->information("------------logging end------------");
    logs->information("        exit fusion system.        ");
    logs->information("-----------------------------------");
    logs->information("\n\n");
}
void main_dispatch::command(const int& cmd)
{
    switch (cmd) {
    case Command_ConfigSave:
        break;
    case Command_SystemLoad:
        //load_data();
        break;
    case Command_SystemSave:
        //save_data();
        break;

    case Command_ExitSystem:
    {
        //! 要处理未保存数据
        exit();
    }
        break;
    case Command_StartOpenGL:
    {
        if (opengl)
            delete opengl;

        opengl = new opengl_viewer();
        QRect geomet(raw_config().display_window[0], raw_config().display_window[1],
                raw_config().display_window[2],raw_config().display_window[3]);

        if (geomet.isEmpty ())
        {
            geomet = toDesktopGeometry();
            raw_config().display_window[0] =geomet.x();
            raw_config().display_window[1] = geomet.y();
            raw_config().display_window[2] = geomet.width();
            raw_config().display_window[3] = geomet.height();
        }

        opengl->setGeometry (geomet);
        opengl->show();
        control(Command_StartDebug);
    }
        break;
    case Error_License:
        if (opengl)
            delete opengl;
        opengl = NULL;
        break;
    case Command_ShowDesktop:
        if (tools && tools->isHidden())
            tools_show = false;
        else
            tools_show = true;
        if (adjust && adjust->isHidden())
            adjust_show = false;
        else
            adjust_show = true;

        if (tools)
            tools->hide();
        if (adjust)
            adjust->hide();
        if (opengl)
            opengl->hide();
        show_state = false;
        break;
    case Command_ReductionShow:
        if (show_state)
            break;
        if (opengl)
            opengl->show();
        if (tools)
            tools->show();
        if (adjust && adjust_show)
            adjust->show();
        show_state = true;
        break;
    default:
        break;
    }
}

void main_dispatch::save_data()
{

}
void main_dispatch::load_data()
{

}
