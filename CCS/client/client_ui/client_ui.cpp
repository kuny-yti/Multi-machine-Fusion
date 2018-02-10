#include "client_ui.h"
#include <QMessageBox>
#include <QTime>
#include <QDebug>
#include <QFile>
#include "c_tcp_implement.h"
//#include "transcoding.h"
#include <QTcpSocket>
#include <QCloseEvent>
#include <QMenu>

client_ui *client_ui::_this = 0;

/// 差配置文件校验功能
/// 差许可证功能(加入时间限制)
/// 差保护机制(守护进程)
/// 差保存后重新连接服务器
///
client_ui::client_ui(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

     client = new client_pro();

     logLevel = message::LOG_WARNING;
     createActions();
     createTrayIcon();
     trayIcon->setIcon(this->windowIcon());
     trayIcon->show();

    _this = this;
    log_object(&client_ui::print_ui);

    if (client->conf_file()->isOk())
    {
        configure_table->clear ();

        QString text = client->conf_file()->addres ()+":"+QString::number (client->conf_file()->port ());
        server_addres->setText (text);

        heartbeat_time->setValue (client->conf_file()->heartbeat ());

        client->init();
        _read_command_list();
    }
}

void client_ui::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}
void client_ui::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QWidget::setVisible(visible);
}
void client_ui::createActions()
{
    messageAction = new QAction(tr("LOG_WARNING"), this);

    connect(messageAction, SIGNAL(triggered()), this, SLOT(on_message_leve()));

    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}
void client_ui::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(messageAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
void client_ui::on_message_leve()
{
    switch (logLevel)
    {
        case message::LOG_INFO:
            logLevel = message::LOG_WARNING;
            messageAction->setText(tr("LOG_WARNING"));
            log_warning(tr("Log Level(%1) [%2]").arg(tr("LOG_WARNING"))
                            .arg(QTime::currentTime().toString("hh:mm:ss")));

        break;
        case message::LOG_WARNING:
            logLevel = message::LOG_ERROR;
            messageAction->setText(tr("LOG_ERROR"));
            log_warning(tr("Log Level(%1) [%2]").arg(tr("LOG_ERROR"))
                            .arg(QTime::currentTime().toString("hh:mm:ss")));

        break;
        case message::LOG_ERROR:
            logLevel = message::LOG_ABOUT;
            messageAction->setText(tr("LOG_ABOUT"));
            log_warning(tr("Log Level(%1) [%2]").arg(tr("LOG_ABOUT"))
                            .arg(QTime::currentTime().toString("hh:mm:ss")));

        break;
        case message::LOG_ABOUT:
            logLevel = message::LOG_INFO;
            messageAction->setText(tr("LOG_INFO"));
            log_warning(tr("Log Level(%1) [%2]").arg(tr("LOG_INFO"))
                            .arg(QTime::currentTime().toString("hh:mm:ss")));

        break;
    default:
        logLevel = message::LOG_WARNING;
        messageAction->setText(tr("LOG_WARNING"));
        log_warning(tr("Log Level(%1) [%2]").arg(tr("LOG_WARNING"))
                        .arg(QTime::currentTime().toString("hh:mm:ss")));

        break;
    }
    message::object()->print_level(logLevel);
}

static QStringList _command(const QString &cmd)
{
    QString c = cmd;
    QStringList list;
    list = c.split ("@");
    return list;
}
void client_ui::_read_command_list()
{
    int inx = 0;
    QStringList play ;
    client->command_read(play);
    configure_table->setItem (inx, 0, new QTableWidgetItem(play[0]));
    inx++;

    for (int i = 1; i < play.size (); i++)
    {
       QStringList lk = _command(play[i]);
       configure_table->setItem (inx, 0, new QTableWidgetItem(lk[0]));
       configure_table->setItem (inx, 1, new QTableWidgetItem("="));
       configure_table->setItem (inx, 2, new QTableWidgetItem(lk[1]));
       inx++;
    }
}
void client_ui::_save_command_list()
{
    QStringList play ;//= config.command_play ();
    QStringList system ;
    int a = 0;

    int count = configure_table->rowCount ();
    for (int i = 0; i < count; i++)
    {
        QTableWidgetItem * item = configure_table->item (i, 0);
        if (item)
            if (!(item->text ().contains ("[")))
            {
                QString tem;
                QTableWidgetItem* items = configure_table->item (i, 0);
                tem = items->text ();
                tem += "@";

                items = configure_table->item (i, 2);
                tem += items->text ();
                if (a == 0x01)
                    play << tem;
            }
            else
            {
                if (item->text ().contains ("[Player_Command]"))
                {
                    play << "[Command]";
                    a = 0x01;
                }
            }
    }

    client->command_save(play);
}
void client_ui::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            retranslateUi(this);
        break;
        default:
        break;
    }
}

void client_ui::on_add_rows_clicked()
{
    configure_table->setRowCount (configure_table->rowCount ()+1);
    log_information(tr("add command list. [time:%1]")
                         .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void client_ui::on_del_rows_clicked()
{
    configure_table->setRowCount (configure_table->rowCount ()-1);
    log_information(tr("delete command list. [time:%1]")
                         .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void client_ui::on_save_clicked()
{
    QStringList ip = server_addres->text ().split (":");
    if (ip.size () >= 2) {}
    else
    {
        QMessageBox::warning (this, tr("server addres error"),
                              tr("server addres error. Format(ip:port)"));
        return;
    }
    client->conf_file()->addres (ip[0]);
    client->conf_file()->port (ip[1].toUInt ());
    float val = heartbeat_time->value ();
    client->conf_file()->heartbeat (val);
    _save_command_list();

    log_information(tr("save configure success. [time:%1]")
                         .arg(QTime::currentTime().toString("hh:mm:ss")));

}

void client_ui::_print(message::LogLevel lev, const QVariant &val)
{
    QString level;
    switch (lev)
    {
        case message::LOG_INFO:
            level = tr("[INFO] ");
        break;
        case message::LOG_WARNING:
            level = tr("[WARNING] ");
        break;
        case message::LOG_ERROR:
            level = tr("[ERROR] ");
        break;
        case message::LOG_ABOUT:
            level = tr("[ABOUT] ");
        break;
    default:
        break;
    }
    log_text->append(level + val.toString());
    //trayIcon->showMessage("Client", level+val.toString());
}
void client_ui::print_ui(message::LogLevel lev, const QVariant &val)
{
    _this->_print(lev, val);
}
