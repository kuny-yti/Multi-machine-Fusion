#include "ffd_multi.h"
#include "bezier_arithmetic.h"
#include "ffd_group.h"

class ffd_multi_p
{
public:
    QVector2D _min;
    QVector2D _max;
    FusionDir _dirs;

    arrayQuadsTwo _ffd_ctrl;///当前多ffd面的位置
    arrayFFDGroupTwo _ffd_surface;///当前多ffd数据

    ffd_multi_p()
    {
        _min = QVector2D(0,0);
        _max = QVector2D(1,1);

        _ffd_ctrl.clear ();
        _ffd_surface.clear ();
    }
    ~ffd_multi_p()
    {
        for(int i=0; i < _ffd_surface.size (); i++)
        {
            _ffd_surface[i].clear ();
        }
        for (int i = 0; i < _ffd_ctrl.size (); i++)
        {
            _ffd_ctrl[i].clear ();
        }
        _ffd_ctrl.clear ();
        _ffd_surface.clear ();
    }
};

QVector2D &ffd_multi::ffd_min()
{
    return _dptr->_min;
}

QVector2D &ffd_multi::ffd_max()
{
    return _dptr->_max;
}
void ffd_multi::ffd_min(const QVector2D &val)
{
    ffd_min() = val;
    coord_update();
}
void ffd_multi::ffd_max(const QVector2D &val)
{
    ffd_max() = val;
    coord_update();
}

ffd_multi::ffd_multi():
    _dptr(new ffd_multi_p)
{
    sel_point = QVector3D();
}
ffd_multi::~ffd_multi()
{
    delete _dptr;
}

void ffd_multi::coord_update()
{
    float cw = ffd_max().x () - ffd_min().x ();//当前总宽度
    float ch = ffd_max().y () - ffd_min().y ();

    for(int i=0; i < _dptr->_ffd_ctrl.size (); i++)///计算每块的网格点
    {
        for(int j=0; j < _dptr->_ffd_ctrl[i].size (); j++)
        {
            float x = cw / (float)_dptr->_ffd_ctrl[i].size ();
            float y = ch / (float)_dptr->_ffd_ctrl.size ();
            QVector2D c0 = QVector2D(ffd_min().x () +((float)j*x)        , ffd_min().y () +((float)i*y));
            QVector2D c1 = QVector2D(ffd_min().x () +((float)(j+1.0)*x)  , ffd_min().y () +((float)(i)*y));
            QVector2D c2 = QVector2D(ffd_min().x () +((float)j*x)        , ffd_min().y () +((float)(i+1.0)*y));
            QVector2D c3 = QVector2D(ffd_min().x () +((float)(j+1.0)*x)  , ffd_min().y () +((float)(i+1.0)*y));

            //  ------------
            //  |         c3|
            //  |           |
            //  |           |
            //  |c0         |
            //  ------------
            _dptr->_ffd_ctrl[i][j].t0.p0.coord = c0;
            _dptr->_ffd_ctrl[i][j].t0.p1.coord = c1;
            _dptr->_ffd_ctrl[i][j].t0.p2.coord = c2;

            _dptr->_ffd_ctrl[i][j].t1.p0.coord = c2;
            _dptr->_ffd_ctrl[i][j].t1.p1.coord = c1;
            _dptr->_ffd_ctrl[i][j].t1.p2.coord = c3;

            _dptr->_ffd_surface[i][j].coord_min_set(c0);
            _dptr->_ffd_surface[i][j].coord_max_set(c3);
        }
   }

}
void ffd_multi::coord_update(const int dir,
                             const QVector2D min, const QVector2D max)
{
    if (!(_dptr->_dirs & dir))
        return;
    if ((dir & FUSION_LEFT) != FUSION_NO)
    {
        for (int i = 0; i < _dptr->_ffd_surface.size (); i++)
        {
            QVector2D mins = _dptr->_ffd_surface[i][0].coord_min ();
            QVector2D maxs = _dptr->_ffd_surface[i][0].coord_max ();
            QVector2D temp = _dptr->_ffd_surface[i][1].coord_min ();

            mins.setX (min.x ());
            maxs.setX (max.x ());
            _dptr->_ffd_surface[i][0].coord_min_set(mins);
            _dptr->_ffd_surface[i][0].coord_max_set(maxs);

            temp.setX (max.x ());
            _dptr->_ffd_surface[i][1].coord_min_set(temp);
        }
    }
    else if ((dir & FUSION_RIGHT) != FUSION_NO)
    {
        for (int i = 0; i < _dptr->_ffd_surface.size (); i++)
        {
            int cols = _dptr->_ffd_surface[i].size ();

            QVector2D mins = _dptr->_ffd_surface[i][0].coord_min ();
            QVector2D maxs = _dptr->_ffd_surface[i][0].coord_max ();
            QVector2D temp = _dptr->_ffd_surface[i][1].coord_max ();

            mins.setX (min.x ());
            maxs.setX (max.x ());
            _dptr->_ffd_surface[i][cols-1].coord_min_set(mins);
            _dptr->_ffd_surface[i][cols-1].coord_max_set(maxs);

            temp.setX (min.x ());
            _dptr->_ffd_surface[i][cols-2].coord_max_set(temp);
        }
    }
    else if ((dir & FUSION_TOP) != FUSION_NO)
    {
        int rows = _dptr->_ffd_surface.size ();
        for (int i = 0; i < _dptr->_ffd_surface[rows-1].size (); i++)
        {
            QVector2D mins = _dptr->_ffd_surface[rows-1][i].coord_min ();
            QVector2D maxs = _dptr->_ffd_surface[rows-1][i].coord_max ();
            QVector2D temp = _dptr->_ffd_surface[rows-2][i].coord_max ();

            mins.setY (min.y ());
            maxs.setY (max.y ());
            _dptr->_ffd_surface[rows-1][i].coord_min_set(mins);
            _dptr->_ffd_surface[rows-1][i].coord_max_set(maxs);

            temp.setY (min.y ());
            _dptr->_ffd_surface[rows-2][i].coord_max_set(temp);
        }
    }
    else if ((dir & FUSION_BOTTOM) != FUSION_NO)
    {
        for (int i = 0; i < _dptr->_ffd_surface[0].size (); i++)
        {
            QVector2D mins = _dptr->_ffd_surface[0][i].coord_min ();
            QVector2D maxs = _dptr->_ffd_surface[0][i].coord_max ();
            QVector2D temp = _dptr->_ffd_surface[1][i].coord_min ();

            mins.setY (min.y ());
            maxs.setY (max.y ());

            _dptr->_ffd_surface[0][i].coord_min_set(mins);
            _dptr->_ffd_surface[0][i].coord_max_set(maxs);

            temp.setY (max.y ());
            _dptr->_ffd_surface[1][i].coord_min_set(temp);
        }
    }
}
void ffd_multi::create(const FusionDir dir,
        const QVector3D P0, const QVector3D P1,
        const QVector3D P2, const QVector3D P3)
{
    array3Dtwo oin;
    oin.resize (2);
    for (int i = 0; i < oin.size (); i++)
        oin[i].resize (2);

    oin[0][0] = P0;
    oin[0][1] = P1;
    oin[1][0] = P2;
    oin[1][1] = P3;

    int row = 1;
    int col = 1;

    _dptr->_dirs = dir;
    if ((dir & FUSION_LEFT) != FUSION_NO)
    {
        col ++;
    }
    if ((dir & FUSION_RIGHT) != FUSION_NO)
    {
        col ++;
    }
    if ((dir & FUSION_TOP) != FUSION_NO)
    {
        row ++;
    }
    if ((dir & FUSION_BOTTOM) != FUSION_NO)
    {
        row ++;
    }

    //生成ffd面位置
    _dptr->_ffd_ctrl = bezier_arithmetic::bezier_mesh(row+1, col+1,
                                           ffd_min(), ffd_max(), oin);
    for (int i = 0; i < _dptr->_ffd_ctrl.size (); i++)
    {
        _dptr->_ffd_ctrl[i].removeLast ();
    }
    _dptr->_ffd_ctrl.removeLast ();

    _dptr->_ffd_surface.resize (_dptr->_ffd_ctrl.size ());
    for (int i =0; i< _dptr->_ffd_ctrl.size (); i++)
    {
        _dptr->_ffd_surface[i].resize (_dptr->_ffd_ctrl[i].size ());
        for (int j = 0; j < _dptr->_ffd_ctrl[i].size (); j++)
        {
            //  ------------
            //  |p1       p3|
            //  |           |
            //  |           |
            //  |p0       p2|
            //  ------------
            QVector3D p0 = _dptr->_ffd_ctrl[i][j].t0.p0.vertex;
            QVector3D p2 = _dptr->_ffd_ctrl[i][j].t0.p1.vertex;
            QVector3D p1 = _dptr->_ffd_ctrl[i][j].t0.p2.vertex;
            QVector3D p3 = _dptr->_ffd_ctrl[i][j].t1.p2.vertex;

            _dptr->_ffd_surface[i][j].param () = this->param();
            _dptr->_ffd_surface[i][j].create(p0, p1, p2, p3);
            if (((dir & FUSION_LEFT) != FUSION_NO) && (j == 0))
            {
                _dptr->_ffd_surface[i][j].color_line () = QColor(200, 0, 0);
            }
            if (((dir & FUSION_RIGHT) != FUSION_NO) && ((j+1) == _dptr->_ffd_surface[i].size ()))
            {
                _dptr->_ffd_surface[i][j].color_line () = QColor(200, 0, 0);
            }
            if (((dir & FUSION_TOP) != FUSION_NO) && ((i+1) == _dptr->_ffd_surface.size ()))
            {
                _dptr->_ffd_surface[i][j].color_line () = QColor(200, 0, 0);
            }
            if (((dir & FUSION_BOTTOM) != FUSION_NO)&& (i == 0))
            {
                _dptr->_ffd_surface[i][j].color_line () = QColor(200, 0, 0);
            }

        }
    }

    update();
    coord_update();
}

void ffd_multi::update()
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].update();
        }
    }
}

void ffd_multi::_property(render_property & prop)
{
    uint ids = 0;
    float brightness = prop.render_brightness();
    float contrast = prop.render_contrast();
    float hue = prop.render_hue() ;
    float saturation = prop.render_saturation() ;
    float red = prop.render_red()  ;
    float green = prop.render_green() ;
    float blue = prop.render_blue()  ;
    float alpha = prop.render_alpha()  ;
    ids = prop.texture_id ();
    prop = (*this);

    prop.texture_id () = ids;
    prop.render_brightness() = brightness;
    prop.render_contrast() = contrast ;
    prop.render_hue() = hue;
    prop.render_saturation() = saturation;
    prop.render_red() = red  ;
    prop.render_green() = green ;
    prop.render_blue() = blue  ;
    prop.render_alpha() = alpha ;
}
void ffd_multi::_property(ffd_property & prop)
{
    QColor line_color = prop.color_line();
    QVector2D min = prop.coord_min();
    QVector2D max  = prop.coord_max ();

    prop = (*this);
    prop.color_line() = line_color;


    prop.coord_min() = min;
    prop.coord_max() = max;
}

void ffd_multi::render(const uint tex_id)
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _property(_dptr->_ffd_surface[i][j].property_render());
            _property(_dptr->_ffd_surface[i][j].property_ffd ());
            _dptr->_ffd_surface[i][j].render (tex_id);
        }
    }
    if (!sel_point.isNull())
        _dptr->_ffd_surface[0][0].render_select(sel_point);
}
void ffd_multi::select_begin(const QPoint &p,const QVector3D &pos)
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].select_begin (p, pos);
        }
    }
}
void ffd_multi::select_begin(const QRectF &p, const QVector3D &pos)
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].select_begin (p, pos);
        }
    }
}
void ffd_multi::select_move(const QVector3D &p)
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].select_move (p);
        }
    }
}
void ffd_multi::key_move(const int p)
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].key_move (p);
        }
    }
}

#include "message.h"
void ffd_multi::key_move(int dindex, const QVector3D &val)
{
    int index_i = (dindex >> 8) &0xff;//通道实际控制点
    int index_j = dindex &0xff;
    int device_i = (dindex >> 24) &0xff;//通道面控制点
    int device_j = (dindex >> 16) &0xff;
    if ((device_i <2) || (device_j < 2))//! 每个面的控制点不能低于2*2
        return;

    int surface_i = 0;
    int surface_j = 0;
    if (index_i < device_i) //! 9宫格的x方向计算当前控制点所在的面
        surface_i = 0;
    else if ((index_i >= device_i) && (index_i < device_i*2-1))
        surface_i = 1;
    else
        surface_i = 2;

    if (index_j < device_j)//! 9宫格的y方向计算当前控制点所在的面
        surface_j = 0;
    else if ((index_j >= device_j) && (index_j < device_j*2-1))
        surface_j = 1;
    else
        surface_j = 2;

    int control_i = 0;
    int control_j = 0;
    if (index_i < device_i)//! 9宫格的x方向计算当前控制点索引
    {
        control_i = index_i;
    }
    else if (index_i >= device_i*2-1)
    {
        control_i = index_i - device_i*2+2;
    }
    else if (index_i >= device_i)
    {
        control_i = index_i - device_i+1;
    }

    if (index_j < device_j)//! 9宫格的y方向计算当前控制点索引
    {
        control_j = index_j;
    }
    else if (index_j >= device_j*2-1)
    {
        control_j = index_j - device_j*2+2;
    }
    else if (index_j >= device_j)
    {
        control_j = index_j - device_j+1;
    }

    log_information(QString("key move: surface=%1,%2; control=%3, %4").arg (surface_i).arg (surface_j).arg (control_i).arg (control_j));

    //! 9宫格计算后的控制点调整
    _dptr->_ffd_surface[surface_i][surface_j].key_move(
                ((control_i << 8) & 0xff00) | (control_j & 0xff), val);

    sel_point =  _dptr->_ffd_surface[surface_i][surface_j].key_move_select
            (((control_i << 8) & 0xff00) | (control_j & 0xff));

    //! 有右融合带，并且x控制点在当前面的最后一个则移动后面的面第一个控制点
    if ((_dptr->_dirs & FUSION_RIGHT) || (_dptr->_dirs & FUSION_LEFT))
    {
        if ((surface_j < (_dptr->_ffd_surface[surface_i].size()-1)) &&
                (control_j == (device_j-1)))
        {
                _dptr->_ffd_surface[surface_i][surface_j+1].key_move(
                            ((control_i << 8) & 0xff00) | 0, val);
        }
    }
    //! 有顶部融合带，并且y控制点在当前面的最后一个则移动后面的面第一个控制点
    if ((_dptr->_dirs & FUSION_TOP)  || (_dptr->_dirs & FUSION_BOTTOM))
    {
        if ((surface_i < (_dptr->_ffd_surface.size()-1)) &&
                (control_i == (device_i-1)))
        {
                _dptr->_ffd_surface[surface_i+1][surface_j].key_move(
                            (0) | (control_j & 0xff), val);
        }
    }
    if (((_dptr->_dirs & FUSION_RIGHT) && (_dptr->_dirs & FUSION_TOP)) ||
            ((_dptr->_dirs & FUSION_LEFT) && (_dptr->_dirs & FUSION_TOP)) ||
            ((_dptr->_dirs & FUSION_LEFT) && (_dptr->_dirs & FUSION_BOTTOM)) ||
            ((_dptr->_dirs & FUSION_RIGHT) && (_dptr->_dirs & FUSION_BOTTOM)))
    {
        if (((surface_i < (_dptr->_ffd_surface.size()-1)) &&
                (control_i == (device_i-1))) &&
                ((surface_j < (_dptr->_ffd_surface[surface_i].size()-1)) &&
                  (control_j == (device_j-1))))
        {
                _dptr->_ffd_surface[surface_i+1][surface_j+1].key_move(
                            0, val);
        }
    }
}
void ffd_multi::select_end()
{
    for(int i=0;i < _dptr->_ffd_surface.size ();i++)
    {
        for(int j=0;j < _dptr->_ffd_surface[i].size ();j++)
        {
            _dptr->_ffd_surface[i][j].select_end ();
        }
    }
}
