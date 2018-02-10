#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H
#include <QFile>
#include <QFileInfo>
#include <QVariant>

class log_entry
{
    QFile *file;
    qint64 size;
public:
    log_entry(const QString &path);

    int wirte(const int , const QVariant & val);
};

#endif // LOG_ENTRY_H
