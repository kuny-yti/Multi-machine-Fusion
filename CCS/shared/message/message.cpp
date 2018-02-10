#include "message.h"
#include <QDebug>

static bool qdebug = true;
static message::print p_object = NULL;
static message::LogLevel levels;

message *message::log = NULL;

message::message(const QString &path)
{
    levels = LOG_WARNING;
    log = this;
    if (path.isEmpty())
        log_file = new log_entry("logging.log");
    else
        log_file = new log_entry(path);
}
message::~message()
{
    if (log_file)
        delete log_file;
}
void message::print_level(message::LogLevel lev)
{
    levels = lev;
}
void message::print_debug(bool b)
{
   qdebug = b;
}
message *message::object()
{
    return log;
}

void message::print_object(print func)
{
    p_object = func;
}

void message::informations(const char *msg, ...) const
{
    QString message;

    va_list ap;
    va_start(ap, msg); // use variable arg list
    message = QString().vsprintf(msg, ap);
    information(message);
    va_end(ap);
}
void message::warnings(const char *msg, ...) const
{
    QString message;

    va_list ap;
    va_start(ap, msg); // use variable arg list
    message = QString().vsprintf(msg, ap);
    warning(message);
    va_end(ap);
}
void message::errors(const char *msg, ...) const
{
    QString message;

    va_list ap;
    va_start(ap, msg); // use variable arg list
    message = QString().vsprintf(msg, ap);
    error(message);
    va_end(ap);
}
void message::abouts(const char *msg, ...) const
{
    QString message;

    va_list ap;
    va_start(ap, msg); // use variable arg list
    message = QString().vsprintf(msg, ap);
    about(message);
    va_end(ap);
}

void message::warning(const QVariant &val)
{
    if (qdebug) qWarning() << val.toString();
    if (levels <= LOG_WARNING)
    {
        object()->log_file->wirte(LOG_WARNING, val);
        if (p_object)
            p_object(LOG_WARNING, val);
    }
}
void message::about(const QVariant &val)
{
    if (levels <= LOG_ABOUT)
    {
        object()->log_file->wirte(LOG_ABOUT, val);
        if (p_object)
            p_object(LOG_ABOUT, val);
        ::abort();
    }
}
void message::error(const QVariant &val)
{
    if (qdebug) qCritical() << val.toString();
    if (levels <= LOG_ERROR)
    {
        object()->log_file->wirte(LOG_ERROR, val);
        if (p_object)
            p_object(LOG_ERROR, val);
    }
}
void message::information(const QVariant &val)
{
    if (qdebug) qDebug() << val.toString();
    if (levels <= LOG_INFO)
    {
        object()->log_file->wirte(LOG_INFO, val);
        if (p_object)
            p_object(LOG_INFO, val);
    }
}
