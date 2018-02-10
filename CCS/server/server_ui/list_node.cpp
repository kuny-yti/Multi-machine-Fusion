#include "list_node.h"
#include <QContextMenuEvent>

list_node::list_node(QWidget *parent):
    QTableWidget(parent)
{
    this->clear();
    this->setColumnCount(2);
    this->setRowCount(100);
    QStringList header;
    header<<tr("time node")<<tr("node name");
    this->setHorizontalHeaderLabels(header);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    _menu();
}
list_node::~list_node()
{
    delete act_disconnect;
    delete popMenu;
}

void list_node::_menu()
{
    popMenu = new QMenu(this);

    act_disconnect =  new QAction(tr("delete"), this);
    connect(act_disconnect, SIGNAL(triggered()), this, SLOT(deleteItemAt()));
    act_edit =  new QAction(tr("edit"), this);
    connect(act_disconnect, SIGNAL(triggered()), this, SLOT(editItemAt()));

}
void list_node::contextMenuEvent ( QContextMenuEvent *e)
{
    if(this->itemAt(e->pos()) != NULL)
    {
        popMenu->clear();

        popMenu->addAction(act_disconnect);
        popMenu->addSeparator ();
        popMenu->addAction(act_edit);
        popMenu->exec(QCursor::pos());
        e->accept();
    }
}
void list_node::on_triggered_play(QAction *)
{
    QList<QTableWidgetItem*> tem = this->selectedItems();
    QStringList addre;
    for (int i = 0; i < tem.size(); i++)
    {
        addre << tem[i]->text();
    }

}
void list_node::deleteItemAt()
{
    QList<QTableWidgetItem*> tem = this->selectedItems();
    //QStringList addre;
    for (int i = 0; i < tem.size(); i++)
    {
        //addre << tem[i]->text();
        int r= this->row(tem[i]);
        delete this->takeItem(r, 0);
        delete this->takeItem(r, 1);
    }
    //emit discon_c(addre);
}
void list_node::editItemAt()
{
    QList<QTableWidgetItem*> tem = this->selectedItems();
    for (int i = 0; i < tem.size(); i++)
    {
        int r= this->row(tem[i]);
        int c= this->column(tem[i]);
           this->setCurrentCell(i, c);
           this->openPersistentEditor(tem[i]); //打开编辑项
           this->editItem(tem[i]);
    }
}
void list_node::append_node(int inx, float pos, const QString &name)
{
    QStringList header;
    header<<tr("time node")<<tr("node name");
    this->setHorizontalHeaderLabels(header);

    this->setItem(inx, 0, new QTableWidgetItem(QString::number(pos)));
    this->setItem(inx, 1, new QTableWidgetItem(name));
}
void list_node::paintEvent (QPaintEvent * event)
{
    QTableWidget::paintEvent(event);
}
