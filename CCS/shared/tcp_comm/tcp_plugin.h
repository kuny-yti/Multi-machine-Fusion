#ifndef TCP_OBJECT_H
#define TCP_OBJECT_H
#include "interfaces.h"

class s_tcp_implement;
class c_tcp_implement;
class tcp_ui;
class QMenu;
class QAction;

class tcp_plugin_p : public QObject
{
    Q_OBJECT
private:
    s_tcp_implement *_tcp_s;
    c_tcp_implement *_tcp_c;
    tcp_ui *_ui;

    QMenu *_menu;
    QWidget *_wid;
    QAction *_tcp_Server;
private slots:
    void on_hide();
public :
    tcp_plugin_p();

    bool initialize(QWidget *a, QMenu *e);
    void extensions();
    void *extensions_object();
};

class  PLUGINS_LIBRARY tcp_plugin :public interfaces
{
    Q_OBJECT
   #if QT_VERSION >= 0x050000
       PLUGIN_SYSTEM_H(org.PluginSystem.tcp_plugin,
                       tcp_plugin.json, tcp_plugin)
   #else
       Q_INTERFACES(interfaces)
   #endif
private:
    tcp_plugin_p *d_ptr;
public:
    tcp_plugin();

    /// str1 主窗体:主窗体为0时，启用内部无ui，str2 标签菜单
    virtual bool initialize(void *str1, void *str2);

    virtual void extensions();

    virtual void *extensions_object() ;
};

#endif // TCP_OBJECT_H
