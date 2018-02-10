#include "tools_gui.h"
#include "ui_define.h"
#include "system_config.h"
#include "color_gui.h"
#include "computer_gui.h"
#include "gui_config.h"

#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QTranslator>
#include <QFileDialog>
#include <QFileInfo>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

const QString image_stl =
        QString("Image(*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)");
/*
const QString video_stl =
        QString("Media(*.ASF *.AVI *.FLV *.MOV *.MP4 *.3GP *.RMVB *.WMV *.MKV *.MPG *.VOB *.SWF)");
*/
const QString video_stl =
        QString("Media(*.ytm *.avi)");

const QString win8_start =
        QString("C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Startup/");
const QString win7_start =
        QString("C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Startup/");

static int search_video(const QString &path, QStringList &sfn, QStringList &sfp)
{
    QDir dir(path);
    if (!dir.exists())
        return 0;

    dir.setFilter(/*QDir::Dirs | */QDir::Files);//除了目录或文件，其他的过滤掉
    //dir.setSorting(QDir::DirsFirst);//优先显示目录

    foreach (QFileInfo f , dir.entryInfoList())
        if(f.fileName() != "." || f.fileName() != "..")
        {
            sfn.append(f.fileName());
            sfp.append(f.filePath());
        }
    return sfn.count();
}

const static QString DeviceStrategyStrq[Strategy_Count] =
{
    QObject::tr("Auto"),
    QObject::tr("Mouse Sigle"),
    QObject::tr("Mouse Mulit")
};
const static QString LanguageStringq[Language_Count] =
{
    QObject::tr("Local"),
    QObject::tr("English"),
    QObject::tr("Chinese")
};
const static QString LocalImageStrq[Image_Count] =
{
    QObject::tr("'&' Symbol"),
    QObject::tr("Mesh"),
    QObject::tr("Letter"),
    QObject::tr("LogoColor"),
    QObject::tr("LogoWriting")
};
const static QString LocalImagePathStrq[Image_Count] =
{
    QObject::tr(":/image/symbol.png"),
    QObject::tr(":/image/mesh.png"),
    QObject::tr(":/image/letter.png"),
    QObject::tr(":/image/yantucolor.png"),
    QObject::tr(":/image/yantuwriting.png")
};

class tools_gui_p
{
public:
    tools_gui_p():
        back_point(0,0),
        enable_desktop(false),
        select_video(NULL)
    {
        translator = new QTranslator();
        qApp->installTranslator(translator);
        device_computer = new computer_gui();
        device_color = new color_gui();
    }
    ~tools_gui_p()
    {
        if (select_video)
            delete select_video;
        if (device_color)
            delete device_color;
        if (device_computer)
            delete device_computer;
        if (translator)
            delete translator;
        translator = 0;
        device_computer = 0;
        device_color = 0;
        select_video = 0;
    }

public:
    QPoint back_point; //! 移动窗口用
    QTranslator *translator;
    bool enable_desktop;//! 桌面启动状态

    gui_config *gui_cf;

    computer_gui * device_computer;//0
    color_gui * device_color;//5
    QTimer *select_video;

};

tools_gui::tools_gui(gui_config *cf, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    d = new tools_gui_p();
    d->gui_cf = cf;

    setWindowOpacity(raw_config().widget_alpha);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags (Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);

    rendering_desktop->hide();
    attach_debug_left->hide();
    rendering_decoder_mode->hide();
    multimedia_desktop->hide();

    system_language->addItem(LanguageStringq[Language_Local], Language_Local);
    system_language->addItem(LanguageStringq[Language_English], Language_English);
    system_language->addItem(LanguageStringq[Language_Chinese], Language_Chinese);
    for (int i = 0; i < Resolution_Count; ++i)
    {
        device_resolution->addItem(DeviceResolutionStr[i]);
    }
    //device_resolution->setCurrentIndex(2);
    for (int i = 1;  i <= 12; ++i)
    {
        device_projection_layout_v->addItem(QString::number(i));
        device_projection_layout_h->addItem(QString::number(i));
    }
    for (int i = 0; i < Image_Count; ++i)
    {
        multimedia_image_local_select->addItem(LocalImageStrq[i] );
    }
    device_strategy->addItem(DeviceStrategyStrq[Strategy_Auto]);
    device_strategy->addItem(DeviceStrategyStrq[Strategy_MouseSigle]);
    handle_gui();
    QTimer::singleShot(1000*2, this, SLOT(handle_start()));
}
tools_gui::~tools_gui()
{
    if (d)
        delete d;
    d = NULL;
}

void tools_gui::changeEvent(QEvent *e)
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

bool tools_gui::event(QEvent *evt)
{
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
    case QEvent::MouseMove:
    {
        QMouseEvent *mEvt = static_cast<QMouseEvent *>(evt);
        if (!d->back_point.isNull())
        {
            move(mEvt->globalPos() - d->back_point);
            evt->accept();

            wParameBegin();
            wParame << this->geometry();
            wParameEnd()

            control(Command_MoveAttach);
        }
    }
        break;
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mEvt = static_cast<QMouseEvent *>(evt);
        if (mEvt->button() == Qt::RightButton)
        {
            d->back_point = mEvt->globalPos() - frameGeometry().topLeft();
            evt->accept();
        }
    }
        break;
    case QEvent::MouseButtonRelease:
    {
        d->back_point = QPoint();
    }
        break;
#ifndef QT_NO_WHEELEVENT
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
#endif

    default:
        break;
    }

    return QWidget::event( evt);
}
#include <QDebug>
void tools_gui::command(const int& cmd)
{
    switch (cmd) {
    case Command_DockAttach:
    {
        int dir;
        rParameBegin();
        rParame >> dir;
        rParameEnd();

        if (dir == Attach_Right)
        {
            attach_debug_left->hide();
            attach_debug->show();
        }
        else if (dir == Attach_Left)
        {
            attach_debug->hide();
            attach_debug_left->show();
        }
    }
        break;
    case Command_Administrator:
    {
        int admin;
        rParameBegin();
        rParame >> admin;
        rParameEnd();

        if (admin == Mode_Junior)
        {
            rendering_desktop->hide();
            rendering_decoder_mode->hide();
            multimedia_desktop->hide();
            d->device_computer->hide();
            d->device_color->hide();
            tools_box->removeItem(5);
            tools_box->removeItem(0);
        }
        else if (admin == Mode_Senior)
        {
            rendering_desktop->show();
            rendering_decoder_mode->show();
            multimedia_desktop->show();
            tools_box->insertItem(0, d->device_computer, tr("Computer"));
            tools_box->insertItem(5, d->device_color, tr("Color"));
            d->device_computer->show();
            d->device_color->show();
            tools_box->layout()->setSpacing(0);
        }
        else if (admin == Mode_Maintenance)
        {

        }
    }
        break;
    case Command_ExitSystem:
        break;
    case Error_License:
    {
        QMessageBox::warning (this, "Software authorization error", "Invalid or encrypted dog. file!");
    }
        break;
    case Command_ShowTools:
        if (isHidden())
            this->show();
        else
        {
            this->hide();
            this->show();
        }
        log_information("show tools.");
        break;
    case Command_HideTools:
        this->hide();
        log_information("hide tools.");
        break;
    case Error_StartDesktop:
        d->enable_desktop = false;
        log_error("enable desktop error.");
        break;
    case Command_Progress:
    {
        ulong dura_s = raw_config().media_duration / (1000*1000);
        ulong dura_mi = dura_s / 60;
        ulong dura_h = dura_mi / 60;
        control_duration->setText(tr("Duration:")+
                                  QString("%1:%2:%3:%4").arg(dura_h%24).
                                  arg(dura_mi%60).arg(dura_s %60).
                                  arg(raw_config().media_duration %1000));

        dura_s = raw_config().media_progress / (1000*1000);
        dura_mi = dura_s / 60;
        dura_h = dura_mi / 60;
        control_progress->setText(tr("Progress:")+
                                  QString("%1:%2:%3:%4").arg(dura_h%24).
                                  arg(dura_mi%60).arg(dura_s %60).
                                  arg(raw_config().media_progress %1000));
    }
        break;
    case ~Command_PlayStop:
    {
        if (raw_config().media_state == 0)//停止
        {
            control_play_stop->setText (tr("Play"));
            control_pause_continue->setText(tr("Pause"));
        }
        else if (raw_config().media_state & 1)//打开播放或重新播放、继续播放
        {
            control_play_stop->setText (tr("Stop"));
            control_pause_continue->setText(tr("Pause"));
        }
        else if (raw_config().media_state & 2)//暂停
        {
            control_pause_continue->setText(tr("Continue"));
        }
    }
        break;
    case Command_AlphaAttach:
        setWindowOpacity(raw_config().widget_alpha);
        break;
    case Command_CmdSelectMedia :
        if (raw_config().media_video_index > 1 && raw_config().media_video_index < multimedia_media_select->count())
        {
            QString file = multimedia_media_select->itemData(raw_config().media_video_index).toString();
            if (!file.isEmpty())
            {
                memset(raw_config().media_video_select, 0, 128);
                memcpy(raw_config().media_video_select, file.toUtf8().data(),
                       file.toUtf8().size());
                log_information("select media = "+file);
                control(Command_SelectMedia);
            }
        }
        break;
    default:
        break;
    }
}

#include <QMessageBox>
#include <QThread>
void tools_gui::handle_start()
{
    if (raw_config().system_setup & 1)
    {
        on_system_data_load_clicked();
        if (raw_config().display_mode & Rendering_Desktop)
        {
            on_multimedia_start_desktop_clicked();
        }
        else if (raw_config().display_mode & Rendering_Media)
        {
            QThread::sleep (2);
            on_control_play_stop_clicked();
        }

        if (raw_config().system_setup & 1<<1)
            control(Command_HideTools);
    }
}
void tools_gui::handle_exit()
{
    //! 要处理是否数据没有保存
    if ((raw_config().data_update < Exit_ConfigNotSave) ||
            raw_config().data_update > Exit_OtherNotSave)//!没有需要保存的
    {

    }
    else if (raw_config().data_update & 0x2)//!几何信息未保存
    {
        switch(QMessageBox::information( this, tr("Current configuration is not saved"),
                                         tr("The current configuration is not saved, whether you need to save !"),
                                         tr("Yes"), tr("No"),
                                         0, 1 ) )
            {
            case 0:
                control(Command_ConfigSave);
                break;
            case 1:
            default:
                break;
            }
    }
    else if (raw_config().data_update & 0x2)//!几何信息未保存
    {

    }

}
void tools_gui::on_system_append_start_stateChanged(int arg1)
{
    //开机自启动
    if (arg1)
    {
        //选中
        raw_config().system_setup |= 1;
        QString curren_run_file = QApplication::applicationFilePath();


        QFile::link (curren_run_file,
                     win8_start+QString("Fusion%1.lnk").arg(FusionVersionStr()));
    }
    else
    {
        raw_config().system_setup &= ~1;

        QFile::remove (win8_start+QString("Fusion%1.lnk").arg(FusionVersionStr()));
    }
    log_information("append windows start = "+QString::number(arg1));
}

void tools_gui::on_system_start_minimize_stateChanged(int arg1)
{
    //启动后最小化工具栏
    if (arg1)
        raw_config().system_setup |= 1<<1;
    else
       raw_config().system_setup &=  ~(1<<1);
    log_information("start minimize = "+QString::number(arg1));
}

void tools_gui::on_system_warp_cursor_stateChanged(int arg1)
{
    //启动后隐藏鼠标
    if (arg1)
        raw_config().system_setup |= 1 << 2;
    else
       raw_config().system_setup &=  ~(1 << 2);
    log_information("warp cursor = "+QString::number(arg1));
}
void tools_gui::on_system_language_activated(int index)
{
    switch (system_language->currentData().toInt())
    {
    case Language_English:
        d->translator->load (raw_config().dir_config_language + QString("language_en.qm"));
        qApp->installTranslator (d->translator);

        raw_config().system_language_index = Language_English;
        memset(&(raw_config().system_language), 0, 32);
        memcpy(&(raw_config().system_language), "language_en", strlen("language_en"));
        break;
    case Language_Chinese:
        d->translator->load (raw_config().dir_config_language +
                             QString("language_zh_CN.qm"));
        qApp->installTranslator (d->translator);

        raw_config().system_language_index = Language_Chinese;
        memset(&(raw_config().system_language), 0,  32);
        memcpy(&(raw_config().system_language), "language_zh_CN", strlen("language_zh_CN"));

        break;

    case Language_Local:
    default:
        d->translator->load (raw_config().dir_config_language +
                             QString("language_")+QLocale::system().name()+".qm");
        qApp->installTranslator (d->translator);

        raw_config().system_language_index = Language_Local;
        memset(&(raw_config().system_language), 0,  32);
        memcpy(&(raw_config().system_language),
               QString("language_"+QLocale::system().name()).toUtf8().data(),
               QString("language_"+QLocale::system().name()).toUtf8().size());

        break;
    }
    log_information("system language = "+QString(LanguageString[index]));
}

void tools_gui::gui_handle()
{
    switch (system_language->currentData().toInt())
    {
    case Language_English:
        raw_config().system_language_index = Language_English;
        memset(&(raw_config().system_language), 0, 32);
        memcpy(&(raw_config().system_language), "language_en", strlen("language_en"));
        break;
    case Language_Chinese:
        raw_config().system_language_index = Language_Chinese;
        memset(&(raw_config().system_language), 0,  32);
        memcpy(&(raw_config().system_language), "language_zh_CN", strlen("language_zh_CN"));

        break;

    case Language_Local:
    default:
        raw_config().system_language_index = Language_Local;
        memset(&(raw_config().system_language), 0,  32);
        memcpy(&(raw_config().system_language),
               QString("language_"+QLocale::system().name()).toUtf8().data(),
               QString("language_"+QLocale::system().name()).toUtf8().size());

        break;
    }
    raw_config().device_resolution = (DeviceResolutionMode)device_resolution->currentIndex();

    if (system_warp_cursor->isChecked())
        raw_config().system_setup |= (1<<2);
    if (system_start_minimize->isChecked())
        raw_config().system_setup |= (1<<1);
    if (system_append_start->isChecked())
        raw_config().system_setup |= (1);

    if (decode_mode_cpu->isChecked())
        raw_config().decoder_mode = Decoder_CPU;
    else if (decoder_mode_gpu->isChecked())
        raw_config().decoder_mode = Decoder_GPU;
    else if (decoder_mode_opencl->isChecked())
        raw_config().decoder_mode = Decoder_OpenCL;

    raw_config().computer_layout = d->device_computer->compute_layout();
    raw_config().computer_index =d->device_computer->compute_index();

    raw_config().device_layout =idTools(device_projection_layout_v->currentText().toInt(),
                                        device_projection_layout_h->currentText().toInt());
    raw_config().device_resolution = (DeviceResolutionMode)device_resolution->currentIndex();
    raw_config().device_strategy = (DeviceStrategy)device_strategy->currentIndex();

    raw_config().geometry_control = d->device_computer->geometry_control();
    raw_config().geometry_precision = d->device_computer->geometry_precision();
    if (geometry_mode_ffd->isChecked())
        raw_config().adjust_mode = MeshMode_FFD;
    else if (geometry_mode_mesh->isChecked())
        raw_config().adjust_mode = MeshMode_MESH;

    QString text = step_load->currentText();
    memset(raw_config().data_step_save, 0, 64);
    if (!text.isEmpty())
        raw_config().data_step_index = step_load->currentIndex();
    text = step_save_name->text();
    if (!text.isEmpty())
        memcpy(raw_config().data_step_save, text.toUtf8().data(), text.toUtf8().size());


    if (this->rendering_media->isChecked())
    {
        raw_config().display_mode = Rendering_Media;
    }
    else if (rendering_image->isChecked())
    {
        raw_config().display_mode = Rendering_Image;
    }
    else if (rendering_grid->isChecked())
    {
        raw_config().display_mode = Rendering_Grid;
    }
    else if (rendering_desktop->isChecked())
    {
        raw_config().display_mode = Rendering_Desktop;
    }

    if (rendering_attach_fusion->isChecked ())
    {
        raw_config().display_mode |= Rendering_Blend;
    }
    if (rendering_attach_grid->isChecked ())
    {
        raw_config().display_mode |= Rendering_Mesh;
    }
    if (rendering_attach_point->isChecked ())
    {
        raw_config().display_mode |= Rendering_Point;
    }

    if (multimedia_image_other_select->isEnabled())
        raw_config().media_image_mode = 2;
    else if (multimedia_image_local_select->isEnabled())
        raw_config().media_image_mode = 1;

    raw_config().media_image_local_index = multimedia_image_local_select->currentIndex();
    raw_config().media_image_other_index = multimedia_image_other_select->currentIndex();
    raw_config().media_sequence_index = multimedia_sequence_select->currentIndex();
    raw_config().media_sequence_speed = multimedia_sequence_speed->value();
    raw_config().media_video_index = multimedia_media_select->currentIndex();
    raw_config().media_volum = (float)control_volum->value() / 100.;
    raw_config().desktop_frame_rate = multimedia_frame_rate->value();

    if (control_default->isChecked())
        raw_config().media_play_mode = Play_Default;
    else if (control_loop_play->isChecked())
        raw_config().media_play_mode = Play_Loop;
    else if (control_command_play->isChecked())
        raw_config().media_play_mode = Play_Command;

    raw_config().color_device_id = d->device_color->device_id();
}
void tools_gui::handle_gui()
{
    if (!QString(raw_config().system_language).isEmpty())
    {
        d->translator->load (raw_config().dir_config_language +
                             QString(raw_config().system_language)+QString(".qm"));
        qApp->installTranslator (d->translator);
        system_language->setCurrentIndex(raw_config().system_language_index);
    }

    device_resolution->setCurrentIndex(raw_config().device_resolution);

    system_warp_cursor->setChecked(raw_config().system_setup & (1<<2) ?2 : 0);
    system_start_minimize->setChecked(raw_config().system_setup & (1<<1) ?2 : 0);
    system_append_start->setChecked(raw_config().system_setup & (1) ?2 : 0);

    if (raw_config().decoder_mode == Decoder_CPU)
        decode_mode_cpu->setChecked(true);
    else if (raw_config().decoder_mode == Decoder_GPU)
        decoder_mode_gpu->setChecked(true);
    else if (raw_config().decoder_mode == Decoder_OpenCL)
        decoder_mode_opencl->setChecked(true);

    d->device_computer->compute_layout(raw_config().computer_layout);
    d->device_computer->compute_index(raw_config().computer_index);
    d->device_computer->geometry_control(raw_config().geometry_control);
    d->device_computer->geometry_precision(raw_config().geometry_precision);

    QString temp = QString::number(idHigh(raw_config().device_layout));
    device_projection_layout_v->setCurrentText (temp);
    temp = QString::number(idLow(raw_config().device_layout));
    device_projection_layout_h->setCurrentText(temp);

    device_resolution->setCurrentIndex((raw_config().device_resolution));
    device_strategy->setCurrentIndex(raw_config().device_strategy);

    if (raw_config().adjust_mode == MeshMode_FFD)
       geometry_mode_ffd->setChecked(true);
    else
       geometry_mode_mesh->setChecked(true);

    handle_rendering_gui(raw_config().display_mode);

    if (raw_config().media_image_mode == 2)
        multimedia_image_other_select->setEnabled(true);
    else
        multimedia_image_local_select->setEnabled(true);

    multimedia_image_local_select->setCurrentIndex(raw_config().media_image_local_index);
    multimedia_image_other_select->setCurrentIndex(raw_config().media_image_other_index);
    multimedia_sequence_select->setCurrentIndex(raw_config().media_sequence_index);
    multimedia_sequence_speed->setValue(raw_config().media_sequence_speed);
    multimedia_media_select->setCurrentIndex(raw_config().media_video_index);
    control_volum->setValue(raw_config().media_volum * 100);

    if (raw_config().media_play_mode == Play_Default)
        control_default->setChecked(true);
    else if (raw_config().media_play_mode == Play_Loop)
        control_loop_play->setChecked(true);
    else if (raw_config().media_play_mode == Play_Command)
        control_command_play->setChecked(true);

    if (raw_config().color_device_id != -1)
        d->device_color->device_id(raw_config().color_device_id);

    if (!QString(raw_config().data_step_save).isEmpty())
        step_save_name->setText(raw_config().data_step_save);
    if (raw_config().data_step_index != -1)
        step_load->setCurrentIndex(raw_config().data_step_index);

    multimedia_frame_rate->setValue(raw_config().desktop_frame_rate);
}
void tools_gui::on_system_junior_clicked()
{
    if (raw_config().admin_mode == Mode_Junior)
        return ;

    wParameBegin();
    wParame << Mode_Junior;
    wParameEnd();
    raw_config().admin_mode = Mode_Junior;
    control(Command_Administrator);
}

void tools_gui::on_system_senior_clicked()
{
    if (raw_config().admin_mode == Mode_Senior)
        return ;
    wParameBegin();
    wParame << Mode_Senior;
    wParameEnd();
    raw_config().admin_mode = Mode_Senior;
    control(Command_Administrator);
}

void tools_gui::on_system_maintenance_clicked()
{
    wParameBegin();
    wParame << Mode_Maintenance;
    wParameEnd();
    raw_config().admin_mode = Mode_Maintenance;
    control(Command_Administrator);
}

void tools_gui::on_device_projection_layout_v_activated(int )
{
    raw_config().device_layout = idTools(
                device_projection_layout_v->currentText().toUtf8().toInt(),
                 device_projection_layout_h->currentText().toUtf8().toInt());
}
void tools_gui::on_device_projection_layout_h_activated(int )
{
    raw_config().device_layout = idTools(
                device_projection_layout_v->currentText().toUtf8().toInt(),
                 device_projection_layout_h->currentText().toUtf8().toInt());
}
void tools_gui::on_device_resolution_activated(int index)
{
    raw_config().device_resolution= (DeviceResolutionMode)index;
}
void tools_gui::on_device_strategy_activated(int index)
{
    raw_config().device_strategy = (DeviceStrategy)index;
}

void tools_gui::on_device_start_clicked()
{
    device_move_pixel->setCurrentIndex(5);
    geometry_mode_ffd->setChecked(true);
    rendering_grid->setChecked(true);
    rendering_attach_grid->setChecked(false);
    rendering_attach_point->setChecked(false);

    raw_config().display_mode = Rendering_Grid;

    raw_config().computer_layout = d->device_computer->compute_layout();
    raw_config().computer_index = d->device_computer->compute_index();

    raw_config().geometry_control = d->device_computer->geometry_control();
    raw_config().geometry_precision = d->device_computer->geometry_precision();

    raw_config().device_layout =
            idTools(device_projection_layout_v->currentText().toInt(),
                    device_projection_layout_h->currentText().toInt());
    raw_config().device_strategy =
            (DeviceStrategy)device_strategy->currentIndex();
    raw_config().device_resolution =
            (DeviceResolutionMode)device_resolution->currentIndex();

    log_information("start adjust data:");
    log_information("    computer layout(v:h) = "+QString("%1:%2").
                    arg(idHigh(d->device_computer->compute_layout())).
                    arg(idLow(d->device_computer->compute_layout())));
    log_information("    computer index(v:h) = "+QString("%1:%2").
                    arg(idHigh(d->device_computer->compute_index())).
                    arg(idLow(d->device_computer->compute_index())));
    log_information("    projection layout(v:h) = "+QString("%1:%2").
                    arg(device_projection_layout_v->currentText().toInt()).
                    arg(device_projection_layout_h->currentText().toInt()));
    log_information("    geometry control(v:h) = "+QString("%1:%2").
                    arg(idHigh(d->device_computer->geometry_control())).
                    arg(idLow(d->device_computer->geometry_control())));
    log_information("    geometry precision(v:h) = "+QString("%1:%2").
                    arg(idHigh(d->device_computer->geometry_precision())).
                    arg(idLow(d->device_computer->geometry_precision())));
    log_information("    geometry strategy = "+
                    QString(DeviceStrategyStr[device_strategy->currentIndex()]));
    log_information("    device resolution = "+
                    QString(DeviceResolutionStr[
                            device_resolution->currentIndex()]));

    control(Command_StartOpenGL);
    if (system_junior->isChecked())
        tools_box->setCurrentIndex(1);
    else
        tools_box->setCurrentIndex(2);
}


void tools_gui::on_geometry_mode_ffd_clicked()
{
    log_information("geometry adjust mode = FFD");
    raw_config().adjust_mode = MeshMode_FFD;
    control(Command_ModeDebug);
}

void tools_gui::on_geometry_mode_mesh_clicked()
{
    log_information("geometry adjust mode = Mesh");
    raw_config().adjust_mode = MeshMode_MESH;
    control(Command_ModeDebug);
}
static void loggin_rendering_mode(int mode)
{
    QString modestr ;
    if (mode & Rendering_Grid)
        modestr = "Grid ";
    else if (mode & Rendering_Image)
        modestr = "Image ";
    else if (mode & Rendering_Media)
        modestr = "Media ";
    else if (mode & Rendering_Desktop)
        modestr = "Desktop ";

    if (mode & Rendering_Blend)
        modestr += "| Blend";
    if (mode & Rendering_Mesh)
        modestr += "| Mesh";
    if (mode & Rendering_Point)
        modestr += "| Point";

    log_information("rendering mode = "+modestr);
}
void tools_gui::handle_rendering_gui(int rd)
{
    if (rd & Rendering_Blend)
        rendering_attach_fusion->setChecked(true);
    if (rd & Rendering_Mesh)
        rendering_attach_grid->setChecked(true);
    if (rd & Rendering_Point)
        rendering_attach_point->setChecked(true);

    if (rd & Rendering_Image)
        rendering_image->setChecked(true);
    else if (rd & Rendering_Grid)
        rendering_grid->setChecked(true);
    else if (rd & Rendering_Desktop)
        rendering_desktop->setChecked(true);
    else if (rd & Rendering_Media)
        rendering_media->setChecked(true);
}
void tools_gui::handle_rendering(int rd)
{
    raw_config().display_mode = rd;
    if (rendering_attach_fusion->isChecked ())
    {
        raw_config().display_mode |= Rendering_Blend;
    }
    if (rendering_attach_grid->isChecked ())
    {
        raw_config().display_mode |= Rendering_Mesh;
    }
    if (rendering_attach_point->isChecked ())
    {
        raw_config().display_mode |= Rendering_Point;
    }
    loggin_rendering_mode(raw_config().display_mode);
    control(Command_RenderingMode);
}
#define RenderingMask 0xFF

void tools_gui::handle_attach(int aa)
{
    if (this->rendering_media->isChecked())
    {
        raw_config().display_mode = Rendering_Media;
    }
    else if (rendering_image->isChecked())
    {
        raw_config().display_mode = Rendering_Image;
    }
    else if (rendering_grid->isChecked())
    {
        raw_config().display_mode = Rendering_Grid;
    }
    else if (rendering_desktop->isChecked())
    {
        raw_config().display_mode = Rendering_Desktop;
    }

    if (rendering_attach_fusion->isChecked())
    {
        raw_config().display_mode |= Rendering_Blend;
    }
    if (rendering_attach_point->isChecked())
    {
        raw_config().display_mode |= Rendering_Point;
    }
    if (rendering_attach_grid->isChecked())
    {
        raw_config().display_mode |= Rendering_Mesh;
    }

    raw_config().display_mode &= aa;
    loggin_rendering_mode(raw_config().display_mode);
    control(Command_RenderingMode);
}
void tools_gui::on_rendering_grid_clicked()
{
    handle_rendering(Rendering_Grid) ;

    if (attach_debug_left->isHidden())
        attach_debug->setText(">");
    else
         attach_debug_left->setText("<");
    control(Command_AttachDebugOff);
}

void tools_gui::on_rendering_image_clicked()
{
    if (multimedia_image_local->isChecked())
    {
        int index = multimedia_image_local_select->currentIndex();
        raw_config().media_image_local_index = index;
        memcpy(raw_config().media_image_local_select,
               LocalImagePathStrq[index].toUtf8().data(),
               LocalImagePathStrq[index].toUtf8().size());
        control(Command_SelectImage);
    }
    else
    {
        int index = multimedia_image_other_select->currentIndex();
        raw_config().media_image_other_index = index;
        if (index != 0 && index != 1)
        {
            memcpy(raw_config().media_image_local_select,
                   multimedia_image_other_select->currentData().toString().toUtf8().data(),
                   multimedia_image_other_select->currentData().toString().toUtf8().size());
            control(Command_SelectOtherImage);
        }
        else
        {
            int index = multimedia_image_local_select->currentIndex();
            raw_config().media_image_local_index = index;
            memcpy(raw_config().media_image_local_select,
                   LocalImagePathStrq[index].toUtf8().data(),
                   LocalImagePathStrq[index].toUtf8().size());
            control(Command_SelectImage);
        }
    }

    handle_rendering(Rendering_Image) ;
    if (attach_debug_left->isHidden())
    {
        attach_debug->setText("<");
        wParameBegin();
        wParame << this->geometry();
        wParameEnd();
        control(Command_AttachDebugOn);
    }
    else
    {
        attach_debug_left->setText(">");
        wParameBegin();
        wParame << this->geometry();
        wParameEnd();
        control(Command_AttachDebugOn);
    }
}

void tools_gui::on_rendering_media_clicked()
{
    handle_rendering(Rendering_Media) ;

    if (multimedia_image_local->isChecked())
    {
        int index = multimedia_image_local_select->currentIndex();
        raw_config().media_image_local_index = index;
        memcpy(raw_config().media_image_local_select,
               LocalImagePathStrq[index].toUtf8().data(),
               LocalImagePathStrq[index].toUtf8().size());
        control(Command_SelectImage);
    }
    else
    {
        int index = multimedia_image_other_select->currentIndex();
        raw_config().media_image_other_index = index;
        if (index != 0 && index != 1)
        {
            memcpy(raw_config().media_image_local_select,
                   multimedia_image_other_select->currentData().toString().toUtf8().data(),
                   multimedia_image_other_select->currentData().toString().toUtf8().size());
            control(Command_SelectOtherImage);
        }
        else
        {
            int index = multimedia_image_local_select->currentIndex();
            raw_config().media_image_local_index = index;
            memcpy(raw_config().media_image_local_select,
                   LocalImagePathStrq[index].toUtf8().data(),
                   LocalImagePathStrq[index].toUtf8().size());
            control(Command_SelectImage);
        }
    }

    if (multimedia_media_select->currentIndex() > 1)
        return;


    QString file;
    //! open image file
    file = QFileDialog::getOpenFileName(this, tr("Select Media File"),
                                        QString() ,video_stl,
                                        0, QFileDialog::DontUseNativeDialog);

    if (!file.isEmpty())
    {
        QFileInfo info(file);
        multimedia_media_select->addItem(info.fileName(), file);
        multimedia_media_select->setCurrentText(info.fileName());
        raw_config().media_video_index = multimedia_media_select->currentIndex();
        memset(raw_config().media_video_select, 0, 128);
        memcpy(raw_config().media_video_select, file.toUtf8().data(),
               file.toUtf8().size());
        log_information("select media = "+file);
        control(Command_SelectMedia);

        if (system_junior->isChecked())
            tools_box->setCurrentIndex(3);
        else
            tools_box->setCurrentIndex(4);

        if (attach_debug_left->isHidden())
        {
            attach_debug->setText("<");
            wParameBegin();
            wParame << this->geometry();
            wParameEnd();
            control(Command_AttachDebugOn);
        }
        else
        {
            attach_debug_left->setText(">");
            wParameBegin();
            wParame << this->geometry();
            wParameEnd();
            control(Command_AttachDebugOn);
        }
    }
    else
    {
        if (system_junior->isChecked())
            tools_box->setCurrentIndex(2);
        else
            tools_box->setCurrentIndex(3);

        if (attach_debug_left->isHidden())
            attach_debug->setText(">");
        else
             attach_debug_left->setText("<");
        control(Command_AttachDebugOff);
    }

}

void tools_gui::on_rendering_desktop_clicked()
{
    handle_rendering(Rendering_Desktop) ;
    if (multimedia_image_local->isChecked())
    {
        int index = multimedia_image_local_select->currentIndex();
        raw_config().media_image_local_index = index;
        memcpy(raw_config().media_image_local_select,
               LocalImagePathStrq[index].toUtf8().data(),
               LocalImagePathStrq[index].toUtf8().size());
        control(Command_SelectImage);
    }
    else
    {
        int index = multimedia_image_other_select->currentIndex();
        raw_config().media_image_other_index = index;
        if (index != 0 && index != 1)
        {
            memcpy(raw_config().media_image_local_select,
                   multimedia_image_other_select->currentData().toString().toUtf8().data(),
                   multimedia_image_other_select->currentData().toString().toUtf8().size());
            control(Command_SelectOtherImage);
        }
        else
        {
            int index = multimedia_image_local_select->currentIndex();
            raw_config().media_image_local_index = index;
            memcpy(raw_config().media_image_local_select,
                   LocalImagePathStrq[index].toUtf8().data(),
                   LocalImagePathStrq[index].toUtf8().size());
            control(Command_SelectImage);
        }
    }

    if (attach_debug_left->isHidden())
    {
        attach_debug->setText("<");
        wParameBegin();
        wParame << this->geometry();
        wParameEnd();
        control(Command_AttachDebugOn);
    }
    else
    {
        attach_debug_left->setText(">");
        wParameBegin();
        wParame << this->geometry();
        wParameEnd();
        control(Command_AttachDebugOn);
    }
}

void tools_gui::on_rendering_attach_fusion_stateChanged(int arg1)
{
    handle_attach(arg1 > 0 ? RenderingMask: ~Rendering_Blend);
}

void tools_gui::on_rendering_attach_grid_stateChanged(int arg1)
{
    handle_attach(arg1 > 0 ? RenderingMask: ~Rendering_Mesh);
}

void tools_gui::on_rendering_attach_point_stateChanged(int arg1)
{
    handle_attach(arg1 > 0 ? RenderingMask: ~Rendering_Point);
}

void tools_gui::on_multimedia_image_local_clicked()
{
    multimedia_image_other_select->setEnabled(false);
    multimedia_image_local_select->setEnabled(true);
    raw_config().media_image_mode = 1;

    raw_config().media_image_local_index = multimedia_image_local_select->currentIndex();
    QString file = LocalImagePathStrq[multimedia_image_local_select->currentIndex()];

    memset(raw_config().media_image_local_select, 0, 128);
    memcpy(raw_config().media_image_local_select, file.toUtf8().data(),
           file.toUtf8().size());
    log_information("select image local = "+file);
    control(Command_SelectImage);
}

void tools_gui::on_multimedia_image_local_select_activated(int index)
{
    if (index < 0)
        return;
    raw_config().media_image_local_index =  index;
    memset(raw_config().media_image_local_select, 0, 128);
    memcpy(raw_config().media_image_local_select,
           LocalImagePathStrq[index].toUtf8().data(),
           LocalImagePathStrq[index].toUtf8().size());
    log_information("select image local = "+LocalImagePathStrq[index]);
    control(Command_SelectImage);
}

void tools_gui::on_multimedia_image_other_clicked()
{
    multimedia_image_other_select->setEnabled(true);
    multimedia_image_local_select->setEnabled(false);
    raw_config().media_image_mode = 2;
    log_information("select image other :");
    if (multimedia_image_other_select->currentIndex() > 1)
    {
        raw_config().media_image_other_index = multimedia_image_other_select->currentIndex();
        QString file = multimedia_image_other_select->currentData().toString();
        memcpy(raw_config().media_image_other_select, file.toUtf8().data(),
               file.toUtf8().size());
        log_information("    path =  "+file);
        control(Command_SelectOtherImage);
    }
}

void tools_gui::on_multimedia_image_other_select_activated(int index)
{
    QString file;
    if (index == 1)
    {
        //! open image file
        file = QFileDialog::getOpenFileName(this, tr("Select Image File"),
                                             QString() ,image_stl,
                                            0, QFileDialog::DontUseNativeDialog);
        if (!file.isEmpty())
        {
            QFileInfo info(file);
            multimedia_image_other_select->addItem(info.fileName(), file);
            multimedia_image_other_select->setCurrentText(info.fileName());
        }
    }
    else if (index != 0)
    {
        file = multimedia_image_other_select->currentData().toString();
    }

    if (!file.isEmpty())
    {
        raw_config().media_image_other_index =  index;
        memset(raw_config().media_image_other_select, 0, 128);
        memcpy(raw_config().media_image_other_select, file.toUtf8().data(),
               file.toUtf8().size());
        log_information("select image other = "+file);
        control(Command_SelectOtherImage);
    }
    else
        multimedia_image_other_select->setCurrentIndex(0);
}

void tools_gui::on_multimedia_sequence_select_activated(int index)
{
    QString dir;
    if (index == 1)
    {
        QFileDialog* openFilePath = new
                QFileDialog( this, "Select Directory", "file");
        openFilePath->setFileMode( QFileDialog::DirectoryOnly );
        if (openFilePath->exec() == QDialog::Accepted )
        {
            //code here！
            dir = openFilePath->directory().path();
            multimedia_sequence_select->addItem(dir);
            multimedia_sequence_select->setCurrentText(dir);
        }
        delete openFilePath;
    }
    else if (index != 0)
    {
        dir = multimedia_sequence_select->currentText();
    }
    if (!dir.isEmpty())
    {
        raw_config().media_sequence_index = index;
        memset(raw_config().media_sequence_dir, 0, 128);
        memcpy(raw_config().media_sequence_dir, dir.toUtf8().data(),
               dir.toUtf8().size());
        log_information("select sequence directory = "+dir);
        control(Command_SelectSequenceCatalog);
    }
    else
        multimedia_sequence_select->setCurrentIndex(0);
}

void tools_gui::on_multimedia_sequence_speed_valueChanged(int arg1)
{
    raw_config().media_sequence_speed = arg1;
    log_information("select sequence play speed = "+QString::number(arg1));

    control(Command_SelectSequenceCatalog);
}

void tools_gui::on_select_video_time()
{
    d->select_video->stop();

    QString file;
    //! open image file
    file = QFileDialog::getOpenFileName(this, tr("Select Media File"),
                                        QString() ,video_stl,
                                        0, QFileDialog::DontUseNativeDialog);

    if (!file.isEmpty())
    {
        QFileInfo info(file);
        multimedia_media_select->addItem(info.fileName(), file);
        multimedia_media_select->setCurrentText(info.fileName());
        raw_config().media_video_index = multimedia_media_select->currentIndex();
        memset(raw_config().media_video_select, 0, 128);
        memcpy(raw_config().media_video_select, file.toUtf8().data(),
               file.toUtf8().size());
        log_information("select media = "+file);
        control(Command_SelectMedia);

        if (system_junior->isChecked())
            tools_box->setCurrentIndex(3);
        else
            tools_box->setCurrentIndex(4);
    }
}
void tools_gui::on_multimedia_media_select_activated(int index)
{
    QString file;
    if (index == 1)
    {
        //! open image file
        file = QFileDialog::getOpenFileName(this, tr("Select Media File"),
                                             QString() ,video_stl,
                                            0, QFileDialog::DontUseNativeDialog);
        if (!file.isEmpty())
        {
            QFileInfo info(file);
            multimedia_media_select->addItem(info.fileName(), file);
            multimedia_media_select->setCurrentText(info.fileName());
        }
    }

    if ((index != 0) && (index != 1))
    {
        file = multimedia_media_select->currentData().toString();
    }

    if (!file.isEmpty())
    {
        raw_config().media_video_index = index;
        memset(raw_config().media_video_select, 0, 128);
        memcpy(raw_config().media_video_select, file.toUtf8().data(),
               file.toUtf8().size());
        log_information("select media = "+file);
        control(Command_SelectMedia);
    }
    else
        multimedia_media_select->setCurrentIndex(0);
}

void tools_gui::on_control_play_stop_clicked()
{
    control(Command_PlayStop);
}

void tools_gui::on_control_pause_continue_clicked()
{
    control(Command_PauseContinue);
}

void tools_gui::on_control_volum_valueChanged(int value)
{
    raw_config().media_volum = (float)value / 100.;
    control(Command_Volum);
}

void tools_gui::on_control_default_clicked()
{
    raw_config().media_play_mode = Play_Default;
    log_information("media play mode = default");
    control(Command_PlayMode);
}

void tools_gui::on_control_loop_play_clicked()
{
    raw_config().media_play_mode = Play_Loop;
    log_information("media play mode = loop");
    control(Command_PlayMode);
}

void tools_gui::on_control_command_play_clicked()
{
    raw_config().media_play_mode = Play_Command;
    log_information("media play mode = command");
    control(Command_PlayMode);
}


void tools_gui::on_attach_debug_clicked()
{
    if (attach_debug->text() == ">")
    {
        attach_debug->setText("<");
    }
    else
    {
        attach_debug->setText(">");
    }
    wParameBegin();
    wParame << this->geometry();
    wParameEnd();
    control(Command_AttachDebug);
}
void tools_gui::on_attach_debug_left_clicked()
{
    if (attach_debug_left->text() == "<")
    {
        attach_debug_left->setText(">");
    }
    else
    {
        attach_debug_left->setText("<");
    }
    wParameBegin();
    wParame << this->geometry();
    wParameEnd();
    control(Command_AttachDebug);
}


void tools_gui::on_save_geometry_clicked()
{
    //保存几何

    if (system_junior->isChecked())
        tools_box->setCurrentIndex(2);
    else
        tools_box->setCurrentIndex(3);
    if (!d->select_video)
    {
        d->select_video = new QTimer();
        connect(d->select_video, &QTimer::timeout ,
                this, &tools_gui::on_select_video_time);
    }
    d->select_video->start(100);
}

void tools_gui::on_load_geometry_clicked()
{
    handle_gui();
}

void tools_gui::on_step_save_clicked()
{

}

void tools_gui::on_step_load_activated(const QString &arg1)
{

}

void tools_gui::on_decode_mode_cpu_clicked()
{
    raw_config().decoder_mode = Decoder_CPU;
    log_information("player decoder mode = CPU");
}

void tools_gui::on_decoder_mode_gpu_clicked()
{
    raw_config().decoder_mode = Decoder_GPU;
    log_information("player decoder mode = GPU");
}

void tools_gui::on_decoder_mode_opencl_clicked()
{
    raw_config().decoder_mode = Decoder_OpenCL;
    log_information("player decoder mode = OpenCL");
}

void tools_gui::on_multimedia_start_desktop_clicked()
{
    control(Command_StartStopDesktop);

    if (!d->enable_desktop)
    {
        multimedia_start_desktop->setText(tr("Disable"));
        multimedia_capture_desktop->setEnabled(true);
        d->enable_desktop = true;
    }
    else
    {
        multimedia_start_desktop->setText(tr("Enable"));
        multimedia_capture_desktop->setEnabled(false);
        d->enable_desktop = false;
    }
}

void tools_gui::on_multimedia_frame_rate_valueChanged(int arg1)
{
    raw_config().desktop_frame_rate = arg1;
    control(Command_DesktopFrameRate);
}

void tools_gui::on_multimedia_capture_desktop_clicked()
{
    control(Command_DesktopCapture);
}

void tools_gui::on_system_data_load_clicked()
{
    control(Command_SystemLoad);
}

void tools_gui::on_system_data_save_clicked()
{
    control(Command_SystemSave);
}

void tools_gui::on_hide_tools_clicked()
{
    control(Command_HideTools);
}

void tools_gui::on_exit_system_clicked()
{
    control(Command_ExitSystem);
}

void tools_gui::on_device_mouse_adjust_clicked()
{

}

void tools_gui::on_device_keyboard_adjust_clicked()
{

}

void tools_gui::on_device_move_pixel_activated(int index)
{
    raw_config().adjust_move_pixel =
            device_move_pixel->currentText().toLatin1().toInt();

    control(Command_MovePixel);
}

