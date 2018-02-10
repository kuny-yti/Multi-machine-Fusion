#include "color_gui.h"

color_gui::color_gui(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void color_gui::changeEvent(QEvent *e)
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

void color_gui::command(const int& cmd)
{
    switch (cmd)
    {
    case Command_StartOpenGL:
    for (int i=0; i < color_config.size(); i++)
        color_config[i].clear();
    color_config.clear();
    color_device_id->clear();

    color_config.resize(idHigh(raw_config().device_layout));
    for (int i=0; i < color_config.size(); i++)
    {
        color_config[i].resize(idLow(raw_config().device_layout));
        for (int j = 0; j < color_config[i].size(); j++)
        {
            ColorConfig co;
            co.device_id = idTools(i, j);
            co.brightness = 0.0;
            co.contrast = 0.0;
            co.hue = 0.0;
            co.saturation = 0.0;
            co.red = 1.0;
            co.green = 1.0;;
            co.blue = 1.0;;
            co.alpha = 1.0;;
            color_config[i][j] = co;

            color_device_id->addItem(QString::number(co.device_id, 16));
        }
    }
        break;
    default:
        break;
    }

}

void color_gui::device_id(int val)
{
    color_device_id->setCurrentText(QString::number(val, 16));
}
int color_gui::device_id()const
{
    if (!color_device_id->currentText().isEmpty())
        return color_device_id->currentText().toInt(0, 16);
    else
        return -1;
}
void color_gui::on_color_device_id_activated(int )
{
    if (color_device_id->currentText().isEmpty())
        return;

    int id = color_device_id->currentText().toUtf8().toInt(0, 16);
    log_information("select color device id = "+color_device_id->currentText());

    if (idHigh(id) >= color_config.size())
        return;
    if (idLow(id) >= color_config[idHigh(id)].size())
        return;

    ColorConfig co = color_config[idHigh(id)][idLow(id)];

    color_brightness_spin->setValue(co.brightness);
    color_contrast_spin->setValue(co.contrast);
    color_hue_spin->setValue(co.hue);
    color_saturation_spin->setValue(co.saturation);

    color_red_spin->setValue(co.red);
    color_green_spin->setValue(co.green);
    color_blue_spin->setValue(co.blue);
    color_alpha_spin->setValue(co.alpha);

    color_brightness_slider->setValue(co.brightness*1000);
    color_contrast_slider->setValue(co.contrast*1000);
    color_hue_slider->setValue(co.hue*1000);
    color_saturation_slider->setValue(co.saturation*1000);

    color_red_slider->setValue(co.red*1000);
    color_green_slider->setValue(co.green*1000);
    color_blue_slider->setValue(co.blue*1000);
    color_alpha_slider->setValue(co.alpha*1000);
}

void color_gui::on_color_brightness_spin_valueChanged(double arg1)
{
    color_brightness_slider->setValue (arg1 * 1000.);
    current_color().brightness = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_brightness_reset_clicked()
{
    color_brightness_slider->setValue(0);
    color_brightness_spin->setValue(0.0);
    current_color().brightness = 0.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_brightness_slider_sliderMoved(int arg1)
{
    color_brightness_spin->setValue ((float)arg1 / 1000.);
    current_color().brightness = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_contrast_slider_sliderMoved(int arg1)
{

    color_contrast_spin->setValue ((float)arg1 / 1000.);
    current_color().contrast = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_contrast_spin_valueChanged(double arg1)
{
    color_contrast_slider->setValue (arg1 * 1000.);
    current_color().contrast = (arg1);
    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_contrast_reset_clicked()
{
    color_contrast_slider->setValue(0);
    color_contrast_spin->setValue(0.0);
    current_color().contrast = 0.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_hue_slider_sliderMoved(int arg1)
{
    color_hue_spin->setValue ((float)arg1 / 1000.);
    current_color().hue = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_hue_spin_valueChanged(double arg1)
{
    color_hue_slider->setValue (arg1 * 1000.);
    current_color().hue = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_hue_reset_clicked()
{
    color_hue_slider->setValue(0);
    color_hue_spin->setValue(0.0);
    current_color().hue = 0.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_saturation_slider_sliderMoved(int arg1)
{
    color_saturation_spin->setValue ((float)arg1 / 1000.);
    current_color().saturation = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_saturation_spin_valueChanged(double arg1)
{
    color_saturation_slider->setValue (arg1 * 1000.);
    current_color().saturation = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_saturation_reset_clicked()
{
    color_saturation_slider->setValue(0);
    color_saturation_spin->setValue(0.0);
    current_color().saturation = 0.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_red_slider_sliderMoved(int arg1)
{
    color_red_spin->setValue ((float)arg1 / 1000.);
    current_color().red = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_red_spin_valueChanged(double arg1)
{
    color_red_slider->setValue (arg1 * 1000.);
    current_color().red = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_red_reset_clicked()
{
    color_red_slider->setValue(1000);
    color_red_spin->setValue(1.0);
    current_color().red = 1.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_green_slider_sliderMoved(int arg1)
{
    color_green_spin->setValue ((float)arg1 / 1000.);
    current_color().green = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_green_spin_valueChanged(double arg1)
{
    color_green_slider->setValue (arg1 * 1000.);
    current_color().green = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_green_reset_clicked()
{
    color_green_slider->setValue(1000);
    color_green_spin->setValue(1.0);
    current_color().green = 1.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_blue_slider_sliderMoved(int arg1)
{
    color_blue_spin->setValue ((float)arg1 / 1000.);
    current_color().blue = ((float)arg1 / 1000.);
    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_blue_spin_valueChanged(double arg1)
{
    color_blue_slider->setValue (arg1 * 1000.);
    current_color().blue = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_blue_teset_clicked()
{
    color_blue_slider->setValue(1000);
    color_blue_spin->setValue(1.0);
    current_color().blue = 1.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

void color_gui::on_color_alpha_slider_sliderMoved(int arg1)
{
    color_alpha_spin->setValue ((float)arg1 / 1000.);
    current_color().alpha = ((float)arg1 / 1000.);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_alpha_spin_valueChanged(double arg1)
{
    color_alpha_slider->setValue (arg1 * 1000.);
    current_color().alpha = (arg1);

    raw_config().color_config = current_color();
    control(Command_Color);
}
void color_gui::on_color_alha_reset_clicked()
{
    color_alpha_slider->setValue(1000);
    color_alpha_spin->setValue(1.0);
    current_color().alpha = 1.0;
    raw_config().color_config = current_color();
    control(Command_Color);
}

ColorConfig &color_gui::current_color(const QString &arg1)
{
    ColorConfig pri;
    if (arg1.isEmpty())
    {
        if (!color_device_id->currentText().isEmpty())
        {
            int idvalue = color_device_id->currentText().toUtf8().toInt(0, 16);
            if ((idvalue < 0xFFFF) && (idvalue >= 0))
                return color_config[idHigh(idvalue)][idLow(idvalue)];
        }
    }
    else
    {
        int idvalue = arg1.toUInt(0, 16);
        if ((idvalue < 0xFFFF) && (idvalue >= 0))
            return color_config[idHigh(idvalue)][idLow(idvalue)];
    }
    return pri;
}
