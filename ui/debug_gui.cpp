#include "debug_gui.h"
#include "ui_define.h"
#include "gui_config.h"

#include <QDesktopWidget>
#include <QWheelEvent>
#include <QDebug>

static const QRect desktop_geometry()
{
    uint screenNumber = 0;
    uint screenHeightMax = 0;
    uint screenHeightMaxBak = 0;

    QRect rect;
    QDesktopWidget *desktopWidget = QApplication::desktop();
    if (desktopWidget->screenCount())
    {
       screenNumber =  desktopWidget->screenCount();
    }
    else
    {
        return QRect();
    }

    for (uint i = 0; i < screenNumber; i++)
    {
        screenHeightMax = desktopWidget->screenGeometry(i).height();
        if (screenHeightMax > screenHeightMaxBak)
        {
            screenHeightMaxBak = screenHeightMax;
            rect.setHeight(screenHeightMax);
        }
        rect.setWidth(desktopWidget->screenGeometry(i).width() + rect.width());
    }
    return rect;
}

//! ret 0 不变，1左边不够，2右边不够
static int is_dock(const QRect &rect)
{
    QRect dg = desktop_geometry();

    if ((rect.x() + rect.width()) > dg.width())
    {
        return 2;
    }
    else if (rect.x() < dg.x())
    {
        return 1;
    }

    return 0;
}

debug_gui::debug_gui(gui_config *cf, QWidget *parent) :
    QWidget(parent),
    dock_direction(1),
    guicf(cf)
{
    setupUi(this);

    setWindowOpacity(raw_config().widget_alpha);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags (Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);

    aadjust = 0;
}
debug_gui::~debug_gui()
{
    if (aadjust)
        delete aadjust;
}

void debug_gui::changeEvent(QEvent *e)
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
bool debug_gui::event(QEvent *evt)
{
    dock_handle(is_dock(this->geometry()));

    if (!isEnabled())
    {
        switch(evt->type())
        {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        case QEvent::TouchCancel:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
#ifndef QT_NO_WHEELEVENT
        case QEvent::Wheel:
#endif
            return false;
        default:
            break;
        }
    }
    switch (evt->type())
    {
    case QEvent::Wheel:
    {
        QWheelEvent *wh = static_cast<QWheelEvent *>(evt);
        if (wh->delta() > 0)
        {
            if (raw_config().widget_alpha < 0.3)
            {
                break ;
            }
            raw_config().widget_alpha -= 0.05;
        }
        else
        {
            if (raw_config().widget_alpha <= 1.0)
                raw_config().widget_alpha += 0.05;
        }

        control(Command_AlphaAttach);
        this->update();
    }
        break;
    default:
        break;
    }

    return QWidget::event(evt);
}
void debug_gui::dock_handle(int dir)
{
    if (dir != 0)
    {
        dock_direction = dir == 1 ? Attach_Right:
                                    dir == 2 ? Attach_Left:0;
        wParameBegin();
        wParame << dock_direction;
        wParameEnd();

        control(Command_DockAttach);

        if (dir == Attach_Left)
        {
        }
        else if (dir == Attach_Right)
        {

        }
    }

}
void debug_gui::command(const int& cmd)
{
    switch (cmd) {
    case Command_AlphaAttach:
        setWindowOpacity(raw_config().widget_alpha);
        break;
    case Command_MoveAttach:
    {
        QRect geom;
        rParameBegin();
        rParame >> geom;
        rParameEnd();

        if (dock_direction == Attach_Left)
        {
            setGeometry(geom.x()-(float)geom.height()*0.9,
                        (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }
        else if (dock_direction == Attach_Right)
        {
            setGeometry(geom.x()+geom.width(), (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }


    }
        break;
    case Command_AttachDebug:
    {
        QRect geom;
        rParameBegin();
        rParame >> geom;
        rParameEnd();

        dock_handle(is_dock(this->geometry()));

        if (dock_direction == Attach_Left)
        {
            setGeometry(geom.x()-(float)geom.height()*0.9,
                        (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }
        else if (dock_direction == Attach_Right)
        {
            setGeometry(geom.x()+geom.width(), (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }

        if (this->isHidden())
            this->show();
        else
            this->hide();
    }
        break;
    case Command_AttachDebugOn:
    {
        QRect geom;
        rParameBegin();
        rParame >> geom;
        rParameEnd();

        dock_handle(is_dock(this->geometry()));

        if (dock_direction == Attach_Left)
        {
            setGeometry(geom.x()-(float)geom.height()*0.9,
                        (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }
        else if (dock_direction == Attach_Right)
        {
            setGeometry(geom.x()+geom.width(), (float)geom.y()+geom.height()*0.1,
                        (float)geom.height()*0.9,(float)geom.height()*0.8);
        }
        this->show();
    }
        break;
    case Command_AttachDebugOff:
        this->hide();
        break;
    case Command_Administrator:
    {
        int admin;
        rParameBegin();
        rParame >> admin;
        rParameEnd();

        if (admin == Mode_Junior)
        {

        }
        else if (admin == Mode_Senior)
        {

        }
        else if (admin == Mode_Maintenance)
        {

        }
    }
        break;
    case Command_ShowTools:
        //this->show();
        break;
    case Command_HideTools:
        this->hide();
        break;
    case Command_StartDebug:
    {
        for (int i = 0; i < blend_config.size(); i++)
        {
            blend_config[i].clear();
        }
        blend_config.clear();
        blend_device_id->clear();

        blend_config.resize(idHigh(raw_config().device_layout));
        for (int i = 0; i < blend_config.size(); i++)
        {
            blend_config[i].resize(idLow(raw_config().device_layout));
            for (int j = 0; j < blend_config[i].size(); j++)
            {
                blend_config[i][j].device_id = idTools(i, j);

                blend_device_id->addItem(QString::number(
                                             blend_config[i][j].device_id, 16));

                //blend_config[i][j].left.area = QRectF() ;
                blend_config[i][j].left.brightness = 0.5;
                blend_config[i][j].left.gamma = 3.0;
                blend_config[i][j].left.exponent = 0.9;

                //blend_config[i][j].right.area = QRectF() ;
                blend_config[i][j].right.brightness = 0.5;
                blend_config[i][j].right.gamma = 3.0;
                blend_config[i][j].right.exponent = 0.9;

                //blend_config[i][j].top.area = QRectF() ;
                blend_config[i][j].top.brightness = 0.5;
                blend_config[i][j].top.gamma = 3.0;
                blend_config[i][j].top.exponent = 0.9;

                //blend_config[i][j].bottom.area = QRectF() ;
                blend_config[i][j].bottom.brightness = 0.5;
                blend_config[i][j].bottom.gamma = 3.0;
                blend_config[i][j].bottom.exponent = 0.9;

            }
        }
        on_blend_device_id_activated(0);
    }
        break;
    default:
        break;
    }
}

BlendConfig &debug_gui::current_data(const QString &arg1)
{
    BlendConfig pri;
    if (arg1.isEmpty())
    {
        if (blend_device_id->currentText().isEmpty())
            return pri;

        int ids = blend_device_id->currentText().toUtf8().toInt(0, 16);
        return blend_config[idHigh(ids)][idLow(ids)];
    }
    int ids = arg1.toUtf8().toInt(0, 16);
    return blend_config[idHigh(ids)][idLow(ids)];
}
void debug_gui::on_blend_device_id_activated(int )
{
    if (blend_device_id->currentText().isEmpty())
        return;

    log_information("select blend device id = "+blend_device_id->currentText());
    BlendConfig fu = current_data(blend_device_id->currentText());
    blend_left_lume_spin->setValue(fu.left.brightness);
    blend_left_lume_slider->setValue(fu.left.brightness*1000);
    blend_left_gamma_spin->setValue(fu.left.gamma);
    blend_left_gamma_slider->setValue(fu.left.gamma*1000);
    blend_left_exponent_spin->setValue(fu.left.exponent);
    blend_left_exponent_slider->setValue(fu.left.exponent*1000);

    blend_right_lume_spin->setValue(fu.right.brightness);
    blend_right_lume_slider->setValue(fu.right.brightness*1000);
    blend_right_gamma_spin->setValue(fu.right.gamma);
    blend_right_gamma_slider->setValue(fu.right.gamma*1000);
    blend_right_exponent_spin->setValue(fu.right.exponent);
    blend_right_exponent_slider->setValue(fu.right.exponent*1000);

    blend_top_lume_spin->setValue(fu.top.brightness);
    blend_top_lume_slider->setValue(fu.top.brightness*1000);
    blend_top_gamma_spin->setValue(fu.top.gamma);
    blend_top_gamma_slider->setValue(fu.top.gamma*1000);
    blend_top_exponent_spin->setValue(fu.top.exponent);
    blend_top_exponent_slider->setValue(fu.top.exponent*1000);

    blend_bottom_lume_spin->setValue(fu.bottom.brightness);
    blend_bottom_lume_slide->setValue(fu.bottom.brightness*1000);
    blend_bottom_gamma_spin->setValue(fu.bottom.gamma);
    blend_bottom_gamma_slider->setValue(fu.bottom.gamma*1000);
    blend_bottom_exponent_spin->setValue(fu.bottom.exponent);
    blend_bottom_exponent_slider->setValue(fu.bottom.exponent*1000);
}

#define check_gui()
#define float_int() (arg1 * 1000)
#define int_float() ((float)value / 1000.)

void debug_gui::on_blend_left_lume_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().left.brightness = arg1;
    blend_left_lume_slider->setValue(float_int());
}

void debug_gui::on_blend_left_lume_slider_sliderMoved(int value)
{
    check_gui();
    current_data().left.brightness = int_float();
    blend_left_lume_spin->setValue(int_float());

    raw_config().blend_config = current_data();
    control(Command_BlendLeft);
}

void debug_gui::on_blend_left_exponent_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().left.exponent = arg1;
    blend_left_exponent_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendLeft);
}

void debug_gui::on_blend_left_exponent_slider_sliderMoved(int value)
{
    check_gui();
    current_data().left.exponent = int_float();
    blend_left_exponent_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendLeft);
}

void debug_gui::on_blend_left_gamma_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().left.gamma = arg1;
    blend_left_gamma_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendLeft);
}

void debug_gui::on_blend_left_gamma_slider_sliderMoved(int value)
{
    check_gui();
    current_data().left.gamma = int_float();
    blend_left_gamma_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendLeft);
}

void debug_gui::on_blend_right_lume_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().right.brightness = arg1;
    blend_right_lume_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_right_lume_slider_sliderMoved(int value)
{
    check_gui();
    current_data().right.brightness = int_float();
    blend_right_lume_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_right_exponent_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().right.exponent = arg1;
    blend_right_exponent_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_right_exponent_slider_sliderMoved(int value)
{
    check_gui();
    current_data().right.exponent = int_float();
    blend_right_exponent_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_right_gamma_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().right.gamma = arg1;
    blend_right_gamma_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_right_gamma_slider_sliderMoved(int value)
{
    check_gui();
    current_data().right.gamma = int_float();
    blend_right_gamma_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendRight);
}

void debug_gui::on_blend_top_lume_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().top.brightness = arg1;
    blend_top_lume_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_top_lume_slider_sliderMoved(int value)
{
    check_gui();
    current_data().top.brightness = int_float();
    blend_top_lume_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_top_exponent_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().top.exponent = arg1;
    blend_top_exponent_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_top_exponent_slider_sliderMoved(int value)
{
    check_gui();
    current_data().top.exponent = int_float();
    blend_top_exponent_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_top_gamma_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().top.gamma = arg1;
    blend_top_gamma_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_top_gamma_slider_sliderMoved(int value)
{
    check_gui();
    current_data().top.gamma = int_float();
    blend_top_exponent_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendTop);
}

void debug_gui::on_blend_bottom_lume_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().bottom.brightness = arg1;
    blend_bottom_lume_slide->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}

void debug_gui::on_blend_bottom_lume_slide_sliderMoved(int value)
{
    check_gui();
    current_data().top.brightness = int_float();
    blend_bottom_lume_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}

void debug_gui::on_blend_bottom_exponent_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().bottom.exponent = arg1;
    blend_bottom_exponent_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}

void debug_gui::on_blend_bottom_exponent_slider_sliderMoved(int value)
{
    check_gui();
    current_data().top.exponent = int_float();
    blend_bottom_exponent_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}

void debug_gui::on_blend_bottom_gamma_spin_valueChanged(double arg1)
{
    check_gui();
    current_data().bottom.gamma = arg1;
    blend_bottom_gamma_slider->setValue(float_int());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}

void debug_gui::on_blend_bottom_gamma_slider_sliderMoved(int value)
{
    check_gui();
    current_data().top.gamma = int_float();
    blend_bottom_gamma_spin->setValue(int_float());
    raw_config().blend_config = current_data();
    control(Command_BlendBottom);
}
