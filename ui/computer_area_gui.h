#ifndef COMPUTER_AREA_GUI_H
#define COMPUTER_AREA_GUI_H

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include "ctrl.h"

class computer_area_gui_p;
class computer_area_gui : public QWidget, private ctrl
{
    Q_OBJECT
public:
    explicit computer_area_gui(QWidget *parent = 0);
    ~computer_area_gui();

    void callback();
protected:
    bool event(QEvent *);
signals:

public slots:

private:
    virtual void command(const int &cmd);
private:
    computer_area_gui_p *d;
};

#endif // COMPUTER_AREA_GUI_H
