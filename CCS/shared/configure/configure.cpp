#include "configure.h"
#include <QStringList>

configure::configure()
{
    config = new QSettings("./network.ini", QSettings::IniFormat);
}
configure::~configure()
{
    delete config;
}
bool configure::isOk()
{
    return config->isWritable ();
}
QString configure::addres()
{
    QString s;
    config->beginGroup ("Server");
    s = config->value ("addres").toString ();
    config->endGroup ();
    return s;
}
uint configure::port()
{
    uint s;
    config->beginGroup ("Server");
    s = config->value ("port").toUInt ();
    config->endGroup ();
    return s;
}
float configure::heartbeat()
{
    double s;
    config->beginGroup ("Server");
    s = config->value ("heartbeat").toFloat ();
    config->endGroup ();
    return s;
}

QStringList configure::command()
{
    QStringList cl;
    config->beginGroup ("Command");
    int size = config->value ("list").toInt ();

    for (int i = 0; i < size; i++)
    {
        cl << config->value(QString::number (i)).toString ();
    }
    config->endGroup();
    return cl;
}
void configure::command(const QStringList &list)
{
    config->beginGroup ("Command");
    config->setValue ("list", list.size ());
    for (int i = 0; i < list.size (); i++)
    {
        config->setValue(QString::number (i), list[i]);
    }
    config->endGroup();
    config->sync();
}

void configure::addres(const QString &ip)
{
    config->beginGroup ("Server");
    config->setValue ("addres", ip);
    config->endGroup ();
    config->sync ();
}
void configure::port(uint po)
{
    config->beginGroup ("Server");
    config->setValue ("port", po);
    config->endGroup ();
    config->sync ();
}
void configure::heartbeat(float vl)
{
    config->beginGroup ("Server");
    config->value ("heartbeat", QVariant(vl));
    config->endGroup ();
    config->sync ();
}

QString configure::command_set(const QString &cmd, const QString &val)
{
    QString out;
    out = cmd;
    out += "@";
    out += val;
    return out;
}
QStringList configure::command_get(const QString &cmd)
{
    QString c = cmd;
    QStringList list;
    list = c.split ("@");
    return list;
}
