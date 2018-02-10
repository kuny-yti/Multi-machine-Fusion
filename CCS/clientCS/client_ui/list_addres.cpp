#include "list_addres.h"
#include <QMessageBox>
#include <QDebug>

list_addres::list_addres(QWidget *parent):
    QListWidget(parent)
{
    conf_file = new configure();
    this->clear();
    this->setSelectionMode(QAbstractItemView::MultiSelection);
    _menu();
}

void list_addres::_menu()
{
    popMenu = new QMenu(this);

    act_disconnect =  new QAction(tr("disconnect"), this);
    connect(act_disconnect, SIGNAL(triggered()), this, SLOT(deleteItemAt()));

    player_menu = new QMenu(tr("Player cmd"), this);
    connect (player_menu, SIGNAL(triggered(QAction*)),
             this, SLOT(on_triggered_play(QAction*)));

    system_menu = new QMenu(tr("System cmd"), this);
    connect (system_menu, SIGNAL(triggered(QAction*)),
             this, SLOT(on_triggered_sys(QAction*)));

    QStringList cmd = conf_file->command_play ();
    for (int i = 0; i < cmd.size (); i++)
    {
        QStringList tem = conf_file->command_get (cmd[i]);
        player_menu->addAction(tem[0]);
    }

    cmd.clear();
    cmd += conf_file->command_system ();
    for (int i = 0; i < cmd.size (); i++)
    {
        QStringList tem = conf_file->command_get (cmd[i]);
        system_menu->addAction(tem[0]);
    }

}
void list_addres::on_triggered_play(QAction *act)
{
    QStringList cmd = conf_file->command_play ();

    QList<QListWidgetItem*> tem = this->selectedItems();
    QStringList addre;
    for (int i = 0; i < tem.size(); i++)
    {
        addre << tem[i]->text();
    }

    for (int i = 0; i < cmd.size (); i++)
    {
        QStringList tem = conf_file->command_get (cmd[i]);
        if (tem[0] == act->text ())
        {
            emit command(addre, cmd[i]);
        }
    }
}
void list_addres::on_triggered_sys(QAction *act)
{
    QStringList cmd = conf_file->command_system ();

    QList<QListWidgetItem*> tem = this->selectedItems();
    QStringList addre;
    for (int i = 0; i < tem.size(); i++)
    {
        addre << tem[i]->text();
    }

    for (int i = 0; i < cmd.size (); i++)
    {
        QStringList tem = conf_file->command_get (cmd[i]);
        if (tem[0] == act->text ())
        {
            emit command(addre, cmd[i]);
        }
    }
}
list_addres::~list_addres()
{
    delete act_disconnect;
    delete popMenu;
}
void list_addres::contextMenuEvent ( QContextMenuEvent *)
{
    if(this->itemAt(mapFromGlobal(QCursor::pos())) != NULL)
    {
        popMenu->clear();

        popMenu->addAction(act_disconnect);
        popMenu->addSeparator ();
        popMenu->addMenu (player_menu);
        popMenu->addSeparator ();
        popMenu->addMenu (system_menu);
        popMenu->addSeparator ();
        popMenu->exec(QCursor::pos());
    }
}

void list_addres::deleteItemAt()
{
    QList<QListWidgetItem*> tem = this->selectedItems();
    QStringList addre;
    for (int i = 0; i < tem.size(); i++)
    {
        addre << tem[i]->text();
        int r= this->row(tem[i]);
        delete this->takeItem(r);
    }
    emit discon_c(addre);
}

