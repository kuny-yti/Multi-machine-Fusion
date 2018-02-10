#include "computer_gui.h"
#include "ui_define.h"

computer_gui::computer_gui(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    for (int i = 1; i <= 9; ++i)
    {
        device_computer_layout_v->addItem(QString::number(i));
        device_computer_layout_h->addItem(QString::number(i));
        device_computer_index_v->addItem(QString::number(i));
        device_computer_index_h->addItem(QString::number(i));
    }
    for (int i = 2; i < 7; ++i)
    {
        geometry_control_v->addItem(QString::number(i));
        geometry_contro_h->addItem(QString::number(i));
    }

    {
        geometry_precision_v->addItem(QString::number(2));
        geometry_precision_h->addItem(QString::number(2));
        geometry_precision_v->addItem(QString::number(4));
        geometry_precision_h->addItem(QString::number(4));
        geometry_precision_v->addItem(QString::number(8));
        geometry_precision_h->addItem(QString::number(8));
        geometry_precision_v->addItem(QString::number(16));
        geometry_precision_h->addItem(QString::number(16));
        geometry_precision_v->addItem(QString::number(32));
        geometry_precision_h->addItem(QString::number(32));
        geometry_precision_v->addItem(QString::number(64));
        geometry_precision_h->addItem(QString::number(64));
        geometry_precision_v->addItem(QString::number(128));
        geometry_precision_h->addItem(QString::number(128));
    }
}

void computer_gui::changeEvent(QEvent *e)
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

void computer_gui::compute_layout(int val)
{
    device_computer_layout_v->setCurrentText(QString::number(idHigh(val)));
    device_computer_layout_h->setCurrentText(QString::number(idLow(val)));
}
void computer_gui::compute_index(int val)
{
    device_computer_index_v->setCurrentText(QString::number(idHigh(val)));
    device_computer_index_h->setCurrentText(QString::number(idLow(val)));
}
void computer_gui::geometry_control(int val)
{
    geometry_control_v->setCurrentText(QString::number(idHigh(val)));
    geometry_contro_h->setCurrentText(QString::number(idLow(val)));
}
void computer_gui::geometry_precision(int val)
{
    geometry_precision_v->setCurrentText(QString::number(idHigh(val)));
    geometry_precision_h->setCurrentText(QString::number(idLow(val)));
}
int computer_gui::compute_layout()const
{
    return idTools(device_computer_layout_v->currentText().toInt(),
                   device_computer_layout_h->currentText().toInt());
}
int computer_gui::compute_index()const
{
    return idTools(device_computer_index_v->currentText().toInt()-1,
                   device_computer_index_h->currentText().toInt()-1);
}
int computer_gui::geometry_control()const
{
    return idTools(geometry_control_v->currentText().toInt(),
                   geometry_contro_h->currentText().toInt());
}
int computer_gui::geometry_precision()const
{
    return idTools(geometry_precision_v->currentText().toInt(),
                   geometry_precision_h->currentText().toInt());
}
void computer_gui::on_device_computer_layout_v_activated(int index)
{

}

void computer_gui::on_device_computer_layout_h_activated(int index)
{

}

void computer_gui::on_device_computer_index_v_activated(int index)
{

}

void computer_gui::on_device_computer_index_h_activated(int index)
{

}

void computer_gui::on_geometry_control_v_activated(int index)
{

}

void computer_gui::on_geometry_contro_h_activated(int index)
{

}

void computer_gui::on_geometry_precision_v_activated(int index)
{

}

void computer_gui::on_geometry_precision_h_activated(int index)
{

}
