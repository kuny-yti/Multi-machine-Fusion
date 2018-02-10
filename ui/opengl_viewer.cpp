#include "opengl_viewer.h"
#include "intersect_test.h"
#include "fbo_image.h"
#include "fusion_render.h"

#include <QThread>
#include <QMutex>
#include <QOpenGLFramebufferObject>
#include <QDesktopWidget>
#include <QApplication>
#include <QGLWidget>
#include <QMouseEvent>
#include <QIcon>
#include <QDir>

#include "direct_show.h"
#include "Desktop.h"


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

/*
const uint KeySelectIndex[10][][2] =
{
    {
        {0,0}
    },//1
    {
        {0,1},
        {1,0},
    },//2
    {
        {0,2},
        {2,0},
        {1,0}//5*2,//4*2,//3*2,//2*2
    },//3
    {
        {0,3},
        {3,0},
        {1,0},//3*3,//2*3
        {1,1}//5*2,//4*2,//3*2,//2*2
    },//4
    {
        {0,4},
        {4,0},
        {1,0},//2*4
        {1,1},//3*3,//2*3
        {2,0}//5*2,//4*2,//3*2
    },//5
    {
        {0,5},
        {5,0},
        {1,0},//2*5
        {1,1},//2*4
        {1,2},//3*3,//2*3
        {2,1}//5*2,//4*2,//3*2
    },//6
    {
        {0,6},
        {6,0},
        {1,1},//2*5
        {1,2},//2*4
        {2,0},//3*3
        {3,0}//5*2,//4*2
    },//7
    {
        {0,7},
        {7,0},
        {1,2},//2*5
        {1,3},//2*4
        {2,1},//3*3
        {3,1}//5*2,//4*2
    },//8
    {
        {0,8},
        {8,0},
        {1,3},//2*5
        {2,2},//3*3
        {4,0}//5*2
    },//9
    {
        {0,9},
        {9,0},
        {1,4},//2*5
        {4,1},//5*2
    }//0
};*/

class thread_loop :public QThread
{
private:
    volatile bool _is_run;
    opengl_viewer *_view;
public:
    bool has_mouse;
    bool has_desktop;
    bool has_movie;
    int frame_rate;
    ulong capture_index;
    bool is_capture;
    Desktop *desktop;
    direct_show *dmovie;

public:
    thread_loop(opengl_viewer *vi):
        _view(vi),
        desktop(NULL),
        dmovie(NULL)
    {
        is_capture = false;
        capture_index = 0;
        has_desktop = false;
        frame_rate = 25.0*1.5;
        has_mouse = false;
        _is_run = false;
        has_movie = false;

        dmovie = new direct_show();
        desktop = new Desktop();
        desktop->init();
    }

    ~thread_loop()
    {
        if (dmovie)
            delete dmovie;
        if (desktop)
            delete desktop;
        desktop = NULL;
        dmovie = NULL;
    }

    void begin()
    {
        if (!_is_run)
        {
            _is_run = true;
            this->start();
            has_desktop = false;
            has_movie =true;
        }
    }
    void end()
    {
        if (_is_run)
        {
            /*
            while (dispatch::playing())
            {
            }*/
            _is_run = false;
            has_movie =false;
            this->wait();
        }
    }
    void begin(int rate)
    {
        frame_rate = (float)rate*1.5;
        end(true);
        _is_run = true;
        has_movie =false;
        has_desktop = true;
        this->start();
    }
    void end(bool )
    {
        if (_is_run)
        {
            _is_run = false;
            has_desktop = false;
            this->wait();
        }
    }
    void run()
    {
        static QPointF dPos;
        while(_is_run)
        {
            if (has_desktop)
            {
                if (desktop->size().isEmpty())
                {
                    has_desktop = false;
                    //delete desktop;
                    //desktop = NULL;
                    controls(Error_StartDesktop);
                    break;
                }

                if (has_mouse)
                {
                    QDesktopWidget *dWidget = QApplication::desktop();
                    if (QCursor::pos ().x () >
                            dWidget->screenGeometry(0).width ())
                    {
                        QCursor::setPos (dWidget->screenGeometry(0).width (),
                                         QCursor::pos ().y ());
                    }
                    QSizeF dWs = QSizeF(dWidget->screenGeometry(0).width (),
                                        dWidget->screenGeometry(0).height ());
                    dPos = QPointF(
                                (float)QCursor::pos ().x() / dWs.width(),
                                1.0-(float)QCursor::pos ().y() / dWs.height());
                }
                _view->activate_desktop(desktop->image(), dPos);
                if (is_capture)
                {
                    desktop->image().
                            save(QString("desktop-%1.jpg").arg(capture_index));
                    capture_index++;
                    is_capture = false;
                }
                else
                    msleep(1000 / frame_rate);
                continue;
            }

            if (has_movie)
            {
                dmovie->update();
                if (dmovie->getPixels())
                    _view->activate_movie(dmovie->getPixels(),
                                             QSize(dmovie->getWidth(),
                                             dmovie->getHeight()));
                msleep(10);
            }

        }
        _is_run = false;
    }
};

class opengl_viewer_p : public intersect_test
{
public:
    fbo_image *handle_image;
    fusion_render *render;

    QMutex call_mutex;
    bool is_initGL;

    bool has_blend_update;
    BlendConfig  blend_update;
    QVector<QVector<BlendConfig> >  blend_data;
    QVector<QVector<QRectF> >  coord_data;

    bool has_image_file;
    QString image_file;

    int display_mode;

    bool has_start_debug;
    bool last_fbo_data;
    bool stop_run;

    bool has_select_begin;
    QPoint select_point;
    bool has_select_move;
    bool has_select_end;
    thread_loop *loop_thread;

    bool has_key_move;
    QVector3D key_move;
    int key_move_pixel;
    int key_device_id;
    int key_move_index;

    bool has_movie_image;
    uchar *movie_image;
    QSize movie_size;
    QOpenGLFramebufferObject *movie_fbo;
    QSize movie_last_size;
     uint movie_texture;

    bool has_desktop_image;
    QImage desktop_image;
    QPointF desktop_pos;
    QOpenGLFramebufferObject *desktop_fbo;
    uint desktop_mouse;
    uint desktop_texture;
    QImage cursor_mouse;
    QImage watermark_image;
    uint watermark_texture;

    bool has_data_load;
    QByteArray data_load;

    opengl_viewer_p():
        handle_image(NULL),
        movie_fbo(NULL),
        render(NULL),
        loop_thread(NULL),
        blend_data(),
        coord_data(),
        is_initGL(false),
        has_desktop_image(false),
        has_movie_image(false),
        desktop_fbo(NULL),
        desktop_mouse(0),
        desktop_pos(),
        desktop_texture(0),
        key_move(),
        key_move_pixel(10),
        key_device_id(0),
        key_move_index(0),
        has_blend_update(false),
        has_image_file(false),
        has_start_debug(false),
        has_data_load(false),
        last_fbo_data(false),
        has_key_move(false),
        stop_run(false),
        select_point(),
        display_mode(0),
        movie_last_size(),
        movie_texture(0),
        movie_image(NULL)
    {
        has_select_begin = false;
        render = new fusion_render();
    }
    ~opengl_viewer_p()
    {
        if (loop_thread)
        {
            if (loop_thread->has_desktop)
            {
                loop_thread->end(true);
            }
            else if (loop_thread->has_movie)
            {
                loop_thread->end();
            }
            //loop_thread->end();
            delete loop_thread;
            loop_thread = NULL;
        }
        if (render)
            delete render;
        if (desktop_fbo)
            delete desktop_fbo;
        if (movie_fbo)
            delete movie_fbo;
        if (movie_image)
            delete []movie_image;
        //if (handle_image)
        //    delete handle_image;
    }
    void init_blend_data(const RawConfig &config)
    {
        for (int i =0; i < blend_data.size(); i++)
        {
            blend_data[i].clear();
        }
        blend_data.clear();

        blend_data.resize(idHigh(config.device_layout));
        for (int i = 0; i < blend_data.size(); i++)
        {
            blend_data[i].resize(idLow(config.device_layout));
            for (int j = 0; j < blend_data[i].size(); j++)
            {
                blend_data[i][j].device_id = idTools(i, j);
                blend_data[i][j].edge = 0;

                //blend_data[i][j].left.area = QRectF() ;
                blend_data[i][j].left.brightness = 0.5;
                blend_data[i][j].left.gamma = 3.0;
                blend_data[i][j].left.exponent = 0.9;

                //blend_config[i][j].right.area = QRectF() ;
                blend_data[i][j].right.brightness = 0.5;
                blend_data[i][j].right.gamma = 3.0;
                blend_data[i][j].right.exponent = 0.9;

                //blend_config[i][j].top.area = QRectF() ;
                blend_data[i][j].top.brightness = 0.5;
                blend_data[i][j].top.gamma = 3.0;
                blend_data[i][j].top.exponent = 0.9;

                //blend_config[i][j].bottom.area = QRectF() ;
                blend_data[i][j].bottom.brightness = 0.5;
                blend_data[i][j].bottom.gamma = 3.0;
                blend_data[i][j].bottom.exponent = 0.9;
            }
        }
    }
    void init_coord_data(const RawConfig &config)
    {
        float cw = (float)1.0 / idLow(config.device_layout);//每通道纹理坐标宽度
        float ch = (float)1.0 / idHigh(config.device_layout);//每通道纹理坐标高度

        for (int i =0; i < coord_data.size(); i++)
        {
            coord_data[i].clear();
        }
        coord_data.clear();

        coord_data.resize(idHigh(config.device_layout));
        for (int i = 0; i < coord_data.size(); i++)
        {
            coord_data[i].resize(idLow(config.device_layout));
            for (int j = 0; j < coord_data[i].size(); j++)
            {
                coord_data[i][j] = QRectF((float)j*cw, (float)i*ch, cw, ch);
            }
        }
    }

    int computer(const RawConfig &config)
    {
        int dirs = 0;
        if (idLow(config.computer_layout) == 1)
            return dirs;

        if (idLow(config.computer_index) > 0)
        {
            dirs |= Blend_Left;
        }
        if ((idLow(config.computer_index)+1) < idLow(config.computer_layout))
        {
            dirs |= Blend_Right;
        }
        return dirs;
    }
    int computer_blend(const RawConfig &config)
    {
        int dirs = 0;
        if (idHigh(config.computer_layout) == 1)
            return computer(config);

        if (idHigh(config.computer_index) > 0)
        {
            dirs |= Blend_Bottom;
        }
        if ((idHigh(config.computer_index)+1) < idHigh(config.computer_layout))
        {
            dirs |= Blend_Top;
        }
        dirs |= computer(config);
        return dirs;
    }

    int device(int index, const RawConfig &config)
    {
        int dirs = 0;
        if (idLow(config.device_layout) == 1)
            return dirs;

        if (idLow(index) > 0)
        {
            dirs |= Blend_Left;
        }
        if ((idLow(index)+1) < idLow(config.device_layout))
        {
            dirs |= Blend_Right;
        }
        return dirs;
    }
    int device_blend(int index, const RawConfig &config)
    {
        int dirs = 0;
        if (idHigh(config.device_layout) == 1)
            return device(index, config);

        if (idHigh(index) > 0)
        {
            dirs |= Blend_Bottom;
        }
        if ((idHigh(index)+1) < idHigh(config.device_layout))
        {
            dirs |= Blend_Top;
        }
        dirs |= device(index, config);
        return dirs;
    }
};

opengl_viewer::opengl_viewer(QWindow *parent):
    window_view(parent),
    d(new opengl_viewer_p())
{
    d->loop_thread = new thread_loop(this);

    setIcon (QIcon(":/image/yantu.ico"));
    setFlags (Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
    log_information("begin opengl .");
}

opengl_viewer::~opengl_viewer()
{
    log_information("end opengl .");

    if (d)
        delete d;
}

MediaPlayMode opengl_viewer::media_play_mode()
{
    return raw_config().media_play_mode;
}
void opengl_viewer::command(const int& cmd)
{
    switch (cmd)
    {
    case Command_StartDebug:
        d->call_mutex.lock();
        d->has_start_debug = true;
        d->call_mutex.unlock();

        break;
    case Command_RenderingMode:
        display_mode(raw_config().display_mode);
        break;
    case Command_ModeDebug:
        d->call_mutex.lock();
        d->render->mode_ctrl () = (CtrlMode)raw_config().adjust_mode;
        d->call_mutex.unlock();
        break;
    case Command_SelectImage:
        d->call_mutex.lock();
        d->image_file = raw_config().media_image_local_select;
        d->has_image_file = true;
        d->call_mutex.unlock();
        break;
    case Command_SelectOtherImage:
        d->call_mutex.lock();
        d->image_file = raw_config().media_image_other_select;
        d->has_image_file = true;
        d->call_mutex.unlock();
        break;
    case Command_SelectMedia:
        if (d->loop_thread->dmovie->isLoaded() && d->loop_thread->dmovie->isPlaying())
        {
            if (d->loop_thread)
                d->loop_thread->end();
            d->loop_thread->dmovie->stop();
            raw_config().media_state = 0;
            control(~Command_PlayStop);
        }
        d->loop_thread->dmovie->loadMovie(raw_config().media_video_select);
        log_information("player open movie.");
        break;
    case Command_PlayStop:
        if (d->loop_thread->dmovie->isLoaded() &&
                (d->loop_thread->dmovie->isPlaying() ||
                 d->loop_thread->dmovie->isPaused ()))
        {
            d->loop_thread->dmovie->stop();

            if (d->loop_thread)
                d->loop_thread->end();

            log_information("player stop .");
            raw_config().media_state = 0;
            control(~Command_PlayStop);
        }
        else
        {

            if (d->loop_thread)
                d->loop_thread->begin();

            if (d->loop_thread->dmovie->isLoaded ())
            {
                d->loop_thread->dmovie->play();
                log_information("player play .");
                raw_config().media_state = 1;
                control(~Command_PlayStop);
            }
            else if (!QString(raw_config ().media_video_select).isEmpty ())
            {
                command(Command_SelectMedia);
                d->loop_thread->dmovie->play();
                log_information("player play .");
                raw_config().media_state = 1;
                control(~Command_PlayStop);
            }
            else
            {
                log_information("player play error(not load movie).");
            }
        }

        break;
    case Command_PlayMode:
        if (raw_config().media_play_mode == Play_Loop)
        {
            d->loop_thread->dmovie->setLoopState(true);
        }
        else
        {
            d->loop_thread->dmovie->setLoopState(false);
        }
        break;
    case Command_PauseContinue:
        if (d->loop_thread->dmovie->isLoaded())
        {
            if (d->loop_thread->dmovie->isPaused() &&
                    !d->loop_thread->dmovie->isPlaying ())
            {
                d->loop_thread->dmovie->setPaused(false);
                log_information("player continue .");
                raw_config().media_state = 1;
            }
            else if (d->loop_thread->dmovie->isPlaying ())
            {
                d->loop_thread->dmovie->setPaused(true);
                log_information("player pause .");
                raw_config().media_state = 2;
            }
            control(~Command_PlayStop);
        }

        break;
    case Command_Volum:
        log_information("player audio volume(%) = "+
                        QString::number(raw_config().media_volum));
        d->loop_thread->dmovie->setVolume(raw_config().media_volum);
        break;

    case Command_Color:
        d->handle_image->update_color(raw_config().color_config.device_id,
                                      raw_config().color_config.brightness,
                                      raw_config().color_config.contrast,
                                      raw_config().color_config.hue,
                                      raw_config().color_config.saturation);
        d->handle_image->update_primary (raw_config().color_config.device_id,
                                         raw_config().color_config.red,
                                         raw_config().color_config.green,
                                         raw_config().color_config.blue,
                                         raw_config().color_config.alpha);
        break;
    case Command_ComputerCut:
    {
        int temp = 0;
        QVector<QVector<QRectF> > coord;
        rParameBegin();
        rParame >> temp; coord.resize(temp);
        for (int i = 0; i < coord.size(); ++i)
        {
            rParame >> temp;coord[i].resize(temp);
            for (int j = 0; j < coord[i].size(); ++j)
            {
                rParame >> coord[i][j];
            }
        }
        rParameEnd();
        device_coord_update(coord);
    }
        break;
    case Command_DeviceCut:
    {
        int temp = 0;
        QVector<QVector<QRectF> > coord_computer;
        QVector<QVector<QRectF> > coord_device;
        rParameBegin();
        rParame >> temp; coord_device.resize(temp);
        for (int i = 0; i < coord_device.size(); ++i)
        {
            rParame >> temp; coord_device[i].resize(temp);
            for (int j = 0; j < coord_device[i].size(); ++j)
            {
                rParame >> coord_device[i][j];
            }
        }

        rParame >> temp; coord_computer.resize(temp);
        for (int i = 0; i < coord_computer.size(); ++i)
        {
            rParame >> temp; coord_computer[i].resize(temp);
            for (int j = 0; j < coord_computer[i].size(); ++j)
            {
                rParame >> coord_computer[i][j];
            }
        }
        rParameEnd();
        computer_coord_update(coord_device, coord_computer);
    }
        break;
    case Command_BlendLeft:
        d->call_mutex.lock();
        d->blend_update = raw_config().blend_config;
        d->blend_update.edge = Blend_Left;
        d->has_blend_update = true;
        d->call_mutex.unlock();
        break;
    case Command_BlendRight:
        d->call_mutex.lock();
        d->blend_update = raw_config().blend_config;
        d->blend_update.edge = Blend_Right;
        d->has_blend_update = true;
        d->call_mutex.unlock();
        break;
    case Command_BlendTop:
        d->call_mutex.lock();
        d->blend_update = raw_config().blend_config;
        d->blend_update.edge = Blend_Top;
        d->has_blend_update = true;
        d->call_mutex.unlock();
        break;
    case Command_BlendBottom:
        d->call_mutex.lock();
        d->blend_update = raw_config().blend_config;
        d->blend_update.edge = Blend_Bottom;
        d->has_blend_update = true;
        d->call_mutex.unlock();
        break;
    case Command_StartStopDesktop:
        if (d->loop_thread->desktop)
        {
            d->loop_thread->end(true);
            d->loop_thread->begin(raw_config().desktop_frame_rate);
        }
        else
            d->loop_thread->begin(raw_config().desktop_frame_rate);
        break;
    case Command_DesktopFrameRate:
        if (d->loop_thread)
            d->loop_thread->frame_rate = raw_config().desktop_frame_rate;
        break;
    case Command_DesktopCapture:
        if (d->loop_thread)
            d->loop_thread->is_capture = true;
    case Command_MovePixel:
        d->key_move_pixel = raw_config().adjust_move_pixel;
        break;
    default:
        break;
    }
    this->update();
}

bool opengl_viewer::event(QEvent *evt)
{
    switch (evt->type())
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mEvt = static_cast<QMouseEvent *>(evt);
        if (mEvt->button() == Qt::RightButton)
        {
            evt->accept();
            control(Command_ShowTools);
        }
    }
        break;
    case QEvent::MouseButtonRelease:
        evt->accept();
        break;
    case QEvent::KeyPress:
    {
        QKeyEvent *kEvt = static_cast<QKeyEvent *>(evt);

        if (kEvt->modifiers() != Qt::ControlModifier)
        {
            if (kEvt->key() == Qt::Key_P) // play/pause
                command(Command_PlayStop);
            else if (kEvt->key() == Qt::Key_C)
            {
                raw_config().media_image_other_index = 0 ;
                QString file = "d:/YanTu/image.jpg";
                memcpy(raw_config().media_image_other_select, file.toUtf8().data(), file.size());
                log_information("    path =  "+file);
                raw_config().display_mode = Rendering_Blend | Rendering_Image;
                control(Command_RenderingMode);
                control(Command_SelectOtherImage);
            }
            else if ((kEvt->key() - Qt::Key_0) <= 9 && (kEvt->key() - Qt::Key_0) >= 1)
            {
                const int idx = kEvt->key() - Qt::Key_0;
                raw_config().media_video_index = idx+1;
                static QStringList slfn, slfp;
                int cn = search_video(raw_config().dir_config_media_video, slfn, slfp);
                if (idx-1 < cn && idx -1 >= 0)
                {
                    QString file = slfp[idx-1];
                    if (!file.isEmpty())
                    {
                        memset(raw_config().media_video_select, 0, 128);
                        memcpy(raw_config().media_video_select, file.toUtf8().data(),
                               file.toUtf8().size());
                        log_information("select media = "+file);
                        raw_config().display_mode = Rendering_Blend | Rendering_Media;
                        control(Command_RenderingMode);
                        control(Command_SelectMedia);
                        control(Command_PlayStop);
                    }

                }
            }
            break;
        }
        else if (!(raw_config().display_mode & Rendering_Point) &&
             !(raw_config().display_mode & Rendering_Grid))
            break;

        d->call_mutex.lock();

        int dirs = d->device_blend(d->key_device_id, raw_config());

        if (kEvt->key() == Qt::Key_1)
        {
            if ((idLow(raw_config().device_layout) > 0) ||
                    (idHigh(raw_config().device_layout) > 0))
            {
                d->key_device_id = idTools(0, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_2)
        {
            if ((idLow(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(0, 1);
            }
            else if (idHigh(raw_config().device_layout) > 1)
            {
                d->key_device_id = idTools(1, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_3)
        {
            //5*2,//4*2,//3*2,//2*2
            if ((idLow(raw_config().device_layout) == 2) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 0);
            }
            else if ((idLow(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(0, 2);
            }
            else if (idHigh(raw_config().device_layout) > 2)
            {
                d->key_device_id = idTools(2, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_4)
        {
            //{1,0},//3*3,//2*3
            //{1,1}//5*2,//4*2,//3*2,//2*2
            if ((idLow(raw_config().device_layout) == 3) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 0);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 1);
            }
            else if ((idLow(raw_config().device_layout) > 3))
            {
                d->key_device_id = idTools(0, 3);
            }
            else if (idHigh(raw_config().device_layout) > 3)
            {
                d->key_device_id = idTools(3, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_5)
        {
            //{1,0},//2*4
            //{1,1},//3*3,//2*3
            //{2,0}//5*2,//4*2,//3*2
            if ((idLow(raw_config().device_layout) == 4) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 0);
            }
            else if ((idLow(raw_config().device_layout) == 3) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 1);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(2, 0);
            }
            else if ((idLow(raw_config().device_layout) > 4))
            {
                d->key_device_id = idTools(0, 4);
            }
            else if (idHigh(raw_config().device_layout) > 4)
            {
                d->key_device_id = idTools(4, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_6)
        {
            //{1,0},//2*5
            //{1,1},//2*4
            //{1,2},//3*3,//2*3
            //{2,1}//5*2,//4*2,//3*2
            if ((idLow(raw_config().device_layout) == 5) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 0);
            }
            else if ((idLow(raw_config().device_layout) == 4) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 1);
            }
            else if ((idLow(raw_config().device_layout) == 3) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 2);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(2, 1);
            }
            else if ((idLow(raw_config().device_layout) > 5))
            {
                d->key_device_id = idTools(0, 5);
            }
            else if (idHigh(raw_config().device_layout) > 5)
            {
                d->key_device_id = idTools(5, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_7)
        {
            //{1,1},//2*5
            //{1,2},//2*4
            //{2,0},//3*3
            //{3,0}//5*2,//4*2
            if ((idLow(raw_config().device_layout) == 5) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 1);
            }
            else if ((idLow(raw_config().device_layout) == 4) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 2);
            }
            else if ((idLow(raw_config().device_layout) == 3) &&
                     (idHigh(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(2, 0);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 3))
            {
                d->key_device_id = idTools(3, 0);
            }
            else if ((idLow(raw_config().device_layout) > 6))
            {
                d->key_device_id = idTools(0, 6);
            }
            else if (idHigh(raw_config().device_layout) > 6)
            {
                d->key_device_id = idTools(6, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_8)
        {
            //{1,2},//2*5
            //{1,3},//2*4
            //{2,1},//3*3
            //{3,1}//5*2,//4*2
            if ((idLow(raw_config().device_layout) == 5) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 2);
            }
            else if ((idLow(raw_config().device_layout) == 4) &&
                     (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 3);
            }
            else if ((idLow(raw_config().device_layout) == 3) &&
                     (idHigh(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(2, 1);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 3))
            {
                d->key_device_id = idTools(3, 1);
            }
            else if ((idLow(raw_config().device_layout) > 7))
            {
                d->key_device_id = idTools(0, 7);
            }
            else if (idHigh(raw_config().device_layout) > 7)
            {
                d->key_device_id = idTools(7, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_9)
        {
            //{1,3},//2*5
            //{2,2},//3*3
            //{4,0}//5*2
            if ((idLow(raw_config().device_layout) == 5) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 3);
            }
            else if ((idLow(raw_config().device_layout) == 3) &&
                     (idHigh(raw_config().device_layout) > 2))
            {
                d->key_device_id = idTools(2, 2);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 4))
            {
                d->key_device_id = idTools(4, 0);
            }
            else if ((idLow(raw_config().device_layout) > 8))
            {
                d->key_device_id = idTools(0, 8);
            }
            else if (idHigh(raw_config().device_layout) > 8)
            {
                d->key_device_id = idTools(8, 0);
            }
        }
        else if (kEvt->key() == Qt::Key_0)//10
        {
            //{1,4},//2*5
            //{4,1},//5*2
            if ((idLow(raw_config().device_layout) == 5) &&
                    (idHigh(raw_config().device_layout) > 1))
            {
                d->key_device_id = idTools(1, 4);
            }
            else if ((idLow(raw_config().device_layout) == 2) &&
                     (idHigh(raw_config().device_layout) > 4))
            {
                d->key_device_id = idTools(4, 1);
            }
            else if ((idLow(raw_config().device_layout) > 9))
            {
                d->key_device_id = idTools(0, 9);
            }
            else if (idHigh(raw_config().device_layout) > 9)
            {
                d->key_device_id = idTools(9, 0);
            }
        }

        else if (kEvt->key() == Qt::Key_W)
        {
            int v = 1;
            if (dirs & Blend_Bottom)
                v++;
            if (dirs & Blend_Top)
                v++;

            int value = idHigh(d->key_move_index);
            if (value < (idHigh(raw_config().geometry_control)*v-v))
                value ++;
            else
            {
                //value = 0;
                //调到下一通道
                if ((idHigh(d->key_device_id) + 1) <
                        idHigh(raw_config().device_layout))
                {
                    d->key_device_id = idTools(idHigh(d->key_device_id) + 1 ,
                                       idLow(d->key_device_id));
                    value = 0;
                }
            }
            d->key_move_index = idTools(value,
                                        idLow(d->key_move_index));
        }
        else if (kEvt->key() == Qt::Key_S)
        {
            int v = 1;
            if (dirs & Blend_Bottom)
                v++;
            if (dirs & Blend_Top)
                v++;
            int value = idHigh(d->key_move_index);
            if (value > 0)
                value --;
            else
            {
                //value = idHigh(raw_config().geometry_control)-1;
                //调到下一通道
                if ((idHigh(d->key_device_id) - 1) >= 0)
                {
                    d->key_device_id = idTools(idHigh(d->key_device_id) - 1 ,
                                       idLow(d->key_device_id));
                    value = idHigh(raw_config().geometry_control)-1;
                }
            }
            d->key_move_index = idTools(value,
                                        idLow(d->key_move_index));
        }
        else if (kEvt->key() == Qt::Key_A)
        {
            int v = 1;
            if (dirs & Blend_Left)
                v++;
            if (dirs & Blend_Right)
                v++;

            int value = idLow(d->key_move_index);
            if (value > 0)
                value --;
            else
            {
                //value = idLow(raw_config().geometry_control)*v-v;

                //调到下一通道
                if ((idLow(d->key_device_id) - 1) >= 0)
                {
                    d->key_device_id = idTools(idHigh(d->key_device_id) ,
                                       idLow(d->key_device_id) -1);
                    value = idLow(raw_config().geometry_control)*v-v;
                }
            }
            d->key_move_index = idTools(idHigh(d->key_move_index),
                                        value );
        }
        else if (kEvt->key() == Qt::Key_D)
        {
            int v = 1;
            if (dirs & Blend_Left)
                v++;
            if (dirs & Blend_Right)
                v++;

            int value = idLow(d->key_move_index);
            if (value < (idLow(raw_config().geometry_control)*v-v))
                value ++;
            else
            {
                //value = 0;
                //调到下一通道
                if ((idLow(d->key_device_id) + 1) <
                        (idLow(raw_config().device_layout)))
                {
                    d->key_device_id = idTools(idHigh(d->key_device_id) ,
                                       idLow(d->key_device_id) +1);
                    value = 0;
                }
            }
            d->key_move_index = idTools(idHigh(d->key_move_index),
                                        value );
        }
        else if (kEvt->key() == Qt::Key_Up)
        {
            d->key_move = QVector3D(0,
                                    d->key_move_pixel / (float)this->size().height(), 0);
        }
        else if (kEvt->key() == Qt::Key_Down)
        {
            d->key_move = QVector3D(0,
                                    -d->key_move_pixel/ (float)this->size().height(), 0);
        }
        else if (kEvt->key() == Qt::Key_Left)
        {
            d->key_move = QVector3D(-d->key_move_pixel/(float)this->size().height(),
                                    0, 0);
        }
        else if (kEvt->key() == Qt::Key_Right)
        {
            d->key_move = QVector3D(d->key_move_pixel/ (float)this->size().height(),
                                    0, 0);
        }

        int h = (idLow(raw_config().geometry_control) << 16) & 0xff0000;
        h |= (idHigh(raw_config().geometry_control) << 24) & 0xff000000;
        d->key_move_index = h | d->key_move_index;

        d->has_key_move = true;
        evt->accept();
        d->call_mutex.unlock();
        this->update();
        return true;
    }
        break;
    case QEvent::KeyRelease:
        d->call_mutex.lock();
        d->key_move = QVector3D();
        evt->accept();
        d->call_mutex.unlock();
        return true;
    default:
        break;
    }
    return window_view::event(evt);
}

void opengl_viewer::initializeGL()
{
    if (!d->handle_image)
    {
        d->handle_image = new fbo_image();
        d->handle_image->prepare(":/glsl/vertex.gl.glsl", ":/glsl/image.gl.glsl");
    }
    if (!d->is_initGL)
        d->is_initGL = true;

    d->handle_image->image_texture(LocalImagePath[Image_AndSymbol]);

    if (d->cursor_mouse.load(":/image/cursors.png"))
    {
        d->cursor_mouse = QGLWidget::convertToGLFormat (d->cursor_mouse);
    }
    else
        d->cursor_mouse = QImage();

    if (d->watermark_image.load (":/image/watermark.png"))
    {
        d->watermark_image = QGLWidget::convertToGLFormat (d->watermark_image);
        d->watermark_texture = texture_gen(d->watermark_image.size(),
                                           PIXEL_RGBA,  DATA_RGBA,
                                           DATA_UNSIGNED_BYTE,
                                           d->watermark_image.bits());
        texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
        texture_filter(FILTER_LINEAR, FILTER_LINEAR);
    }
    else
    {
        d->watermark_image = QImage();
        d->watermark_texture = 0;
    }
    if (raw_config().system_setup & 0x04)
    {
        this->setCursor(Qt::BlankCursor);
        this->hide();
        this->show();
    }

    log_information("opengl initialize.");
}
void opengl_viewer::paintGL()
{
    if (d->stop_run)
        return;

    render_kernel::clear ();
    setp_run();

    for (int i = 0; i < d->render->param ()._p_count_row; i++)
    {
        for (int j = 0; j < d->render->param ()._p_count_col; j++)
        {
            uint id = idTools(i, j);
            uint ids = d->handle_image->render (id);
            d->render->render (id, ids);
        }
    }

}

void opengl_viewer::select_begin(const QPoint &p)
{
    if (!(raw_config().display_mode & Rendering_Point) &&
         !(raw_config().display_mode & Rendering_Grid))
        return;

    d->call_mutex.lock();
    d->select_point = p;
    d->has_select_begin = true;
    d->call_mutex.unlock();
    this->update();
}
void opengl_viewer::select_begin(const QRectF &p)
{
    this->update();
}
void opengl_viewer::select_move(const QPoint &p)
{
    if (!(raw_config().display_mode & Rendering_Point) &&
         !(raw_config().display_mode & Rendering_Grid))
        return;
    d->call_mutex.lock();
    d->select_point = p;
    d->has_select_move = true;
    d->call_mutex.unlock();
    this->update();
}
void opengl_viewer::select_end()
{
    if (!(raw_config().display_mode & Rendering_Point) &&
         !(raw_config().display_mode & Rendering_Grid))
        return;
    d->call_mutex.lock();
    d->has_select_end = true;
    d->call_mutex.unlock();
    this->update();
}

void opengl_viewer::activate_desktop(const QImage &image, const QPointF &pos)
{
    d->call_mutex.lock();
    d->has_desktop_image = true;
    d->desktop_image = image;
    d->desktop_pos = pos;
    d->loop_thread->has_mouse = raw_config().system_setup & 1<<2;
    d->call_mutex.unlock();
    this->update();
}

void opengl_viewer::activate_movie(uchar *buff, const QSize &size)
{
    d->call_mutex.lock();
    d->has_movie_image = true;
    if (d->movie_size != size)
    {
        if (d->movie_image )
            delete []d->movie_image;

        d->movie_image = new uchar[size.width() *size.height() *3];
    }
    else if (d->movie_image)
    {
        memcpy(d->movie_image, buff, size.width() *size.height() *3);
    }

    d->movie_size = size;
    d->call_mutex.unlock();
    this->update();
}
bool opengl_viewer::setp_run()
{
    bool ret = false;
    d->call_mutex.lock();

    if (d->has_start_debug)
    {
        if (d->render)
        {
            delete d->render;
            d->render = new fusion_render();
        }
        d->render->param_set (raw_config());
        d->render->mesh_create ();

        d->init_blend_data(raw_config());
        d->init_coord_data(raw_config());
        d->last_fbo_data  =false;

        for (int i = 0; i < idHigh(raw_config().device_layout); i++)
        {
            for (int j = 0; j < idLow(raw_config().device_layout); j++)
            {
                QSize size = QSize(
                            DeviceResolution[raw_config().device_resolution][0],
                            DeviceResolution[raw_config().device_resolution][1]);
                d->handle_image->out_texture(idTools(i, j), size);

                {
                    d->handle_image->blend_texture (d->blend_data[i][j].device_id,
                                        d->blend_data[i][j].left.brightness,
                                        d->blend_data[i][j].left.gamma ,
                                        d->blend_data[i][j].left.exponent,
                                        Blend_Left);
                }
                {
                    d->handle_image->blend_texture (d->blend_data[i][j].device_id,
                                        d->blend_data[i][j].right.brightness,
                                        d->blend_data[i][j].right.gamma ,
                                        d->blend_data[i][j].right.exponent,
                                        Blend_Right);
                }
                {
                    d->handle_image->blend_texture (d->blend_data[i][j].device_id,
                                        d->blend_data[i][j].top.brightness,
                                        d->blend_data[i][j].top.gamma ,
                                        d->blend_data[i][j].top.exponent,
                                        Blend_Top);
                }
                {
                    d->handle_image->blend_texture (d->blend_data[i][j].device_id,
                                        d->blend_data[i][j].bottom.brightness,
                                        d->blend_data[i][j].bottom.gamma ,
                                        d->blend_data[i][j].bottom.exponent,
                                        Blend_Bottom);
                }
            }
        }
        d->call_mutex.unlock();
        computer_coord_update(d->coord_data);
        d->call_mutex.lock();
        d->has_start_debug = false;
    }
    if (d->has_movie_image && d->display_mode & Rendering_Media)
    {
        if ((d->movie_last_size != d->movie_size) ||
             (d->movie_texture == 0))
        {
            if (d->movie_texture != 0)
            {
                texture_del(d->movie_texture);
            }
            if (d->movie_fbo)
                delete d->movie_fbo;

            QOpenGLFramebufferObjectFormat format;
            format.setMipmap(true);
            d->movie_fbo = new QOpenGLFramebufferObject(
                        d->movie_size, format);

            d->movie_texture = texture_gen(d->movie_size,
                                             PIXEL_RGB, DATA_RGB,
                                             DATA_UNSIGNED_BYTE,
                                             d->movie_image);
              texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
              texture_filter(FILTER_LINEAR, FILTER_LINEAR);
              d->movie_last_size = d->movie_size;
        }
        else
        {
            texture_sub (d->movie_texture, d->movie_size,
                         DATA_RGB, DATA_UNSIGNED_BYTE,
                         d->movie_image);

            d->movie_fbo->bind();
            glPushAttrib(GL_VIEWPORT_BIT);
            viewport(QRect(0, 0,
                           d->movie_fbo->size().width(),
                           d->movie_fbo->size().height()));

            matrix_mode(MATRIX_PROJECTION);
            matrix_push();
            load_identity();
            QMatrix4x4 ortho;
            ortho.ortho(0, 1, 0, 1, 0, 1);
            load_matrix(ortho);
            matrix_mode(MATRIX_MODELVIEW);
            matrix_push();
            load_identity();

            //! 进行鼠标和图像叠加
            activate_texture(MULTI_TEXTURE_0);
            enable (ENABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, d->movie_texture);

            polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_FILL);
            glColor3f (1, 1, 1);
            glBegin (GL_QUADS);
            multi_texCoord_2f(MULTI_TEXTURE_0, 0,0);
            glVertex2f(0,1);
            multi_texCoord_2f(MULTI_TEXTURE_0, 0,1);
            glVertex2f(0,0);
            multi_texCoord_2f(MULTI_TEXTURE_0, 1,1);
            glVertex2f(1,0);
            multi_texCoord_2f(MULTI_TEXTURE_0, 1,0);
            glVertex2f(1,1);
            glEnd();

            activate_texture(MULTI_TEXTURE_0);
            disable (DISABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, 0);

            ///////////////////////
            /*enable (ENABLE_ALPHA_TEST);
            activate_texture(MULTI_TEXTURE_1);
            enable (ENABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, d->watermark_texture);

            glBegin (GL_QUADS);
            multi_texCoord_2f(MULTI_TEXTURE_1, 0,0);
            glVertex2f(0,0);
            multi_texCoord_2f(MULTI_TEXTURE_1, 0,1);
            glVertex2f(0,1);
            multi_texCoord_2f(MULTI_TEXTURE_1, 1,1);
            glVertex2f(1,1);
            multi_texCoord_2f(MULTI_TEXTURE_1, 1,0);
            glVertex2f(1,0);
            glEnd();

            activate_texture(MULTI_TEXTURE_1);
            disable (DISABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, 0);*/
            ///////////////////////////

            matrix_mode(MATRIX_PROJECTION);
            matrix_pop();
            matrix_mode(MATRIX_MODELVIEW);
            matrix_pop();

            glPopAttrib();
            d->movie_fbo->release();
            d->handle_image->media_texture(d->movie_fbo->texture());
        }
        d->has_movie_image = false;
    }
    else if (d->has_desktop_image && d->display_mode & Rendering_Desktop)
    {
        if (d->desktop_fbo &&
                (d->desktop_fbo->size() != d->desktop_image.size()))
        {
            delete d->desktop_fbo;
            d->desktop_fbo = NULL;
        }
        if (!d->desktop_fbo && !d->desktop_image.size().isEmpty())
        {
            QOpenGLFramebufferObjectFormat format;
            format.setMipmap(true);
            d->desktop_fbo = new QOpenGLFramebufferObject(
                        d->desktop_image.size(), format);
            d->desktop_texture = texture_gen(d->desktop_image.size(),
                                             PIXEL_RGBA, DATA_BGRA,
                                             DATA_UNSIGNED_BYTE,
                                             d->desktop_image.bits());
              texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
              texture_filter(FILTER_LINEAR, FILTER_LINEAR);

        }
        if (!d->cursor_mouse.size().isEmpty() && !d->desktop_mouse)
        {
            d->desktop_mouse = texture_gen(d->cursor_mouse.size(), PIXEL_RGBA,
                                           DATA_RGBA, DATA_UNSIGNED_BYTE,
                                           d->cursor_mouse.bits());
            texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
            texture_filter(FILTER_LINEAR, FILTER_LINEAR);
        }
        if (d->desktop_fbo && d->desktop_texture)
        {
            texture_sub (d->desktop_texture, d->desktop_image.size(),
                         DATA_BGRA, DATA_UNSIGNED_BYTE,
                         d->desktop_image.bits ());
            d->desktop_fbo->bind();
            glPushAttrib(GL_VIEWPORT_BIT);
            viewport(QRect(0, 0,
                           d->desktop_fbo->size().width(),
                           d->desktop_fbo->size().height()));

            matrix_mode(MATRIX_PROJECTION);
            matrix_push();
            load_identity();
            QMatrix4x4 ortho;
            ortho.ortho(0, 1, 0, 1, 0, 1);
            load_matrix(ortho);
            matrix_mode(MATRIX_MODELVIEW);
            matrix_push();
            load_identity();

            //! 进行鼠标和图像叠加
            activate_texture(MULTI_TEXTURE_0);
            enable (ENABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, d->desktop_texture);

            polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_FILL);
            glColor3f (1, 1, 1);
            glBegin (GL_QUADS);
            multi_texCoord_2f(MULTI_TEXTURE_0, 0,0);
            glVertex2f(0,1);
            multi_texCoord_2f(MULTI_TEXTURE_0, 0,1);
            glVertex2f(0,0);
            multi_texCoord_2f(MULTI_TEXTURE_0, 1,1);
            glVertex2f(1,0);
            multi_texCoord_2f(MULTI_TEXTURE_0, 1,0);
            glVertex2f(1,1);
            glEnd();

            activate_texture(MULTI_TEXTURE_0);
            disable (DISABLE_TEXTURE_2D);
            bind_texture (BIND_TEXTURE_2D, 0);

            if (raw_config().system_setup & (1<<2))
            {
                enable (ENABLE_ALPHA_TEST);
                activate_texture(MULTI_TEXTURE_1);
                enable (ENABLE_TEXTURE_2D);
                bind_texture (BIND_TEXTURE_2D, d->desktop_mouse);

                glBegin (GL_QUADS);
                multi_texCoord_2f(MULTI_TEXTURE_1, 0,0);
                glVertex2f(d->desktop_pos.x()-0.01,
                           d->desktop_pos.y()-0.03);
                multi_texCoord_2f(MULTI_TEXTURE_1, 0,1);
                glVertex2f(d->desktop_pos.x()-0.01,
                           d->desktop_pos.y());
                multi_texCoord_2f(MULTI_TEXTURE_1, 1,1);
                glVertex2f(d->desktop_pos.x()+0.02,
                           d->desktop_pos.y());
                multi_texCoord_2f(MULTI_TEXTURE_1, 1,0);
                glVertex2f(d->desktop_pos.x()+0.02,
                           d->desktop_pos.y()-0.03);
                glEnd();

                activate_texture(MULTI_TEXTURE_1);
                disable (DISABLE_TEXTURE_2D);
                bind_texture (BIND_TEXTURE_2D, 0);
            }

            matrix_mode(MATRIX_PROJECTION);
            matrix_pop();
            matrix_mode(MATRIX_MODELVIEW);
            matrix_pop();

            glPopAttrib();
            d->desktop_fbo->release();
            d->handle_image->media_texture(d->desktop_fbo->texture());
        }

        d->has_desktop_image = false;
    }

    if (d->has_blend_update)
    {
        int row = idHigh(d->blend_update.device_id);
        int col = idLow(d->blend_update.device_id);

        if (d->blend_data.size() != 0)
        {
            d->blend_data[row][col] = d->blend_update;
        }

        if (d->blend_update.edge & Blend_Left)
        {
            d->handle_image->blend_texture (d->blend_update.device_id,
                                d->blend_update.left.brightness,
                                d->blend_update.left.gamma ,
                                d->blend_update.left.exponent,
                                Blend_Left);
        }
        if (d->blend_update.edge & Blend_Right)
        {
            d->handle_image->blend_texture (d->blend_update.device_id,
                                d->blend_update.right.brightness,
                                d->blend_update.right.gamma ,
                                d->blend_update.right.exponent,
                                Blend_Right);
        }
        if (d->blend_update.edge & Blend_Top)
        {
            d->handle_image->blend_texture (d->blend_update.device_id,
                                d->blend_update.top.brightness,
                                d->blend_update.top.gamma ,
                                d->blend_update.top.exponent,
                                Blend_Top);
        }
        if (d->blend_update.edge & Blend_Bottom)
        {
            d->handle_image->blend_texture (d->blend_update.device_id,
                                d->blend_update.bottom.brightness,
                                d->blend_update.bottom.gamma ,
                                d->blend_update.bottom.exponent,
                                Blend_Bottom);
        }
        d->has_blend_update = false;
    }

    if (d->has_image_file)
    {
        if (d->image_file.isEmpty())
            if (raw_config().media_image_mode == 2)
                d->image_file = raw_config().media_image_other_select;
            else
                d->image_file = raw_config().media_image_local_select;

        d->handle_image->image_texture(d->image_file);
        d->has_image_file = false;
    }

    if (d->has_key_move)
    {
        d->render->key_move (d->key_device_id, d->key_move_index, d->key_move);
        d->has_key_move = false;
    }
    if (d->has_select_begin)
    {
        d->render->select_begin (d->select_point);
        d->has_select_begin = false;
    }
    if (d->has_select_move)
    {
        d->render->select_move (d->select_point);
        d->has_select_move = false;
    }
    if (d->has_select_end)
    {
        d->render->select_end ();
        d->has_select_end = false;
    }

    d->call_mutex.unlock();
    return ret;
}

void opengl_viewer::display_mode(const uint val)
{
    d->call_mutex.lock();
    d->display_mode = val;
    if (val & Rendering_Grid)
    {
        if (d->loop_thread->dmovie->isPlaying() &&
                !d->loop_thread->dmovie->isPaused())
        {
            d->loop_thread->has_movie = false;
            d->loop_thread->dmovie->setPaused(true);
        }
        if (d->loop_thread->has_desktop)
        {
            d->loop_thread->has_desktop = false;
            d->loop_thread->end(true);
        }

        d->render->mode_draw () = DRAW_POINT_AND_LINE;
        d->render->show_texture() = false;
    }
    if (val & Rendering_Image)
    {
        if (d->loop_thread->dmovie->isPlaying() &&
                !d->loop_thread->dmovie->isPaused())
        {
            d->loop_thread->has_movie = false;
            d->loop_thread->dmovie->setPaused(true);
        }
        if (d->loop_thread->has_desktop)
        {
            d->loop_thread->has_desktop = false;
            d->loop_thread->end(true);
        }
        d->render->mode_draw () = DRAW_FILL;
        d->handle_image->render_type () = INPUT_IMAGE;
        d->render->show_texture() = true;
    }
    if (val & Rendering_Media)
    {
        if (!d->loop_thread->dmovie->isPlaying() ||
                !d->loop_thread->dmovie->isPaused())
        {
            d->loop_thread->has_movie = true;
            d->loop_thread->dmovie->setPaused(false);
        }
        if (d->loop_thread->has_desktop)
        {
            d->loop_thread->has_desktop = false;
            d->loop_thread->end(true);
        }

        d->render->mode_draw () = DRAW_FILL;
        d->handle_image->render_type () = INPUT_MEDIA;
        d->render->show_texture() = true;
    }
    if (val & Rendering_Desktop)
    {
        if (d->loop_thread->dmovie->isPlaying() &&
                !d->loop_thread->dmovie->isPaused())
        {
            d->loop_thread->has_movie = false;
            d->loop_thread->dmovie->setPaused(true);
        }
        if (!d->loop_thread->has_desktop)
        {
            d->loop_thread->has_desktop = true;
            d->loop_thread->begin(raw_config().desktop_frame_rate);
        }

        d->render->mode_draw () = DRAW_FILL;
        d->handle_image->render_type () = INPUT_MEDIA;
        d->render->show_texture() = true;
    }
    if (val & Rendering_Blend && !(val & Rendering_Grid))
    {
        d->render->mode_draw () = (DrawMode)
                                  (d->render->mode_draw () | DRAW_FILL);

        if (val & Rendering_Image)
            d->handle_image->render_type () = INPUT_IMAGE_BLEND;
        else
            d->handle_image->render_type () = INPUT_MEDIA_BLEND;
        d->render->show_texture() = true;
    }
    if (val & Rendering_Mesh&& !(val & Rendering_Grid))
    {
        d->render->mode_draw () = (DrawMode)(d->render->mode_draw () | 6);
        d->render->show_texture() = true;
    }
    if (val & Rendering_Point&& !(val & Rendering_Grid))
    {
        d->render->mode_draw () = (DrawMode)(d->render->mode_draw () | DRAW_POINT);
        d->render->show_texture() = true;
    }
    d->call_mutex.unlock();
    this->update();
}

static void _reset_coord_(const QRectF &rect, fbo_geomtry &geom)
{
    geom._coord[0][0] = QVector3D(rect.x (), rect.y (), 0);
    geom._coord[0][1] = QVector3D(rect.x (), rect.y (), 0);
    geom._coord[0][2] = QVector3D(rect.x ()+rect.width (),
                                   rect.y (), 0);
    geom._coord[0][3] = QVector3D(rect.x ()+rect.width (),
                                   rect.y (), 0);

    geom._coord[1][0] = QVector3D(rect.x (), rect.y (), 0);
    geom._coord[1][1] = QVector3D(rect.x (), rect.y (), 0);
    geom._coord[1][2] = QVector3D(rect.x ()+rect.width (),
                                   rect.y (), 0);
    geom._coord[1][3] = QVector3D(rect.x ()+rect.width (),
                                   rect.y (), 0);

    geom._coord[2][0] = QVector3D(rect.x (), rect.y ()+
                                   rect.height (), 0);
    geom._coord[2][1] = QVector3D(rect.x (), rect.y ()+
                                   rect.height (), 0);
    geom._coord[2][2] = QVector3D(rect.x ()+rect.width (),
                                   rect.y ()+ rect.height (), 0);
    geom._coord[2][3] = QVector3D(rect.x ()+rect.width (),
                                   rect.y ()+ rect.height (), 0);

    geom._coord[3][0] = QVector3D(rect.x (), rect.y ()+
                                   rect.height (), 0);
    geom._coord[3][1] = QVector3D(rect.x (), rect.y ()+
                                   rect.height (), 0);
    geom._coord[3][2] = QVector3D(rect.x ()+rect.width (),
                                   rect.y ()+ rect.height (), 0);
    geom._coord[3][3] = QVector3D(rect.x ()+rect.width (),
                                   rect.y ()+ rect.height (), 0);

    geom._vertex[0][0] = QVector3D(0, 0, 0);
    geom._vertex[0][1] = QVector3D(0, 0, 0);
    geom._vertex[0][2] = QVector3D(1, 0, 0);
    geom._vertex[0][3] = QVector3D(1, 0, 0);

    geom._vertex[1][0] = QVector3D(0, 0, 0);
    geom._vertex[1][1] = QVector3D(0, 0, 0);
    geom._vertex[1][2] = QVector3D(1, 0, 0);
    geom._vertex[1][3] = QVector3D(1, 0, 0);

    geom._vertex[2][0] = QVector3D(0, 1, 0);
    geom._vertex[2][1] = QVector3D(0, 1, 0);
    geom._vertex[2][2] = QVector3D(1, 1, 0);
    geom._vertex[2][3] = QVector3D(1, 1, 0);

    geom._vertex[3][0] = QVector3D(0, 1, 0);
    geom._vertex[3][1] = QVector3D(0, 1, 0);
    geom._vertex[3][2] = QVector3D(1, 1, 0);
    geom._vertex[3][3] = QVector3D(1, 1, 0);
}
//原先没有a
void opengl_viewer::computer_coord_update(const QVector<QVector<QRectF> > &coord,
                                          const QVector<QVector<QRectF> > &p)
{
    QVector<QVector<BlendEdge> > edge =
            d->intersect (coord);
    fbo_geomtry ge;

    for (int i = 0; i < edge.size (); i++)
    {
        for (int j = 0; j < edge[i].size (); j++)
        {
            QRectF temp = coord[i][j];
            uint ids = idTools(i, j);

            if (!d->last_fbo_data)
            {
                _reset_coord_(temp, ge);
                d->handle_image->geomtry_set(ids, ge);
            }

            float fatc_w = 1.0 / temp.width ();
            float fatc_h = 1.0 / temp.height ();

            if ((edge[i][j].dir &Blend_Left) != 0)
            {
                float *area_ = edge[i][j].edge[0].area;
                QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);

                d->handle_image->geomtry(ids)._coord[0][0].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[0][1].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[0][1].setX
                        (d->handle_image->geomtry(ids)._vertex[0][0].x () +
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[1][0].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[1][1].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[1][1].setX
                        (d->handle_image->geomtry(ids)._vertex[1][0].x () +
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[2][0].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[2][1].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[2][1].setX
                        (d->handle_image->geomtry(ids)._vertex[2][0].x () +
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[3][0].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[3][1].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[3][1].setX
                        (d->handle_image->geomtry(ids)._vertex[3][0].x () +
                        (lefts.width () * fatc_w));

                d->render->coord_update (ids, Blend_Left,
                        d->handle_image->geomtry(ids)._vertex[0][0].toVector2D (),
                        d->handle_image->geomtry(ids)._vertex[3][1].toVector2D ());
            }

            if ((edge[i][j].dir &Blend_Right) != 0)
            {
                float *area_ = edge[i][j].edge[1].area;
                QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                d->handle_image->geomtry(ids)._coord[0][2].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[0][3].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[0][2].setX
                        (d->handle_image->geomtry(ids)._vertex[0][3].x () -
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[1][2].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[1][3].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[1][2].setX
                        (d->handle_image->geomtry(ids)._vertex[1][3].x () -
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[2][2].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[2][3].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[2][2].setX
                        (d->handle_image->geomtry(ids)._vertex[2][3].x () -
                        (lefts.width () * fatc_w));

                d->handle_image->geomtry(ids)._coord[3][2].setX (lefts.x ());
                d->handle_image->geomtry(ids)._coord[3][3].setX
                        (lefts.x ()+lefts.width ());
                d->handle_image->geomtry(ids)._vertex[3][2].setX
                        (d->handle_image->geomtry(ids)._vertex[3][3].x () -
                        (lefts.width () * fatc_w));

                d->render->coord_update (ids, Blend_Right,
                                      d->handle_image->geomtry(ids).
                                      _vertex[0][2].toVector2D (),
                        d->handle_image->geomtry(ids)._vertex[3][3].toVector2D ());
            }

            if ((edge[i][j].dir &Blend_Top) != 0)
            {
                float *area_ = edge[i][j].edge[2].area;
                QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                d->handle_image->geomtry(ids)._coord[0][0].setY (lefts.y ());
                d->handle_image->geomtry(ids)._coord[0][1].setY (lefts.y ());

                d->handle_image->geomtry(ids)._coord[0][2].setY (lefts.y ());
                d->handle_image->geomtry(ids)._coord[0][3].setY (lefts.y ());

                d->handle_image->geomtry(ids)._coord[1][0].setY
                        (lefts.y ()+lefts.height ());
                d->handle_image->geomtry(ids)._vertex[1][0].setY
                        (d->handle_image->geomtry(ids)._vertex[0][0].y ()+
                        (lefts.height () *fatc_h));
                d->handle_image->geomtry(ids)._coord[1][1].setY
                        (lefts.y ()+lefts.height ());
                d->handle_image->geomtry(ids)._vertex[1][1].setY
                        (d->handle_image->geomtry(ids)._vertex[0][1].y ()+
                        (lefts.height () *fatc_h));

                d->handle_image->geomtry(ids)._coord[1][2].setY
                        (lefts.y ()+lefts.height ());
                d->handle_image->geomtry(ids)._vertex[1][2].setY
                        (d->handle_image->geomtry(ids)._vertex[0][2].y ()+
                        (lefts.height () *fatc_h));
                d->handle_image->geomtry(ids)._coord[1][3].setY
                        (lefts.y ()+lefts.height ());
                d->handle_image->geomtry(ids)._vertex[1][3].setY
                        (d->handle_image->geomtry(ids)._vertex[0][3].y ()+
                        (lefts.height () *fatc_h));

                d->render->coord_update (ids, Blend_Bottom,
                                      d->handle_image->geomtry(ids).
                                      _vertex[0][0].toVector2D (),
                        d->handle_image->geomtry(ids)._vertex[1][3].toVector2D ());
            }

            if ((edge[i][j].dir &Blend_Bottom) != 0)
            {
                float *area_ = edge[i][j].edge[3].area;
                QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                d->handle_image->geomtry(ids)._coord[3][0].setY
                        (lefts.y ()+ lefts.height ());
                d->handle_image->geomtry(ids)._coord[3][1].setY
                        (lefts.y ()+ lefts.height ());

                d->handle_image->geomtry(ids)._coord[3][2].setY
                        (lefts.y ()+ lefts.height ());
                d->handle_image->geomtry(ids)._coord[3][3].setY
                        (lefts.y ()+ lefts.height ());

                d->handle_image->geomtry(ids)._coord[2][0].setY (lefts.y ());
                d->handle_image->geomtry(ids)._vertex[2][0].setY
                        (d->handle_image->geomtry(ids)._vertex[3][0].y () -
                        (lefts.height ()*fatc_h));
                d->handle_image->geomtry(ids)._coord[2][1].setY (lefts.y ());
                d->handle_image->geomtry(ids)._vertex[2][1].setY
                        (d->handle_image->geomtry(ids)._vertex[3][1].y () -
                        (lefts.height ()*fatc_h));

                d->handle_image->geomtry(ids)._coord[2][2].setY (lefts.y ());
                d->handle_image->geomtry(ids)._vertex[2][2].setY
                        (d->handle_image->geomtry(ids)._vertex[3][2].y () -
                        (lefts.height ()*fatc_h));
                d->handle_image->geomtry(ids)._coord[2][3].setY (lefts.y ());
                d->handle_image->geomtry(ids)._vertex[2][3].setY
                        (d->handle_image->geomtry(ids)._vertex[3][3].y () -
                        (lefts.height ()*fatc_h));

                d->render->coord_update (ids, Blend_Top,
                                      d->handle_image->geomtry(ids).
                                      _vertex[2][0].toVector2D (),
                        d->handle_image->geomtry(ids)._vertex[3][3].toVector2D ());
            }
        }
    }
    if (!p.isEmpty())
    {
        device_coord_update(p);
    }

    d->last_fbo_data = true;
    this->update();
}

//原先是a
void  opengl_viewer::device_coord_update(const QVector<QVector<QRectF> > &coord)
{
     d->last_fbo_data = false;
    QVector<QVector<BlendEdge> > edge = d->intersect (coord);

    //当前矩形的混合值
    EdgeFusion edd ;

    BlendEdge temps_ = edge[d->render->param()._pc_count_row_c]
            [d->render->param()._pc_count_col_c];
    edd.coord =
            QRectF(temps_.coord[0], temps_.coord[1],
            temps_.coord[2], temps_.coord[3]);
    edd.dir = (FusionDir)temps_.dir;
    edd.id = temps_.device_id;
    for (int i = 0; i < 4; i++)
    {
        edd.edge[i].area = QRectF(temps_.edge[i].area[0], temps_.edge[i].area[1],
                temps_.edge[i].area[2], temps_.edge[i].area[3]);
        edd.edge[i].id = temps_.edge[i].device_id;
        edd.edge[i].ratio = QSizeF(temps_.edge[i].ratio[0],
                temps_.edge[i].ratio[1]);
    }

    //计算当前计算机的混合方向
    int dirs = d->computer_blend(raw_config());

    //计算水平所有的垂直拼接
    for (int j = 0; j < d->render->param()._p_count_row; j++)
    {
        for (int i = 0; i < d->render->param()._p_count_col; i++)
        {
            uint ids = (j << 8) & 0xff00;
            ids |= i;

            float fatc_w = 1.0 / coord[d->render->param()._pc_count_row_c]
                    [d->render->param()._pc_count_col_c].width ();
            float fatc_h = 1.0 / coord[d->render->param()._pc_count_row_c]
                    [d->render->param()._pc_count_col_c].height ();

            if (((dirs &Blend_Left) != 0) && (i == 0))
                if ((edd.dir &Blend_Left) != 0)
                {
                    float *area_ = edge[i][j].edge[0].area;
                    QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);

                    d->handle_image->geomtry(ids)._coord[0][0].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[0][1].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[0][1].setX
                            (d->handle_image->geomtry(ids)._vertex[0][0].x () +
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[1][0].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[1][1].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[1][1].setX
                            (d->handle_image->geomtry(ids)._vertex[1][0].x () +
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[2][0].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[2][1].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[2][1].setX
                            (d->handle_image->geomtry(ids)._vertex[2][0].x () +
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[3][0].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[3][1].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[3][1].setX
                            (d->handle_image->geomtry(ids)._vertex[3][0].x () +
                            (lefts.width () * fatc_w));

                    d->render->coord_update (ids, Blend_Left,
                                          d->handle_image->geomtry(ids)
                                          ._vertex[0][0].toVector2D (),
                            d->handle_image->geomtry(ids)._vertex[3][1].toVector2D ());
                }

            if (((dirs &Blend_Right) != 0) &&
                    (i == (d->render->param()._p_count_col-1)))
                if ((edd.dir &Blend_Right) != 0)
                {
                    float *area_ = edge[i][j].edge[1].area;
                    QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                    d->handle_image->geomtry(ids)._coord[0][2].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[0][3].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[0][2].setX
                            (d->handle_image->geomtry(ids)._vertex[0][3].x () -
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[1][2].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[1][3].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[1][2].setX
                            (d->handle_image->geomtry(ids)._vertex[1][3].x () -
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[2][2].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[2][3].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[2][2].setX
                            (d->handle_image->geomtry(ids)._vertex[2][3].x () -
                            (lefts.width () * fatc_w));

                    d->handle_image->geomtry(ids)._coord[3][2].setX (lefts.x ());
                    d->handle_image->geomtry(ids)._coord[3][3].setX
                            (lefts.x ()+lefts.width ());
                    d->handle_image->geomtry(ids)._vertex[3][2].setX
                            (d->handle_image->geomtry(ids)._vertex[3][3].x () -
                            (lefts.width () * fatc_w));

                    d->render->coord_update (ids, Blend_Right,
                                          d->handle_image->geomtry(ids)
                                          ._vertex[0][2].toVector2D (),
                            d->handle_image->geomtry(ids)._vertex[3][3].toVector2D ());
                }

            if (((dirs &Blend_Bottom) != 0) && (j == 0))
                if ((edd.dir &Blend_Top) != 0)
                {
                    float *area_ = edge[i][j].edge[2].area;
                    QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                    d->handle_image->geomtry(ids)._coord[0][0].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._coord[0][1].setY (lefts.y ());

                    d->handle_image->geomtry(ids)._coord[0][2].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._coord[0][3].setY (lefts.y ());

                    d->handle_image->geomtry(ids)._coord[1][0].setY
                            (lefts.y ()+lefts.height ());

                    d->handle_image->geomtry(ids)._vertex[1][0].setY
                            (d->handle_image->geomtry(ids)._vertex[0][0].y ()+
                            (lefts.height () *fatc_h));
                    d->handle_image->geomtry(ids)._coord[1][1].setY
                            (lefts.y ()+lefts.height ());
                    d->handle_image->geomtry(ids)._vertex[1][1].setY
                            (d->handle_image->geomtry(ids)._vertex[0][1].y ()+
                            (lefts.height () *fatc_h));

                    d->handle_image->geomtry(ids)._coord[1][2].setY
                            (lefts.y ()+lefts.height ());
                    d->handle_image->geomtry(ids)._vertex[1][2].setY
                            (d->handle_image->geomtry(ids)._vertex[0][2].y ()+
                            (lefts.height () *fatc_h));
                    d->handle_image->geomtry(ids)._coord[1][3].setY
                            (lefts.y ()+lefts.height ());
                    d->handle_image->geomtry(ids)._vertex[1][3].setY
                            (d->handle_image->geomtry(ids)._vertex[0][3].y ()+
                            (lefts.height () *fatc_h));

                    d->render->coord_update (ids, Blend_Bottom,
                                          d->handle_image->geomtry(ids).
                                          _vertex[0][0].toVector2D (),
                            d->handle_image->geomtry(ids)._vertex[1][3].toVector2D ());
                }

            if (((dirs &Blend_Top) != 0) &&
                    (j == (d->render->param()._p_count_row-1)))
                if ((edd.dir &Blend_Bottom) != 0)
                {
                    float *area_ = edge[i][j].edge[3].area;
                    QRectF lefts = QRectF(area_[0], area_[1], area_[2], area_[3]);
                    d->handle_image->geomtry(ids)._coord[3][0].setY
                            (lefts.y ()+ lefts.height ());
                    d->handle_image->geomtry(ids)._coord[3][1].setY
                            (lefts.y ()+ lefts.height ());

                    d->handle_image->geomtry(ids)._coord[3][2].setY
                            (lefts.y ()+ lefts.height ());
                    d->handle_image->geomtry(ids)._coord[3][3].setY
                            (lefts.y ()+ lefts.height ());

                    d->handle_image->geomtry(ids)._coord[2][0].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._vertex[2][0].setY
                            (d->handle_image->geomtry(ids)._vertex[3][0].y () -
                            (lefts.height ()*fatc_h));
                    d->handle_image->geomtry(ids)._coord[2][1].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._vertex[2][1].setY
                            (d->handle_image->geomtry(ids)._vertex[3][1].y () -
                            (lefts.height ()*fatc_h));

                    d->handle_image->geomtry(ids)._coord[2][2].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._vertex[2][2].setY
                            (d->handle_image->geomtry(ids)._vertex[3][2].y () -
                            (lefts.height ()*fatc_h));
                    d->handle_image->geomtry(ids)._coord[2][3].setY (lefts.y ());
                    d->handle_image->geomtry(ids)._vertex[2][3].setY
                            (d->handle_image->geomtry(ids)._vertex[3][3].y () -
                            (lefts.height ()*fatc_h));

                    d->render->coord_update (ids, Blend_Top,
                                          d->handle_image->geomtry(ids).
                                          _vertex[2][0].toVector2D (),
                            d->handle_image->geomtry(ids)._vertex[3][3].toVector2D ());
                }
        }
    }
}
