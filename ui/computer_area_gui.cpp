#include "computer_area_gui.h"
#include "ui_define.h"
#include <QEvent>
#include <QVector>
#include <QRectF>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>

const int SPS = 20;
class computer_area_gui_p
{
public:
    computer_area_gui_p():
        bg_image(),
        current_rect(),
        back_point(),
        rect_data(),
        coord_data()
    {
        back_select_v = -1;
        back_select_h = -1;
        fusion_dir = 0;
        computer_h = 0;
        computer_v = 0;
        widget_size = QSize(320-SPS, 260-SPS);//画布大小
    }
    void mousePress(QMouseEvent * e, QWidget *draw)
    {
        for (int i = 0; i < rect_data.size (); i++)
        {
            for (int j = 0; j < rect_data[i].size (); j++)
            {
               if (rect_data[i][j].contains(e->pos ()))
               {
                   fusion_dir = 0;
                   if (!move_rect(e->pos (), rect_data[i][j], draw))
                   {
                       //draw->setCursor(Qt::SizeAllCursor);
                   }
                   else
                   {
                       back_select_v = i;
                       back_select_h = j;
                       back_point = e->pos ();
                       draw->update ();
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
        current_rect = QRectF(0, 0, 0, 0);
        draw->setCursor(Qt::ArrowCursor);
        draw->update ();
    }
    void mouseDoubleClick(QMouseEvent *)
    {

    }
    void mouseMove(QMouseEvent *e, QWidget *draw)
    {
        if ((back_select_v != -1) && (back_select_h != -1))
        {
            float x = 1.0f / (float)widget_size.width ();
            float y = 1.0f / (float)widget_size.height ();

            QPoint bak = back_point - e->pos ();//移动量
            QRectF tem = rect_data[back_select_v][back_select_h];
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
                    tem.setHeight (rect_data[back_select_v][back_select_h].
                                   height ()+bak.y());
                    current_rect = QRect(tem.x(), tem.y()-1, tem.width(), 2);

                    c0.setY(c2.y() -(-(tem.height()) *y));
                    c1.setY(c3.y() -(-(tem.height()) *y));
                }
                break;
                case 3:
                {
                    //c0、c1
                    tem.setHeight (rect_data[back_select_v][back_select_h].
                                   height ()-bak.y());
                    current_rect = QRect(tem.x(),tem.y()+tem.height()-1, tem.width(), 2);

                    c2.setY(c0.y() + (-(tem.height()) *y));
                    c3.setY(c1.y() + (-(tem.height()) *y));

                }
                break;
                case 2:
                {
                    //c0、c2
                    tem.setX (xs);
                    tem.setWidth (rect_data[back_select_v][back_select_h].
                                  width ()+bak.x ());
                    current_rect = QRect(tem.x()-1,tem.y(), 2, tem.height());

                    c0.setX(c1.x() -(tem.width() *x));
                    c2.setX(c3.x() -(tem.width() *x));
                }
                break;
                case 4:
                {
                    //c1、c3
                    tem.setWidth (rect_data[back_select_v][back_select_h].
                                  width ()-bak.x ());
                    current_rect = QRect(tem.x()+tem.width()-1,tem.y(), 2, tem.height());

                    c1.setX(c0.x() +(tem.width() *x));
                    c3.setX(c2.x() +(tem.width() *x));
                }
                break;
                default:
                break;
            }

            tempc = QRectF(c0.x(), c0.y(), c3.x() - c0.x(), c3.y() - c0.y());

            QRect tolRect = QRect(SPS/2-1, SPS/2-1,
                                  widget_size.width ()+3,
                                  widget_size.height ()+3);

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
                    rect_data[back_select_v][back_select_h] = tem;
                    coord_data[back_select_v][back_select_h] = tempc;
                }
            }
            else
            {
                back_point = e->pos ();
                draw->update ();
                return;
            }

            //! 通知修改数据
            ((computer_area_gui*)draw)->callback();
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
        if (!computer_h && !computer_v)
            return ;

        QPainter pai;
        pai.begin (draw);

        //绘制背景图
        if (!bg_image.isNull())
            pai.drawImage(QRect(SPS/2, SPS/2,
                                widget_size.width (), widget_size.height ()),
                          bg_image);

        //绘边界框
        pai.setPen(QPen(Qt::darkMagenta, 1, Qt::DashDotLine)); //设置画笔形式
        pai.setBrush(QBrush(Qt::red, Qt::NoBrush)); //设置画刷形式
        pai.drawRect (QRect(SPS/2, SPS/2,
                            widget_size.width (),widget_size.height ()));

        //绘制通道指示边界框
        int tw = widget_size.width () / computer_h;
        int th = widget_size.height () / computer_v;
        for (int i = 0; i < computer_v; i++)
        {
            for (int j = 0; j < computer_h; j++)
            {
                QRect tem ;
                tem = QRect(SPS/2+(j*tw), SPS/2+(i*th), tw, th);
                pai.setPen(QPen(Qt::red,1,Qt::DashDotDotLine)); //设置画笔形式
                pai.setBrush(QBrush(Qt::red,Qt::NoBrush)); //设置画刷形式
                pai.drawRect (tem);
            }
        }

        //绘制通道裁剪边界框
        pai.setPen(QPen(Qt::red,2,Qt::DotLine)); //设置画笔形式
        pai.setBrush(QBrush(Qt::red,Qt::NoBrush)); //设置画刷形式
        pai.drawRect (current_rect);

        for (int i = 0; i < rect_data.size (); i++)
        {
            for (int j = 0; j < rect_data[i].size (); j++)
            {
                QRectF temp = rect_data[i][j];
                //QRectF tempc = coord[i][j];

                pai.setPen(QPen(Qt::red,1,Qt::SolidLine)); //设置画笔形式
                pai.setBrush(QBrush(Qt::red,Qt::NoBrush)); //设置画刷形式

                pai.drawRect (temp);
                pai.drawText (temp.center () - QPointF(10, 3),
                              QObject::tr("id:")+
                              QString::number(idTools(i, j), 16));
            }
        }
        pai.end ();
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
        computer_h = h_;
        computer_v = v_;

        float w = (float)widget_size.width () / (float)computer_h;
        float h = (float)widget_size.height () / (float)computer_v;

        float coord_w = (float)1.0 / (float)computer_h;
        float coord_h = (float)1.0 / (float)computer_v;

        if (coord_data.size()) coord_data.clear();
        if (rect_data.size()) rect_data.clear();

        coord_data.resize(computer_v);
        rect_data.resize(computer_v);
        for (int i = 0; i < computer_v; i++)
        {
            coord_data[i].resize(computer_h);
            rect_data[i].resize(computer_h);
            for (int j = 0; j < computer_h; j++)
            {
                rect_data[i][j] = QRectF(j*w+(SPS/2),
                                         widget_size.height()- (i*h-(SPS/2)),
                                         w, -h);
                coord_data[i][j] = QRectF((float)j*coord_w, (float)i*coord_h,
                                          coord_w, coord_h);
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
            fusion_dir = 1;//top
            current_rect = QRectF(ret.x (), ret.y ()-1, ret.width (), 2);
            draw->setCursor(Qt::SizeVerCursor);
            return true;
        }
        else if (newRect1.contains (pos))
        {
            fusion_dir = 2;//left
            current_rect = QRectF(ret.x ()-1, ret.y (), 2, ret.height ());
            draw->setCursor(Qt::SizeHorCursor);
            return true;
        }
        else if (newRect2.contains (pos))
        {
            fusion_dir = 3;//bottom
            current_rect = QRectF(ret.x (), (ret.y ()+ret.height ())-1
                               , ret.width (), 2);
            draw->setCursor(Qt::SizeVerCursor);
            return true;
        }
        else if (newRect3.contains (pos))
        {
            fusion_dir = 4;//right
            current_rect = QRectF((ret.x ()+ret.width ())-1, ret.y ()
                               , 2, ret.height ());
            draw->setCursor(Qt::SizeHorCursor);
            return true;
        }
        return false;
    }
public:
    int computer_h;
    int computer_v;

    QSize widget_size;
    QImage bg_image;
    QRectF current_rect;
    int fusion_dir;
    int back_select_v;
    int back_select_h;
    QPoint back_point;

    QVector<QVector<QRectF> > rect_data;
    QVector<QVector<QRectF> > coord_data;
};

computer_area_gui::computer_area_gui(QWidget *parent) :
    QWidget(parent),
    d(new computer_area_gui_p())
{

}

computer_area_gui::~computer_area_gui()
{
    if (d)
        delete d;
    d = NULL;
}

void computer_area_gui::callback()
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

    wParame << d->rect_data.size();
    for (int i = 0; i < d->rect_data.size(); ++i)
    {
        wParame << d->rect_data[i].size();
        for (int j = 0; j < d->rect_data[i].size(); ++j)
        {
            wParame << d->rect_data[i][j];
        }
    }
    wParame << d->widget_size;
    wParameEnd();

    control(Command_ComputerCut);
}
bool computer_area_gui::event(QEvent *evt)
{
    if (evt->type() == QEvent::Paint)
    {
        QSize temp(SPS, SPS);
        if (d->widget_size != (this->size()-temp))
        {
            log_information("update widget size(computer) = "+
                            QString("%1*%2").arg(d->widget_size.width()).
                            arg(d->widget_size.height()));
            d->widget_size = this->size()-temp;
            d->start_debug(d->computer_v, d->computer_h);
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
void computer_area_gui::command(const int &cmd)
{
    switch (cmd)
    {
    case Command_StartDebug:
    {
        d->start_debug(idHigh(raw_config().computer_layout),
                       idLow(raw_config().computer_layout));
        d->background(raw_config().media_image_local_select);
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
    }
}
