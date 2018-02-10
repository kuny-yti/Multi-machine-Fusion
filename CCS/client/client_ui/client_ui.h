#ifndef WIDGET_H
#define WIDGET_H

#include "ui_client_ui.h"
#include "client_pro.h"
#include <QTimer>
#include <QSystemTrayIcon>
#ifdef _MESSAFE_LOGS_FILE_
#include "message.h"
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

class client_ui : public QWidget, private Ui::Widget
{
    Q_OBJECT
private:
    client_pro *client;
    static client_ui* _this;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QAction *startAction;
    QAction *messageAction;
    message::LogLevel logLevel;

private:
    void _read_command_list();
    void _save_command_list();

    void createTrayIcon();
    void createActions();
    void _print(message::LogLevel, const QVariant &val);
public:
    static void print_ui(message::LogLevel, const QVariant &val);

public:
    explicit client_ui(QWidget *parent = 0);
    void setVisible(bool visible);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_add_rows_clicked();
    void on_del_rows_clicked();
    void on_save_clicked();

     void on_message_leve();
};

#endif // WIDGET_H
