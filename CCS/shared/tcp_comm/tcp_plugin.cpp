#include "tcp_plugin.h"
#include "tcp_implement.h"
#include "tcp_ui.h"
#include <QMainWindow>
#include <QMenu>
#include <QVBoxLayout>

tcp_plugin_p::tcp_plugin_p()
{
    _tcp_s = 0;
    _tcp_c = 0;
    _ui = 0;

    _menu = 0;
}

void tcp_plugin_p::on_hide()
{
    if (_tcp_Server->isCheckable ())
    {
        _ui->setHidden (true);
        _tcp_Server->setCheckable (false);
    }
    else
    {
        _ui->setHidden (false);
        _ui->show ();
        _tcp_Server->setCheckable (true);
    }
}
bool tcp_plugin_p::initialize(QWidget *a, QMenu *e)
{
    _menu = e;
    _wid = a;

    if (_menu != 0)
    {
        _tcp_Server =
                _menu->addAction(QIcon(":/ico/ico/home_server.png"), tr("TCP Server"));
        _tcp_Server->setCheckable (false);

        _ui = new tcp_ui();
        connect (_tcp_Server, &QAction::triggered, this, &tcp_plugin_p::on_hide);
        return _ui;
    }
    else
    {
        if (_wid == 0)
        {
             _tcp_c = new c_tcp_implement();
             return _tcp_c;
        }
        else
        {
            _tcp_s = new s_tcp_implement();
            return _tcp_s;
        }
    }
}

void tcp_plugin_p::extensions()
{
}

void *tcp_plugin_p::extensions_object()
{
    if (_menu != 0)
    {
        return (void*)_ui;
    }
    else if (_wid)
    {
        return (void*)_tcp_s ;
    }
    else
    {
        return (void*)_tcp_c ;
    }
}

tcp_plugin::tcp_plugin()
{
    d_ptr = new tcp_plugin_p;
}
bool tcp_plugin::initialize(void *str1, void *str2)
{
    return d_ptr->initialize ((QWidget*)str1, (QMenu*)str2);
}

void tcp_plugin::extensions()
{
    d_ptr->extensions ();
}

void *tcp_plugin::extensions_object()
{
    return d_ptr->extensions_object ();
}

PLUGIN_SYSTEM_C(org.PluginSystem.tcp_plugin, tcp_plugin)
