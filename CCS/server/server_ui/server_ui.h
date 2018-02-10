#ifndef TCP_UI_H
#define TCP_UI_H

#include "ui_server_ui.h"
#include "server_pro.h"
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

class QTcpSocket;
class QThread;
class server_tcp;
class list_addres;
class command_execution;
class server_time;

class server_ui : public QWidget, private Ui::tcp_ui
{
    Q_OBJECT
private:
    server_pro *server;
    list_addres *list_addre;
    QGridLayout *list_layout;
    server_time *time_server_ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QAction *startAction;
    QAction *messageAction;
    message::LogLevel logLevel;

    static server_ui *ui;
    void _print(message::LogLevel, const QVariant &val);
    void createTrayIcon();
    void createActions();
private slots:
    void on_disconnect(const QString& ip);//断开
    void on_connect(QThread *ts,const QString& ip);//连接

    void on_cboxMax_currentIndexChanged(const QString &arg1);

    void on_btnListen_clicked();
    void on_btnClearServer_clicked();
    void on_btnSendServer_clicked();

    void on_discon_c(const QStringList &addres);
    void on_message_leve();
    void on_checkBox_win_start_stateChanged(int arg1);

    void on_btnPJlink_clicked();

    void on_btnPlayerNode_clicked();

public slots:
    void op_hide();
public:
    explicit server_ui(QWidget *parent = 0);
    ~server_ui();
    void setVisible(bool visible);
    static void print_ui(message::LogLevel, const QVariant &val);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // TCP_UI_H
