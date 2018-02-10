#ifndef LIST_ADDRES_H
#define LIST_ADDRES_H

#include<QWidget>
#include<QListWidget>
#include<QMenu>
#include<QAction>
#include "configure.h"

class list_addres : public QListWidget
{
    Q_OBJECT
public:
    explicit list_addres(QWidget *parent = 0);
    ~list_addres();
    configure *config_file()
    {
        return conf_file;
    }
private:
    QMenu* popMenu;
    QAction *act_disconnect;

    QMenu* player_menu;
    configure *conf_file;

private:
    void contextMenuEvent (QContextMenuEvent *);

    void _menu();
signals:
    void discon_c(const QStringList &addres);
    void command(const QStringList &addres, const QString &cmd);

private slots:
    void deleteItemAt();

    void on_triggered_play(QAction*);
};

#endif // LIST_ADDRES_H
