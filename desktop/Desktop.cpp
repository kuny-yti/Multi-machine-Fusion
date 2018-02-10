#include "Desktop.h"
#include <QGLWidget>

Desktop::Desktop():
    Screen(0)
{

}
Desktop::~Desktop()
{
    if (Screen)
        delete Screen;
}
bool Desktop::is_init()
{
    return Screen;
}
void Desktop::init()
{
    try
    {
       Screen = new ScreenClient();
       //Screen->applyNewProperties();
       Screen->executeDetection();
       dim = Screen->getScreenDimension();
    }
    catch (QString &e)
    {
        qDebug() << "QMirrorDriver mirror driver:" << e.toLocal8Bit().data();
    }
}

void Desktop::apply_new()
{
    if (Screen)
        delete Screen;

    Screen = new ScreenClient();
    Screen->executeDetection();
    dim = Screen->getScreenDimension();
    Screen->applyNewProperties();
}
QSize Desktop::size()const
{
    return QSize(dim.width, dim.height);
}
QImage Desktop::image()const
{
    Rect re = Rect(0, 0, dim.width, dim.height);
    Screen->grab(&re);

    return QImage((uchar*)Screen->getScreenBuffer(),
                  dim.width,
                  dim.height,
                  QImage::Format_ARGB32);
    /*QGLWidget::convertToGLFormat
            (QImage((uchar*)Screen->getScreenBuffer(),
                    dim.width,
                    dim.height,
                    QImage::Format_ARGB32));*/
}

