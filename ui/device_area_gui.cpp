#include "device_area_gui.h"
#include "ui_define.h"
#include <QEvent>
#include <QVector>
#include <QRectF>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

static QColor patin_color[12][12] =
{
    {
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)}
    },
    {
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)}
    },
    {
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)}
    },
    {
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)}
    },
    {
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)}
    },
    {
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)}
    },
    {
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)}
    },
    {
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)}
    },
    {
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)}
    },
    {
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)}
    },
    {
        {QColor(200, 100, 40)},
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)}
    },
    {
        {QColor(100, 100, 200)},
        {QColor(10, 10, 10)},
        {QColor(200, 0, 0)},
        {QColor(0, 200, 0)},
        {QColor(0, 0, 200)},
        {QColor(200, 200, 0)},
        {QColor(200, 0, 200)},
        {QColor(0, 200, 200)},
        {QColor(200, 40, 40)},
        {QColor(100, 40, 200)},
        {QColor(40, 100, 200)},
        {QColor(200, 100, 40)}
    }
};

const int SP = 20;
class device_area_gui_p
{
public:
    device_area_gui_p():
        bg_image(),
        current_point1(),
        current_point2(),
        back_point(),
        channel_data(),
        coord_data(),
        coord_computer()
    {
        back_select_v = -1;
        back_select_h = -1;
        fusion_dir = 0;
        device_h = 0;
        device_v = 0;
        widget_size = QSize(320-SP, 260-SP);//画布大小
    }
    void mousePress(QMouseEvent * e, QWidget *draw)
    {
        for (int i = 0; i < channel_data.size (); i++)
        {
            for (int j = 0; j < channel_data[i].size (); j++)
            {
               if (channel_data[i][j].contains(e->pos ()))
               {
                   fusion_dir = 0;
                   if (!move_rect(e->pos (), channel_data[i][j], draw))
                   {
                       //setCursor(Qt::SizeAllCursor);
                   }
                   else
                   {
                       back_select_v = i;
                       back_select_h = j;
                       back_point = e->pos ();
                       return;
                   }
               }
            }
        }
        draw->update ();
    }
    void mouseRelease(QMouseEvent *, QWidget *draw)
    {
        back_select_h = -1;
        back_select_v = -1;
        fusion_dir = 0;
        current_point1 = QPoint(0,0);
        current_point2 = QPoint(0,0);
        draw->setCursor(Qt::ArrowCursor);
        draw->update ();
    }
    void mouseDoubleClick(QMouseEvent *)
    {

    }
    void mouseMove(QMouseEvent *e, QWidget *draw)
    {
        if ((back_select_h != -1) && (back_select_v != -1))
        {
            float x = 1.0f / (float)widget_size.width ();
            float y = 1.0f / (float)widget_size.height ();

            QPoint bak = back_point - e->pos ();//移动量
            QRectF tem = channel_data[back_select_v][back_select_h];
            QRectF tempc = coord_data[back_select_v][back_select_h];

            float xs = tem.x () - bak.x ();//移动后矩形的位置
            float ys = tem.y () - bak.y ();

            QVector2D c0 = QVector2D(tempc.x(), tempc.y());
            QVector2D c1 = QVector2D(tempc.x()+tempc.width(), tempc.y());
            QVector2D c2 = QVector2D(tempc.x(), tempc.y()+tempc.height());
            QVector2D c3 = QVector2D(tempc.x()+tempc.width(), tempc.y()+tempc.height());

            switch (fusion_dir)
            {
                case 0:
                {
                    /*tem.setX (xs);
                    tem.setY (ys);
                    tem.setWidth (channel[a][b].width ());
                    tem.setHeight (channel[a][b].height ());

                    curr_rect = tem;

                    tempc.setX (cx);
                    tempc.setY (cy);
                    tempc.setWidth (coord[a][b].width ());
                    tempc.setHeight (coord[a][b].height ());*/
                }
                break;
                case 1:
                {
                    //c2、c3
                    tem.setY (ys);
                    tem.setHeight (channel_data[back_select_v][back_select_h].
                                   height ()+bak.y());
                    current_point1 = QPoint(tem.x(), tem.y());
                    current_point2 = QPoint(tem.x()+tem.width(), tem.y());

                    c0.setY(c2.y() -(-(tem.height()) *y));
                    c1.setY(c3.y() -(-(tem.height()) *y));
                }
                break;
                case 3:
                {
                    //c0、c1
                    tem.setHeight (channel_data[back_select_v][back_select_h].
                                   height ()-bak.y());
                    current_point1 = QPoint(tem.x(), tem.y()+tem.height());
                    current_point2 = QPoint(tem.x()+tem.width(), tem.y()+tem.height());

                    c2.setY(c0.y() + (-(tem.height()) *y));
                    c3.setY(c1.y() + (-(tem.height()) *y));

                }
                break;
                case 2:
                {
                    //c0、c2
                    tem.setX (xs);
                    tem.setWidth (channel_data[back_select_v][back_select_h].
                                  width ()+bak.x ());
                    current_point1 = QPoint(tem.x(), tem.y());
                    current_point2 = QPoint(tem.x(), tem.y()+tem.height());

                    c0.setX(c1.x() -(tem.width() *x));
                    c2.setX(c3.x() -(tem.width() *x));
                }
                break;
                case 4:
                {
                    //c1、c3
                    tem.setWidth (channel_data[back_select_v][back_select_h].
                                  width ()-bak.x ());
                    current_point1 = QPoint(tem.x()+tem.width(), tem.y());
                    current_point2 = QPoint(tem.x()+tem.width(), tem.y()+tem.height());

                    c1.setX(c0.x() +(tem.width() *x));
                    c3.setX(c2.x() +(tem.width() *x));
                }
                break;
                default:
                break;
            }

            tempc = QRectF(c0.x(), c0.y(), c3.x() - c0.x(), c3.y() - c0.y());

            QRect tolRect = QRect(SP/2-1, SP/2-1,
                                  widget_size.width ()+3,widget_size.height ()+3);
            if ((tolRect.left () < tem.left ()) &&
                (tolRect.right () > tem.right ())&&
                (tolRect.top () < tem.bottom ())&&
                (tolRect.bottom () > tem.top())
                /*(tolRect.top () < tem.top ())&&
                (tolRect.bottom () > tem.bottom())*/)
            {
                if ((tem.left() >=  tem.right()) ||
                        (tem.top() <= tem.bottom())
                        /*(tem.top() >= tem.bottom())*/)
                {
                    back_point = e->pos ();
                    draw->update ();
                    return;
                }
                else
                {
                    channel_data[back_select_v][back_select_h] = tem;
                    coord_data[back_select_v][back_select_h] = tempc;
                }
            }
            else
            {
                back_point = e->pos ();
                draw->update ();
                return;
            }

            //! 通知更新数据
            ((device_area_gui*)draw)->callback(Command_DeviceCut);
        }
        back_point = e->pos ();
        draw->update ();
    }
    #ifndef QT_NO_WHEELEVENT
    void wheel(QWheelEvent *)
    {

    }
    #endif
    void keyPress(QKeyEvent *)
    {

    }
    void keyRelease(QKeyEvent *)
    {

    }

    void paint(QPaintEvent *, QWidget *draw)
    {
        if (device_v && device_h)
        {
            QPainter pai;
            pai.begin (draw);

            pai.drawImage(QRect(SP/2, SP/2,
                                widget_size.width (),widget_size.height ()),
                          bg_image);
            pai.setPen(QPen(Qt::darkMagenta,1,Qt::DashDotLine)); //设置画笔形式
            pai.setBrush(QBrush(Qt::blue,Qt::NoBrush)); //设置画刷形式
            pai.drawRect (QRect(SP/2, SP/2,
                                widget_size.width (),widget_size.height ()));

            int tw = widget_size.width () / device_h;
            int th = widget_size.height () / device_v;
            for (int i = 0; i < device_v; i++)
            {
                for (int j = 0; j < device_h; j++)
                {
                    QRect tem ;
                    tem = QRect(SP/2+(j*tw), SP/2+(i*th), tw, th);
                    pai.setPen(QPen(patin_color[i][j],1,Qt::DashDotDotLine)); //设置画笔形式
                    pai.setBrush(QBrush(patin_color[i][j],Qt::NoBrush)); //设置画刷形式
                    pai.drawRect (tem);
                }
            }
            pai.setPen(QPen(Qt::blue, 2, Qt::DotLine)); //设置画笔形式
            pai.setBrush(QBrush(Qt::blue,Qt::NoBrush)); //设置画刷形式
            pai.drawLine (current_point1, current_point2);

            for (int i = 0; i < channel_data.size (); i++)
            {
                for (int j = 0; j < channel_data[i].size (); j++)
                {
                    QRectF temp = channel_data[i][j];
                    //QRectF tempc = coord[i][j];

                    pai.setPen(QPen(patin_color[i][j], 1,Qt::SolidLine)); //设置画笔形式
                    pai.setBrush(QBrush(patin_color[i][j], Qt::NoBrush)); //设置画刷形式

                    pai.drawRect (temp);
                    pai.drawText (temp.center () - QPointF(10, 3),
                                  QObject::tr("id:")+
                                  QString::number(idTools(i, j), 16));
                        /*
                    pai.setPen(QPen(Qt::blue,1,Qt::SolidLine)); //设置画笔形式
                    pai.setBrush(QBrush(Qt::red,Qt::NoBrush)); //设置画刷形式

                    QVector2D c0 = QVector2D(tempc.x(), tempc.y());
                    QVector2D c1 = QVector2D(tempc.x()+tempc.width(), tempc.y());
                    QVector2D c2 = QVector2D(tempc.x(), tempc.y()+tempc.height());
                    QVector2D c3 = QVector2D(tempc.x()+tempc.width(), tempc.y()+tempc.height());

                    QPoint p0 = QPoint(temp.x ()+10, temp.y ()-10);
                    QPoint p1 = QPoint(temp.x ()+temp.width ()-60, temp.y ()-10);
                    QPoint p2 = QPoint(temp.x ()+10, temp.y ()+temp.height ()+10);
                    QPoint p3 = QPoint(temp.x ()+temp.width ()-60, temp.y ()+temp.height ()+10);

                    pai.drawText (p0, tr("c0:%1,%2").arg (c0.x ()).arg (c0.y ()));
                    pai.drawText (p1, tr("c1:%1,%2").arg (c1.x ()).arg (c1.y ()));
                    pai.drawText (p2, tr("c2:%1,%2").arg (c2.x ()).arg (c2.y ()));
                    pai.drawText (p3, tr("c3:%1,%2").arg (c3.x ()).arg (c3.y ()));*/
                }
            }
            pai.end ();
        }
    }
    void move(QMoveEvent *)
    {

    }
    void resize(QResizeEvent *)
    {

    }
    void close(QCloseEvent *)
    {

    }
    bool background(const QString &pathfile)
    {
        return bg_image.load(pathfile);
    }
    void start_debug(int v_ = 2, int h_ = 2)
    {
        device_h = h_;
        device_v = v_;

        float w = (float)widget_size.width () / device_h;//每通道宽度
        float h = (float)widget_size.height () / device_v;//每通道高度

        float cw = (float)1.0 / device_h;//每通道纹理坐标宽度
        float ch = (float)1.0 / device_v;//每通道纹理坐标高度

        coord_data.resize(device_v);
        channel_data.resize(device_v);
        for (int i = 0; i < device_v; i++)
        {
            coord_data[i].resize(device_h);
            channel_data[i].resize(device_h);
            for (int j = 0; j < device_h; j++)
            {
                channel_data[i][j] = QRectF(j*w+(SP/2),
                                            widget_size.height()- (i*h-(SP/2)),
                                            w, -h);
                coord_data[i][j] = QRectF((float)j*cw, (float)i*ch, cw, ch);
            }
        }

    }
    void computer_cut(QVector<QVector<QRectF> > &aa,
                      QVector<QVector<QRectF> > &bb,
                      const RawConfig &conf)
    {
        //取当前Pc机矩形裁剪位置
        coord_computer = aa;

        QRectF current_rect =
                aa[idHigh(conf.computer_index)][idLow(conf.computer_index)];
        QRectF current_rectb =
                bb[idHigh(conf.computer_index)][idLow(conf.computer_index)];

        //对当前Pc机投影位置的y值更新(一台PC机上有多个垂直混合)
        float y_h_r = current_rect.height() / (float)coord_data.size();
        float y_h_c = current_rectb.height() / (float)coord_data.size();

        for (int j = 0; j < coord_data.size(); j++)
        {
            //对当前Pc机投影位置的x值更新(一台PC机上有多个水平混合)
            float x_w_r = current_rect.width() / (float)coord_data[j].size();
            float x_w_c = current_rectb.width() / (float)coord_data[j].size();
            for (int i = 0; i < coord_data[j].size(); i++)
            {
                coord_data[j][i].setY(current_rect.y()+(j*y_h_r));
                coord_data[j][i].setHeight(y_h_r);

                channel_data[j][i].setY(current_rectb.y()+(j*y_h_c));
                channel_data[j][i].setHeight(y_h_c);

                coord_data[j][i].setX(current_rect.x()+(i*x_w_r));
                coord_data[j][i].setWidth(x_w_r);

                channel_data[j][i].setX(current_rectb.x()+(i*x_w_c));
                channel_data[j][i].setWidth(x_w_c);
            }
        }
    }
private:
    bool move_rect(const QPoint &pos,const QRectF &ret, QWidget *draw)
    {
        float w = 15;
        QRectF newRect0 = QRectF(ret.x (), ret.y ()-w/2., ret.width (), w);
        QRectF newRect1 = QRectF(ret.x ()-w/2., ret.y (), w, ret.height ());
        QRectF newRect2 = QRectF(ret.x (), (ret.y ()+ret.height ())-w/2.
                                 , ret.width (),w);
        QRectF newRect3 = QRectF((ret.x ()+ret.width ())-w/2., ret.y ()
                                 , w, ret.height ());

        if (newRect0.contains(pos))
        {
            fusion_dir = 1;
            current_point1 = QPoint(ret.x (), ret.y ());
            current_point2 = QPoint(ret.x ()+ret.width (), ret.y ());
            draw->setCursor(Qt::SizeVerCursor);
            return true;
        }
        else if (newRect1.contains (pos))
        {
            fusion_dir = 2;
            current_point1 = QPoint(ret.x (), ret.y ());
            current_point2 = QPoint(ret.x (), ret.y ()+ret.height ());
            draw->setCursor(Qt::SizeHorCursor);
            return true;
        }
        else if (newRect2.contains (pos))
        {
            fusion_dir = 3;
            current_point1 = QPoint(ret.x (), ret.y ()+ret.height ());
            current_point2 = QPoint(ret.x ()+ret.width (), ret.y ());
            draw->setCursor(Qt::SizeVerCursor);
            return true;
        }
        else if (newRect3.contains (pos))
        {
            fusion_dir = 4;
            current_point1 = QPoint(ret.x ()+ret.width (), ret.y ());
            current_point2 = QPoint(ret.x (), ret.y ()+ret.height ());
            draw->setCursor(Qt::SizeHorCursor);
            return true;
        }
        return false;
    }
public:
    int device_h;
    int device_v;

    QSize widget_size;
    QImage bg_image;
    QPoint current_point1;
    QPoint current_point2;
    int fusion_dir;

    int back_select_v;
    int back_select_h;
    QPoint back_point;

    QVector<QVector<QRectF> > channel_data;
    QVector<QVector<QRectF> > coord_data;
    QVector<QVector<QRectF> > coord_computer;
};


device_area_gui::device_area_gui(QWidget *parent) :
    QWidget(parent),
    d(new device_area_gui_p())
{

}

device_area_gui::~device_area_gui()
{
    if (d)
        delete d;
    d = NULL;
}
void device_area_gui::callback(int cmd)
{
    wParameBegin();
    wParame << d->coord_data.size();
    for (int i = 0; i < d->coord_data.size(); ++i)
    {
        wParame << d->coord_data[i].size();
        for (int j = 0; j < d->coord_data[i].size(); ++j)
        {
            wParame << d->coord_data[i][j];
        }
    }

    wParame << d->coord_computer.size();
    for (int i = 0; i < d->coord_computer.size(); ++i)
    {
        wParame << d->coord_computer[i].size();
        for (int j = 0; j < d->coord_computer[i].size(); ++j)
        {
            wParame << d->coord_computer[i][j];
        }
    }
    wParameEnd();
    control(cmd);
}

bool device_area_gui::event(QEvent *evt)
{
    if (evt->type() == QEvent::Paint)
    {
        QSize temp(SP, SP);
        if (d->widget_size != (this->size()-temp))
        {
            d->widget_size = this->size()-temp;
            log_information("update widget size(device) = "+
                            QString("%1*%2").arg(d->widget_size.width()).
                            arg(d->widget_size.height()));
            d->start_debug(d->device_v, d->device_h);
        }
        d->paint(static_cast<QPaintEvent *>(evt), this);
        evt->accept();
        return true;
    }
    else if (evt->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mEvt = static_cast<QMouseEvent *>(evt);
        if (mEvt->button() != Qt::LeftButton)
            return QWidget::event(evt);

        d->mousePress(mEvt, this);
    }
    else if (evt->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mEvt = static_cast<QMouseEvent *>(evt);
        if (mEvt->button() != Qt::LeftButton)
            return QWidget::event(evt);

        d->mouseRelease(mEvt, this);
    }
    else if (evt->type() == QEvent::MouseMove)
    {
        d->mouseMove(static_cast<QMouseEvent *>(evt), this);
    }

    return QWidget::event(evt);
}
void device_area_gui::command(const int &cmd)
{
    switch (cmd)
    {
    case Command_StartDebug:
    {
        d->start_debug(idHigh(raw_config().device_layout),
                       idLow(raw_config().device_layout));
        this->update();
    }
        break;
    case Command_SelectImage:
        d->background(raw_config().media_image_local_select);
        this->update();
        break;
    case Command_SelectOtherImage:
        d->background(raw_config().media_image_other_select);
        this->update();
        break;
    case Command_ComputerCut:
    {
        int temp = 0;
        QVector<QVector<QRectF> > coord;
        QVector<QVector<QRectF> > rect;
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

        rParame >> temp;rect.resize(temp);
        for (int i = 0; i < rect.size(); ++i)
        {
            rParame >> temp;rect[i].resize(temp);
            for (int j = 0; j < rect[i].size(); ++j)
            {
                rParame >> rect[i][j];
            }
        }
        rParame >> d->widget_size;
        rParameEnd();
        d->computer_cut(coord, rect, raw_config());
        this->update();
        callback(Command_DeviceCut);
    }
        break;
    }
}
