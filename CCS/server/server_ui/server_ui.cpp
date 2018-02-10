#include "server_ui.h"
#include <QTCPSocket>
#include <QThread>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QString>
#include "server_tcp.h"
#include "list_addres.h"
#include "transcoding.h"
#include "command_execution.h"
#include "server_time.h"

server_ui *server_ui::ui = NULL;

server_ui::server_ui(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    server = 0;
    time_server_ui = new server_time();
    logLevel = message::LOG_WARNING;

    btnSendServer->setEnabled(false);
    txtSendServer->setEnabled(false);
    txtDataServer->setReadOnly(true);
    cboxMax->setEnabled(false);

    for (int i = 10;i <= 1000; i = i+10)
    {
        cboxMax->addItem(QString::number(i));
    }
    cboxMax->setCurrentText("200");
    list_addre = new list_addres(this);

    connect (list_addre, SIGNAL(discon_c(QStringList)),
             this, SLOT(on_discon_c(QStringList)));

    list_addre->setEnabled (false);

    list_layout = new QGridLayout(listWidget);
    list_layout->addWidget(list_addre);
    list_layout->setHorizontalSpacing (0);
    list_layout->setVerticalSpacing (0);
    list_layout->setMargin (0);

    listWidget->setLayout(list_layout);

    createActions();
    createTrayIcon();
    trayIcon->setIcon(this->windowIcon());
    trayIcon->show();

    time_server_ui->setHidden(true);

    ui = this;
    log_object(&server_ui::print_ui);
    log_information(tr("start system[%1]").arg(QTime::currentTime().toString("hh:mm:ss")));

    uint port = list_addre->config_file()->port();
    txtPortServer->setText(QString::number(port));
    on_btnListen_clicked();

}

server_ui::~server_ui()
{
    if (server)
    {
        server->stop();
        delete server;
    }
}
void server_ui::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}
void server_ui::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QWidget::setVisible(visible);
}
void server_ui::createActions()
{
    startAction = new QAction(tr("Start"), this);
    messageAction = new QAction(tr("LOG_WARNING"), this);

    startAction->setText(tr("Start"));
    startAction->setIcon(QIcon(":/ico/ico/start.png"));
    connect(startAction, SIGNAL(triggered()), this, SLOT(on_btnListen_clicked()));
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
void server_ui::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(startAction);
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
void server_ui::on_message_leve()
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

void server_ui::print_ui(message::LogLevel lev, const QVariant &val)
{
    ui->_print(lev, val);
}
void server_ui::_print(message::LogLevel lev, const QVariant &val)
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
    txtDataServer->append(level+val.toString());
    trayIcon->showMessage("Server", level+val.toString());
}

void server_ui::changeEvent(QEvent *e)
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

void server_ui::on_disconnect(const QString& ip)
{
    QList<QListWidgetItem*> tem = list_addre->findItems(ip, Qt::MatchCaseSensitive);
    for (int i = 0; i < tem.size(); i++)
    {
        int r= list_addre->row(tem[i]);
        delete list_addre->takeItem(r);
    }
}
void server_ui::on_connect(QThread *,const QString& ip)
{
    QListWidgetItem*    oneItem = new QListWidgetItem(ip, list_addre);
    oneItem->setFlags(oneItem->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    oneItem->setCheckState(Qt::Unchecked); // AND initialize check state
}

void server_ui::on_cboxMax_currentIndexChanged(const QString &arg1)
{
    if (server)
    {
        server->listen_max(arg1.toInt());
    }
}

void server_ui::on_btnListen_clicked()
{
    //server_start
    if (!server)
    {
        server = new server_pro (list_addre->config_file());
        connect (list_addre, SIGNAL(command(QStringList, QString)),
                 server, SLOT(on_command(QStringList, QString)));
        connect (server, SIGNAL(s_disconnect(QString)),
                 this, SLOT(on_disconnect(QString)));
        connect (server, SIGNAL(s_connect(QThread *, QString)),
                 this, SLOT(on_connect(QThread *, QString)));
         connect(time_server_ui, &server_time::command_exec,
                 server, &server_pro::on_command_exec);
         connect(server, &server_pro::clock_run,
                 time_server_ui, &server_time::update_seek);

        list_addre->config_file()->port(txtPortServer->text().toInt());
        bool ret = server->start(txtPortServer->text().toInt());
        if (ret)
        {
            startAction->setText(tr("Stop"));
            startAction->setIcon(QIcon(":/ico/ico/stop.png"));

            btnListen->setText(tr("Stop"));
            btnListen->setIcon(QIcon(":/ico/ico/stop.png"));
            btnListen->setIconSize(QSize(32, 32));
            cboxMax->setEnabled(true);
            btnSendServer->setEnabled(true);
            txtSendServer->setEnabled(true);
            txtPortServer->setEnabled(false);
            list_addre->setEnabled (true);
        }
    }
    else
    {
        server->stop();
        delete server;
        server = 0;

        startAction->setText(tr("Start"));
        startAction->setIcon(QIcon(":/ico/ico/start.png"));

        btnListen->setText(tr("Start"));
        btnListen->setIcon(QIcon(":/ico/ico/start.png"));
        btnListen->setIconSize(QSize(32, 32));

        cboxMax->setEnabled(false);
        btnSendServer->setEnabled(false);
        txtSendServer->setEnabled(false);
        txtPortServer->setEnabled(true);
        list_addre->setEnabled (false);
    }
}

void server_ui::on_btnClearServer_clicked()
{
    //server_clean
    txtDataServer->clear();
    log_information(tr("clean data. [time:%1]").arg(QTime::currentTime().toString("hh:mm:ss")));
}

void server_ui::on_btnSendServer_clicked()
{
    //server_send
    if (server)
    {
        QString txt = txtSendServer->text();
        if (txt.isEmpty()) return;

        for (int i = 0; i < list_addre->count(); i++)
        {
            if (list_addre->item(i)->isSelected())
            {
                QString ip = list_addre->item(i)->text();
                if (ip.isEmpty()) return;

                server->send(ip, txt.toUtf8());
            }
        }
    }
}

void server_ui::op_hide()
{
    if (this->isHidden ())
    {
        this->setHidden (false);
    }
    else
    {
        this->setHidden (true);
    }
}

void server_ui::on_discon_c(const QStringList &addres)
{
    for (int i = 0; i < addres.size (); i++)
    {
        server->disc_client (addres[i]);
    }
}

const QString win8_start =
        QString("C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Startup/");
void server_ui::on_checkBox_win_start_stateChanged(int arg1)
{
    if (arg1)
    {
        QString curren_run_file = QApplication::applicationFilePath();
        QFile::link (curren_run_file, win8_start+"Fusion_v1.1.lnk");
    }
    else
    {
        QFile::remove (win8_start+"Fusion_v1.1.lnk");
    }
}

void server_ui::on_btnPJlink_clicked()
{

}

void server_ui::on_btnPlayerNode_clicked()
{
    if (time_server_ui->isHidden ())
    {
        time_server_ui->setHidden (false);
    }
    else
    {
        time_server_ui->setHidden (true);
    }
}
