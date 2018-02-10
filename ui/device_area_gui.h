#ifndef DEVICE_AREA_GUI_H
#define DEVICE_AREA_GUI_H

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "ctrl.h"

class device_area_gui_p;
class device_area_gui : public QWidget, private ctrl
{
    Q_OBJECT
public:
    explicit device_area_gui(QWidget *parent = 0);
    ~device_area_gui();
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

#endif // DEVICE_AREA_GUI_H
