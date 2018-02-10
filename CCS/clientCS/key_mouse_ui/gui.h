#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QTouchEvent>
#include <QTextEdit>
#include <QGridLayout>

class GUI : public QWidget
{
    Q_OBJECT
public:
    explicit GUI(QWidget *parent = 0);

protected:
    bool event(QEvent *);
    virtual void paintEvent(QPaintEvent *);

    QStringList text;
signals:
    void mouse(QMouseEvent*);
    void wheel(QWheelEvent*);
    void key(QKeyEvent*);

    void drop(QDropEvent*);
    void dragEnter(QDragEnterEvent*);
    void dragMove(QDragMoveEvent*);
    void dragLeave(QDragLeaveEvent*);
    void touch(QTouchEvent *);
public slots:

};

#endif // GUI_H
