#include "ffd_surface.h"
#include "ray_pick.h"
#include "bezier_arithmetic.h"
#include <QGLFunctions>
#include <QDataStream>
#include <QDebug>

ffd_surface::ffd_surface()
{
    _c_point.clear ();
    _data.clear ();
    _select_object.clear();

    _bak_pos = QVector3D(0,0,0);
}
ffd_surface::~ffd_surface()
{
    for (int i = 0; i < _c_point.size (); i++)
    {
        _c_point[i].clear ();
    }
    _c_point.clear ();

    for (int i = 0; i < _data.size (); i++)
    {
        _data[i].clear ();
    }
    _data.clear ();

    _select_object.clear();
}

void ffd_surface::coord_min_set(const QVector2D &min)
{
    _t_min = min;
    update();
}

void ffd_surface::coord_max_set(const QVector2D &max)
{
    _t_max = max;
    update();
}

void ffd_surface::update()
{
    //更新内部数据(生成的时候一定要先更新一次)
    _data = bezier_arithmetic::bezier_mesh(
                _param._v_count_row,
                _param._v_count_col,
                _t_min,
                _t_max,
                _c_point);

}
void ffd_surface::create(const QVector3D &p0, const QVector3D &p1,
                       const QVector3D &p2, const QVector3D &p3)
{
    //  ------------
    //  |p1       p3|
    //  |           |
    //  |           |
    //  |p0       p2|
    //  ------------
    array3Dtwo oin;
    oin.resize (2);
    for (int i = 0; i < oin.size (); i++)
    {
        oin[i].resize (2);
    }
    oin[0][0] = p0;
    oin[0][1] = p1;
    oin[1][0] = p2;
    oin[1][1] = p3;

    //生成控制点
    arrayQuadsTwo _tem_c_point = bezier_arithmetic::bezier_mesh(
                _param._c_count_row,
                _param._c_count_col,
                _t_min,
                _t_max,
                oin);
    //  ------------
    //  |p2       p3|
    //  |           |
    //  |           |
    //  |p0       p1|
    //  ------------
    _c_point.resize (_param._c_count_row);
    for(int k=0; k < _c_point.size (); k++)
    {
        _c_point[k].resize (_param._c_count_col);
    }

    for (int i = 0; i < _c_point.size (); i++)
    {
        if ((i == _c_point.size ()-1))
        {
            for (int k = 0; k < _c_point[i].size (); k++)
            {
                if (k == _c_point[i].size ()-1)
                {
                    _c_point[i][k] = _tem_c_point[i-1][k-1].t1.p2.vertex;
                }
                else
                    _c_point[i][k] = _tem_c_point[i-1][k].t0.p1.vertex;
            }
        }
        else
            for (int j = 0; j < _c_point[i].size (); j++)
            {
                if (j == _c_point[i].size ()-1)
                {
                    _c_point[i][j] = _tem_c_point[i][j-1].t0.p2.vertex;
                }
                else
                    _c_point[i][j] = _tem_c_point[i][j].t0.p0.vertex;
            }
    }

    update();
}

void ffd_surface::select_begin(const QPoint &p, const QVector3D &pos)
{
    QVector3D _near, _far;
    render_kernel::ray_pos(p, _near, _far);
    _bak_pos = pos;

    if (this->mode_ctrl() == CTRL_FFD)
    {
        for (int r = 0; r < _c_point.size(); r++)
        {
            for (int c = 0; c < _c_point[r].size(); c++)
            {
                QVector3D tem = _c_point[r][c];
                bool bRet = ray_pick::pick(tem , _near, _far);
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 8) & 0xff00;
                    inx = inx | c;

                    _select_object << inx;
                }
            }
        }
    }
    else if (this->mode_ctrl() == CTRL_MESH)
    {
        for (int r = 0; r < _data.size(); r++)
        {
            for (int c = 0; c < _data[r].size(); c++)
            {
                QVector3D p0 = _data[r][c].t0.p0.vertex;
                QVector3D p1 = _data[r][c].t0.p1.vertex;
                QVector3D p2 = _data[r][c].t0.p2.vertex;
                QVector3D p3 = _data[r][c].t1.p2.vertex;

                bool bRet = ray_pick::pick(p0 , _near, _far);
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 0;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p1 , _near, _far);
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 1;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p2 , _near, _far);
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 2;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p3 , _near, _far);
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 3;

                    _select_object << inx;
                }
            }
        }
    }

}
void ffd_surface::_move(const QVector3D &local)
{
    for (int k = 0; k < _select_object.size(); k++)
    {
        if (this->mode_ctrl() == CTRL_FFD)
        {
            int row = (_select_object[k] >> 8) & 0xff;
            int col = (_select_object[k]) & 0xff;

            if (((_c_point[row][col] + local).x() < 2.0) &&
                    ((_c_point[row][col] + local).y() < 2.0))
                _c_point[row][col] += local;
        }
        else if (this->mode_ctrl() == CTRL_MESH)
        {
            int row = (_select_object[k] >> 12) & 0xff;
            int col = (_select_object[k] >> 4) & 0xff;
            int sta = _select_object[k] & 0x0f;

            if (((_data[row][col].t0.p0.vertex + local).x() < 2.0) &&
                    ((_data[row][col].t0.p0.vertex + local).y() < 2.0))
                if (sta == 0)
                {
                    _data[row][col].t0.p0.vertex += local;
                }
                else if (sta == 1)
                {
                    _data[row][col].t0.p1.vertex += local;
                    _data[row][col].t1.p1.vertex += local;
                }
                else if (sta == 2)
                {
                    _data[row][col].t0.p2.vertex += local;
                    _data[row][col].t1.p0.vertex += local;
                }
                else if (sta == 3)
                {
                    _data[row][col].t1.p2.vertex += local;
                }
        }
    }
    if (this->mode_ctrl() == CTRL_MESH)
        return;
    this->update();
}
void ffd_surface::key_move(const int p)
{
    switch (p)
    {
        case 0://y+
            _move(QVector3D(0, this->size_accuracy (), 0));
        break;
        case 1://y-
            _move(QVector3D(0, -(this->size_accuracy ()), 0));
        break;
        case 2://x+
            _move(QVector3D(this->size_accuracy (), 0, 0));
        break;
        case 3://x-
            _move(QVector3D(-(this->size_accuracy ()), 0, 0));
        break;
        case 4://z+
            _move(QVector3D(0, 0, this->size_accuracy ()));
        break;
        case 5://z-
            _move(QVector3D(0, 0, -(this->size_accuracy ())));
        break;
        default:
        break;
    }
}
void ffd_surface::key_move(int index, const QVector3D &val)
{
    if (this->mode_ctrl() == CTRL_FFD)
    {
        _select_object.append(index);
        _move(val);
        _select_object.clear();
    }
}
QVector3D ffd_surface::key_move_select(int index)
{
    if (this->mode_ctrl() == CTRL_FFD)
    {
        int row = (index >> 8) & 0xff;
        int col = (index) & 0xff;

        return _c_point[row][col];
    }
    return QVector3D();
}
void ffd_surface::select_move(const QVector3D &pos)
{
    QVector3D local = pos - _bak_pos;//screen_pos(p, _bak_z);
    ////local = local *QVector3D(2.0, 2.0, 0);
    _move(local);
    _bak_pos = pos;
}
void ffd_surface::select_begin(const QRectF &p, const QVector3D &pos)
{
    QVector3D _near, _far;
    render_kernel::ray_pos(QPoint(p.x(), p.y()), _near, _far);
    _bak_pos = pos;

    if (this->mode_ctrl() == CTRL_FFD)
    {
        for (int r = 0; r < _c_point.size(); r++)
        {
            for (int c = 0; c < _c_point[r].size(); c++)
            {
                QVector3D tem = _c_point[r][c];
                bool bRet = ray_pick::pick(tem , _near, _far,
                                           QSizeF(p.width(), p.height()));
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 8) & 0xff00;
                    inx = inx | c;

                    _select_object << inx;
                }
            }
        }
    }
    else if (this->mode_ctrl() == CTRL_MESH)
    {
        for (int r = 0; r < _data.size(); r++)
        {
            for (int c = 0; c < _data[r].size(); c++)
            {
                QVector3D p0 = _data[r][c].t0.p0.vertex;
                QVector3D p1 = _data[r][c].t0.p1.vertex;
                QVector3D p2 = _data[r][c].t0.p2.vertex;
                QVector3D p3 = _data[r][c].t1.p2.vertex;

                bool bRet = ray_pick::pick(p0 , _near, _far,
                                           QSize(p.width(), p.height()));
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 0;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p1 , _near, _far,
                                      QSize(p.width(), p.height()));
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 1;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p2 , _near, _far,
                                      QSize(p.width(), p.height()));
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 2;

                    _select_object << inx;
                }
                bRet = ray_pick::pick(p3 , _near, _far,
                                      QSize(p.width(), p.height()));
                if (bRet)
                {
                    uint inx = 0;
                    inx = (r << 12) & 0xff000;
                    inx = inx | ((c << 4) &0xff0);
                    inx = inx | 3;

                    _select_object << inx;
                }
            }
        }
    }
}
void ffd_surface::select_end()
{
    _bak_pos = QVector3D(0,0,0);
    _select_object.clear();
}
ffd_property &ffd_surface::property()
{
    return *this;
}
