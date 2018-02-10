#ifndef PJLINK_H
#define PJLINK_H

#include "ui_pjlink.h"

class PJlink : public QWidget, private Ui::PJlink
{
    Q_OBJECT

public:
    explicit PJlink(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // PJLINK_H
