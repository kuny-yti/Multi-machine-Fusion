#include "pjlink.h"

PJlink::PJlink(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void PJlink::changeEvent(QEvent *e)
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
