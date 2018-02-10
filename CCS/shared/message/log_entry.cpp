#include "log_entry.h"
#include <QTextStream>
#include <QTime>
#include "message.h"

log_entry::log_entry(const QString &path)
{
    size = 1024 * 1024 *50;
    file =  new QFile(path);

}

int log_entry::wirte(const int le, const QVariant &val)
{
    if (file->size() > size) file->remove();

    file->open(QIODevice::ReadWrite| QIODevice::Text);
    file->readAll();

    QTextStream in(file);
    QString out;
    switch (le)
    {
    case message::LOG_INFO:
        out = "[INFO] ";
        break;
    case message::LOG_WARNING:
        out = "[WARNING] ";
        break;
    case message::LOG_ERROR:
        out = "[ERROR] ";
        break;
    case message::LOG_ABOUT:
        out = "[ABOUT] ";
        break;
    default:
        break;
    }
    in << QTime::currentTime().toString(Qt::ISODate);
    in << " ";
    in << (out +val.toString());
    in << endl;
    file->close();
    return 0;
}
