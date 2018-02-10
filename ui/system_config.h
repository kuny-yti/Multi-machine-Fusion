#ifndef SYSTEM_CONF_H
#define SYSTEM_CONF_H

#include <QObject>
#include <QSettings>

class system_config_p;
class system_config
{
    system_config_p *_d;
    static system_config *_config;
public:
    explicit system_config(QSettings::Format form);

    void begin(const QString &prefix);
    void end();
    void value(const QString &name, const QVariant &val);
    QVariant value(const QString &name);
    void remove(const QString &key);
    bool contains(const QString &key) const;

public:
    static system_config *config();
};

#define config() system_config::config()
#endif // SYSTEM_CONF_H
