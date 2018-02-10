#ifndef MESSAGE_H
#define MESSAGE_H

#include <QFile>
#include <QVariant>
#include "log_entry.h"
///
/// \brief The message class
/// log file printf message
///
#ifdef _MESSAFE_LOGS_FILE_
#    define log_object(val) message::print_object(val)
#    define log_warning(val) message::warning(val)
#    define log_about(val) message::about(val)
#    define log_error(val) message::error(val)
#    define log_information(val) message::information(val)
#else
#    define log_object(val)
#    define log_warning(val)
#    define log_about(val)
#    define log_error(val)
#    define log_information(val)
#endif

class message
{
public:
    typedef enum
    {
        LOG_INFO = 0x0,
        LOG_WARNING,
        LOG_ERROR,
        LOG_ABOUT,
    }LogLevel;

    typedef void (*print)(message::LogLevel, const QVariant &val);

private:
    static message *log;
    log_entry *log_file;
public:
    message(const QString &path = QString());
    ~message();

    void print_level(message::LogLevel);
    void print_debug(bool);


    void informations(const char *msg, ...) const
    #if defined(Q_CC_GNU) && !defined(__INSURE__)
    #  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
        __attribute__ ((format (gnu_printf, 2, 3)))
    #  else
        __attribute__ ((format (printf, 2, 3)))
    #  endif
    #endif
        ;

    void warnings(const char *msg, ...) const
    #if defined(Q_CC_GNU) && !defined(__INSURE__)
    #  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
        __attribute__ ((format (gnu_printf, 2, 3)))
    #  else
        __attribute__ ((format (printf, 2, 3)))
    #  endif
    #endif
        ;
    void errors(const char *msg, ...) const
    #if defined(Q_CC_GNU) && !defined(__INSURE__)
    #  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
        __attribute__ ((format (gnu_printf, 2, 3)))
    #  else
        __attribute__ ((format (printf, 2, 3)))
    #  endif
    #endif
        ;

    #ifndef Q_CC_MSVC
        Q_NORETURN
    #endif
        void abouts(const char *msg, ...) const
    #if defined(Q_CC_GNU) && !defined(__INSURE__)
    #  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
        __attribute__ ((format (gnu_printf, 2, 3)))
    #  else
        __attribute__ ((format (printf, 2, 3)))
    #  endif
    #endif
        ;
public:
    static void warning(const QVariant &val);//警告
    static void about(const QVariant &val);//退出系统
    static void error(const QVariant &val);//错误
    static void information(const QVariant &val);//信息

public:
    static message *object();
    static void print_object(print func);
};

//#define qDebug QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug
#endif // MESSAGE_H
