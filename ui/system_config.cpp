#include "system_config.h"
#include <QSettings>


const QString conf_file_sys = QString("HKEY_CURRENT_USER//Software//Fusion_v1.3.1");
const QString conf_file_ini = QString("etc/system.ini");
system_config *system_config::_config = 0;

class system_config_p
{
public:
    QSettings *conf;
    system_config_p(QSettings::Format form)
    {
        if (form == QSettings::NativeFormat)
            conf = new QSettings(conf_file_sys, form);
        else if (form == QSettings::IniFormat)
            conf = new QSettings(conf_file_ini, form);

    }
    ~system_config_p()
    {
        delete conf;
    }
};

system_config::system_config(QSettings::Format form)
{
    _d = new system_config_p(form);
    system_config::_config = this;
}
void system_config::begin(const QString &prefix)
{
    _d->conf->beginGroup (prefix);
}
void system_config::end()
{
    _d->conf->endGroup ();
}
void system_config::value(const QString &key, const QVariant &val)
{
    _d->conf->setValue (key, val);
}
QVariant system_config::value(const QString &key)
{
    return _d->conf->value (key);
}
void system_config::remove(const QString &key)
{
    _d->conf->remove (key);
}
bool system_config::contains(const QString &key) const
{
    return _d->conf->contains (key);
}
system_config *system_config::config()
{
    return system_config::_config;
}
