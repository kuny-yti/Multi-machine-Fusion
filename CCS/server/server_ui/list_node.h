#ifndef LIST_NODE_H
#define LIST_NODE_H
#include<QWidget>
#include<QTableWidget>
#include<QMenu>
#include<QAction>

class list_node: public QTableWidget
{
    Q_OBJECT
private:
    QMenu* popMenu;
    QAction *act_disconnect;
    QAction *act_edit;
private:
    void contextMenuEvent (QContextMenuEvent *);
    void _menu();
private slots:
    void on_triggered_play(QAction *);
    void deleteItemAt();
    void editItemAt();
public:
    explicit list_node(QWidget *parent = 0);
    ~list_node();

    void append_node(int inx, float pos, const QString &name);
    virtual void paintEvent (QPaintEvent * event);
};

#endif // LIST_NODE_H
