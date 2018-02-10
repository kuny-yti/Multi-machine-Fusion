#ifndef AREA_ADJUST_H
#define AREA_ADJUST_H

#include <QWidget>
#include "ui_area_adjust.h"
#include "ctrl.h"

class device_area_gui_p;
class area_adjust : public QWidget, private Ui::area_adjust, private ctrl
{
    Q_OBJECT
public:
    explicit area_adjust(QWidget *parent = 0);
    ~area_adjust();
    void callback(int cmd);
protected:
    bool event(QEvent *);
signals:

public slots:

private:
    virtual void command(const int &cmd);
private:
    device_area_gui_p *d;
};

#endif // AREA_ADJUST_H
