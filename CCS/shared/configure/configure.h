#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QSettings>
#include <QtGlobal>

class configure
{
    QSettings *config;
public:
    configure();
    ~configure();

    QString addres();
    uint port();
    float heartbeat();
    void addres(const QString &ip);
    void port(uint po);
    void heartbeat(float vl);

    QStringList command();
    void command(const QStringList &list);

    bool isOk();

    static QString command_set(const QString &cmd, const QString &val);
    static QStringList command_get(const QString &cmd);
};

#endif // CONFIGURE_H
