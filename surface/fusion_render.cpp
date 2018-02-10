#include "fusion_render.h"
#include "fusion_calc.h"
#include "ffd_group.h"
#include "ffd_multi.h"
#include "message.h"

#include <QMessageBox>

int b = 0,c = 0,v = 0;

class fusion_render_p : public fusion_calc, public ffd_property
{
public:
    bool _isCreate;
    bool _is_press;
    bool _stop_run;
    arrayFFDMultiTwo _ffd_object;//投影仪对象
    typedef QVector <array3Dtwo> array3Do;
    array3Do _create;
    float zNera;
    bool _rect_select;
    QVector3D _temp_v;

    int select_index;

    ffd_property & property() {return (*this);}
    fusion_render_p()
    {
        _rect_select = false;
        _stop_run = true;
        _ffd_object.clear ();
        _isCreate = false;
        zNera = 0.502513f;
        select_index = 0;
        this->mode_ctrl () = CTRL_FFD;
    }

    ~fusion_render_p()
    {
        for (int i = 0; i < _ffd_object.size(); i++)
        {
            _ffd_object[i].clear ();
        }
        _ffd_object.clear ();
    }

    void _mouse_single_create();
    void _mouse_mulit_create();

    FusionDir pc_cols(const PorjectParam &param)
    {
        int dirs = FUSION_NO;
        if (param._pc_count_col == 1)
        {
            return (FusionDir)dirs;
        }
        if (param._pc_count_col_c > 0)
        {
            dirs |= FUSION_LEFT;
        }
        if ((param._pc_count_col_c+1) < param._pc_count_col)
        {
            dirs |= FUSION_RIGHT;
        }
        return (FusionDir)dirs;
    }
    FusionDir pc_dir(const PorjectParam &param)
    {
        int dirs = FUSION_NO;
        if (param._pc_count_row == 1)
        {
            return (FusionDir)pc_cols(param);
        }
        if (param._pc_count_row_c > 0)
        {
            dirs |= FUSION_BOTTOM;
        }
        if ((param._pc_count_row_c+1) < param._pc_count_row)
        {
            dirs |= FUSION_TOP;
        }
        dirs |= pc_cols(param);
        return (FusionDir)dirs;
    }
    FusionDir _cols(const PorjectParam &param, const int col)
    {
        int dirs = FUSION_NO;
        if (param._p_count_col == 1)
        {
            return (FusionDir)dirs;
        }

        if (col > 0)
        {
            dirs |= FUSION_LEFT;
        }
        if ((col+1) < param._p_count_col)
        {
            dirs |= FUSION_RIGHT;
        }
        return (FusionDir)dirs;
    }
    FusionDir _fusion_dir(const PorjectParam &param, const int row, const int col)
    {
        int dirs = FUSION_NO;
        if (param._p_count_row == 1)
        {
            return (FusionDir)_cols(param, col);
        }

        if (row > 0)
        {
            dirs |= FUSION_BOTTOM;
        }
        if ((row+1) < param._p_count_row)
        {
            dirs |= FUSION_TOP;
        }
        dirs |= _cols(param, col);
        return (FusionDir)dirs;
    }
};

void fusion_render_p::_mouse_single_create()
{
    for (int i = 0; i < _ffd_object.size (); i++)
    {
        _ffd_object[i].clear();
    }
    _ffd_object.clear();

    _ffd_object.resize(_create.size ());
    for (int i = 0; i < _create.size (); i++)
    {
        _ffd_object[i].resize (_create[i].size ());
        for (int j = 0; j < _create[i].size (); j++)
        {
            FusionDir dir_ = pc_dir(this->param ());
            dir_ = (FusionDir)(dir_ | _fusion_dir(this->param (), i, j));
            _ffd_object[i][j].param() = (*this).param ();
            _ffd_object[i][j].create(dir_,
                    _create[(_create.size ()-1)-i][j][2],
                    _create[(_create.size ()-1)-i][j][3],
                    _create[(_create.size ()-1)-i][j][0],
                    _create[(_create.size ()-1)-i][j][1]);

            _ffd_object[i][j].update();
        }
    }
    _isCreate = true;
    this->param()._policy = CREATE_NO;
}
void fusion_render_p::_mouse_mulit_create()
{

}

fusion_render::fusion_render()
{
    _dptr = new fusion_render_p();
    _dptr->_is_press = false;
    this->size_point() = 10.;
    this->size_point_select() = 15.;
}

fusion_render::~fusion_render()
{
    delete _dptr;
}
void fusion_render::update_comput(const RawConfig & pp)
{
    ffd_property::param ()._pc_count_col = idLow (pp.computer_layout);
    ffd_property::param ()._pc_count_col_c = idLow (pp.computer_index);
    ffd_property::param ()._pc_count_row = idHigh (pp.computer_layout);
    ffd_property::param ()._pc_count_row_c = idHigh (pp.computer_index);
}
void fusion_render::param_set(const RawConfig &pro)
{
    _dptr->_isCreate = false;

    ffd_property::param ()._pc_count_col = idLow (pro.computer_layout);
    ffd_property::param ()._pc_count_row = idHigh (pro.computer_layout);

    ffd_property::param ()._pc_count_col_c = idLow (pro.computer_index);
    ffd_property::param ()._pc_count_row_c = idHigh (pro.computer_index);

    ffd_property::param ()._p_count_col = idLow (pro.device_layout);
    ffd_property::param ()._p_count_row = idHigh (pro.device_layout);

    ffd_property::param ()._c_count_col = idLow (pro.geometry_control);
    ffd_property::param ()._c_count_row = idHigh (pro.geometry_control);

    ffd_property::param ()._v_count_col = idLow (pro.geometry_precision);
    ffd_property::param ()._v_count_row = idHigh (pro.geometry_precision);

    ffd_property::param ()._policy = (CreatePolicy)pro.device_strategy;
    ffd_property::param ()._resol = (ResolutionType)pro.device_resolution;
}

void fusion_render::mouse_create()
{
    if (!_dptr->_isCreate && _dptr->_create.size () &&
            ((_dptr->param()._policy == CREATE_MOUSE_SINGLE)
            || (_dptr->param()._policy == CREATE_MOUSE_MULIT)))
    {
        if (_dptr->param()._policy == CREATE_MOUSE_SINGLE)
        {
            _dptr->_mouse_single_create();
        }
        else if (_dptr->param()._policy == CREATE_MOUSE_MULIT)
        {
            _dptr->_mouse_mulit_create();
        }
    }
}

void fusion_render::auto_create()
{
    float x= -0.95;
    float y = -0.95;

    float sw = 0.04;
    float tw = 1.9f / (float)(_dptr->param()._p_count_col);
    float th = 1.9f / (float)(_dptr->param()._p_count_row);

    float w1 = 1.9f - ((float)(_dptr->param()._p_count_col -1) *sw);///除去中间间隔的总
    float h1 = 1.9f - ((float)(_dptr->param()._p_count_row -1) *sw);

    float w = w1 / (float)_dptr->param()._p_count_col;
    float h = h1 / (float)_dptr->param()._p_count_row;

    _dptr->_ffd_object.resize(_dptr->param()._p_count_row);
    for (int i = 0; i < _dptr->_ffd_object.size(); i++)
    {
        _dptr->_ffd_object[i].resize (_dptr->param()._p_count_col);
        for (int j = 0; j < _dptr->_ffd_object[i].size (); j++)
        {
            //  ------------
            //  |p1       p3|
            //  |           |
            //  |           |
            //  |p0       p2|
            //  ------------
            QVector3D p0 = QVector3D(x+((float)j*tw), y+((float)i*th), 0);
            QVector3D p2 = QVector3D(x+((float)j*tw)+w, y+((float)i*th), 0);
            QVector3D p1 = QVector3D(x+((float)j*tw), y+((float)i*th)+h, 0);
            QVector3D p3 = QVector3D(x+((float)j*tw)+w, y+((float)i*th)+h, 0);

            FusionDir dir_ = _dptr->pc_dir(this->param ());
            dir_ = (FusionDir)(dir_ | _dptr->_fusion_dir(this->param (), i, j));

            _dptr->_ffd_object[i][j].param() = _dptr->param();
            _dptr->_ffd_object[i][j].create(dir_,
                                     p0, p2, p1, p3);

            _dptr->_ffd_object[i][j].update();
        }
    }
    _dptr->_isCreate = true;
    _dptr->param()._policy = CREATE_NO;
}

void fusion_render::mesh_create()
{
    _dptr->property() = (*this);
    if (_dptr->_isCreate)
    {
        return;
    }
    else
    {
        for (int i = 0; i < _dptr->_ffd_object.size(); i++)
        {
            _dptr->_ffd_object[i].clear();
        }
        _dptr->_ffd_object.clear();
    }

    if (_dptr->param()._policy == CREATE_MOUSE_SINGLE)
    {
        _dptr->_create.resize (_dptr->param()._p_count_row);
        for (int i =0; i < _dptr->_create.size (); i++)
        {
            _dptr->_create[i].resize (_dptr->param()._p_count_col);
            for (int j = 0; j < _dptr->_create[i].size (); j++)
            {
                _dptr->_create[i][j].resize (4);
            }
        }
    }
    else if (_dptr->param()._policy == CREATE_MOUSE_MULIT)
    {

    }
    else if (_dptr->param()._policy == CREATE_AUTO)
    {
        auto_create();
    }
}

void fusion_render::movs(const int ps)
{
    for (int i = 0; i < _dptr->_ffd_object.size(); i++)
    {
        for (int j = 0; j < _dptr->_ffd_object[i].size(); j++)
        {
            _dptr->_ffd_object[i][j].key_move(ps);
        }
    }
}
void fusion_render::select_begin(const QPoint &p)
{
    if (_dptr->_isCreate)
    {
        if (_dptr->_rect_select)
        {
            _dptr->_rect_select = false;
            _dptr->zNera = 0.502513f;
            return;
        }
        QVector3D ps = render_kernel::screen_pos(p, _dptr->zNera);

        for (int i = 0; i < _dptr->_ffd_object.size(); i++)
        {
            for (int j = 0; j < _dptr->_ffd_object[i].size(); j++)
            {
                _dptr->_ffd_object[i][j].show_select () = true;
                _dptr->_ffd_object[i][j].select_begin(p, ps);
            }
        }
    }
    else if ((_dptr->param ()._policy == CREATE_MOUSE_SINGLE)
             || (_dptr->param ()._policy == CREATE_MOUSE_MULIT))
    {
        float ace = 0.502513f;//在z为0的状态下转换x、y坐标到三维空间中
        _dptr->_temp_v = render_kernel::screen_pos(p, ace);
        _dptr->_is_press = true;
    }
}
void fusion_render::select_begin(const QRectF &p)
{
    const QPoint &pos = QPoint(p.x(), p.y());

    _dptr->_rect_select = true;
    QVector3D ps = render_kernel::screen_pos(pos, _dptr->zNera);
    for (int i = 0; i < _dptr->_ffd_object.size(); i++)
    {
        for (int j = 0; j < _dptr->_ffd_object[i].size(); j++)
        {
            _dptr->_ffd_object[i][j].show_select () = true;
            _dptr->_ffd_object[i][j].select_begin(p, ps);
        }
    }
}
void fusion_render::select_move(const QPoint &p)
{
    QVector3D ps = render_kernel::screen_pos(p, _dptr->zNera);
    if (_dptr->_isCreate)
    {
        for (int i = 0; i < _dptr->_ffd_object.size(); i++)
        {
            for (int j = 0; j < _dptr->_ffd_object[i].size(); j++)
            {
                _dptr->_ffd_object[i][j].select_move(ps);
            }
        }
    }
    else if ((_dptr->param ()._policy == CREATE_MOUSE_SINGLE)
             || (_dptr->param ()._policy == CREATE_MOUSE_MULIT))
    {
        if (_dptr->_is_press)
        {
            float ace = 0.502513f;//在z为0的状态下转换x、y坐标到三维空间中
            _dptr->_temp_v = render_kernel::screen_pos(p, ace);
        }
    }
}

#include "message.h"
void fusion_render::key_move(int did, int dindex,const QVector3D &val)
{
    int i = (did >> 8) &0xff;
    int j = did &0xff;
    if (i >= _dptr->_ffd_object.size())
        return;
    if (j >= _dptr->_ffd_object[i].size())
        return;

    log_information(
                QString("key move:(device id=%1; device index=%2; move value=%3,%4)")
                    .arg (QString::number (did, 16))
                    .arg (QString::number (dindex,16))
                    .arg (val.x ()).arg (val.y ()));
    if (_dptr->select_index != did)
    {
        _dptr->select_index = did;
        for (int dx = 0; dx < _dptr->_ffd_object.size(); ++dx)
        {
            for (int dy = 0; dy < _dptr->_ffd_object[dx].size(); ++dy)
            {
                _dptr->_ffd_object[dx][dy].sel_point = QVector3D();
            }
        }
    }
    _dptr->_ffd_object[i][j].key_move(dindex, val);
    log_information("key move success.");
}
void fusion_render::select_end()
{
    if (_dptr->_isCreate)
    {
        for (int i = 0; i < _dptr->_ffd_object.size(); i++)
        {
            for (int j = 0; j < _dptr->_ffd_object[i].size(); j++)
            {
                if (_dptr->_rect_select)
                {
                }
                else
                {
                    _dptr->_ffd_object[i][j].select_end();
                    _dptr->_ffd_object[i][j].show_select () = false;
                }
            }
        }
        if (_dptr->_rect_select == 1)
        {
            _dptr->_rect_select = 2;
        }
        else if (_dptr->_rect_select == 2)
        {
            _dptr->_rect_select = 0;
        }
        _dptr->zNera = 0.502513f;
    }
    else if ((_dptr->param ()._policy == CREATE_MOUSE_SINGLE)
             || (_dptr->param ()._policy == CREATE_MOUSE_MULIT))
    {
        if (_dptr->_is_press)
        {
            _dptr->_is_press = false;
            _dptr->_create[b][c][v] = _dptr->_temp_v;
            Primitive primit;
            primit.color = QColor(100, 150, 180);
            primit.vertex = _dptr->_create[b][c][v];
            _temp_create << primit;
            v++;
            if (v == 4)
            {
                v = 0;
                c++;
            }
            if (c == _dptr->param ()._p_count_col)
            {
                c = 0;
                b++;
            }
            if (b == _dptr->param ()._p_count_row)
            {
                v = 0;
                c = 0;
                b = 0;
                mouse_create();
                _temp_create.clear ();
            }
        }
    }
}


void fusion_render::_property(render_property & prop)
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
void fusion_render::_property(ffd_property & prop)
{
    QColor line_color = prop.color_line();
    QVector2D min = prop.coord_min();
    QVector2D max  = prop.coord_max ();

    prop = (*this);
    prop.color_line() = line_color;

    prop.coord_min() = min;
    prop.coord_max() = max;
}
void fusion_render::coord_update(const uint id, const int dir,
                  const QVector2D min, const QVector2D max)
{
    int row = (id >> 8) &0xff;
    int col  = id &0xff;
    _dptr->_ffd_object[row][col].coord_update (dir, min, max);

}

void fusion_render::render(const uint id,const uint tex_id)
{
    _dptr->property() = (*this);

    if (_temp_create.size())
    {
        point_size (20);
        primitive_draw(_temp_create);
        point_size (1);
    }

    int row = (id >> 8) &0xff;
    int col = id &0xff;
    if ((row >= _dptr->_ffd_object.size ()) ||
        (col >= _dptr->_ffd_object[0].size ()))
    {
        return;
    }
    _property(_dptr->_ffd_object[row][col].property_render());
    _property(_dptr->_ffd_object[row][col].property_ffd ());
    _dptr->_ffd_object[row][col].render (tex_id);

}
