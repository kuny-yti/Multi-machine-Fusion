#include "ffd_render.h"
#include "ffd_surface.h"
#include <QDataStream>

ffd_render::ffd_render()
{
}

void ffd_render::execute(ffd_surface *_data, const uint tex_id)
{
    ffd_data = _data;

    if ((this->mode_draw () & DRAW_FILL) != 0)
    {
        activate_texture(MULTI_TEXTURE_0);
        enable(ENABLE_TEXTURE_2D);
        bind_texture (BIND_TEXTURE_2D, tex_id);

        _draw_surface (SURFACE_FRONT_AND_BACK, POLYGON_FILL);

        activate_texture(MULTI_TEXTURE_0);
        bind_texture (BIND_TEXTURE_2D, 0);
        disable (DISABLE_TEXTURE_2D);
    }

    if ((this->mode_draw () & DRAW_LINE) != 0)
    {
        _draw_Line (SURFACE_FRONT_AND_BACK, POLYGON_LINE);
        _draw_ctrl_line();
    }
    if ((this->mode_draw () & DRAW_POINT) != 0)
    {
        switch (ffd_data->mode_ctrl ())
        {
            case CTRL_FFD:
                _draw_ctrl (SURFACE_FRONT_AND_BACK, POLYGON_POINT);
            break;
            case CTRL_MESH:
                _draw_point (SURFACE_FRONT_AND_BACK, POLYGON_POINT);
            break;
            default:
            break;
        }
    }

}
void ffd_render::_draw_surface(const SurfaceMode su, const PolygonMode mode)
{
    polygon_mode(su, mode);
    for (int i=0; i< ffd_data->_data.size (); i++)
    {
        for (int j=0; j< ffd_data->_data[i].size (); j++)
        {
            quads_draw (ffd_data->_data[i][j]);
       }
    }
}

void ffd_render::_draw_Line(const SurfaceMode su,const PolygonMode mode)
{
    polygon_mode(su, mode);
    line_size (ffd_data->size_line ());
    for (int i=0; i< ffd_data->_data.size (); i++)
    {
        for (int j=0; j< ffd_data->_data[i].size (); j++)
        {
            Quads q = ffd_data->_data[i][j];
            q.t0.p0.color = ffd_data->color_line ();
            q.t0.p1.color = ffd_data->color_line ();
            q.t0.p2.color = ffd_data->color_line ();
            q.t1.p0.color = ffd_data->color_line ();
            q.t1.p1.color = ffd_data->color_line ();
            q.t1.p2.color = ffd_data->color_line ();

            quads_draw (q);
       }
    }
}

void ffd_render::_draw_point(const SurfaceMode su,const PolygonMode mode)
{
    polygon_mode(su, mode);
    point_size (ffd_data->size_point ());
    for (int i=0; i< ffd_data->_data.size (); i++)
    {
        for (int j=0; j< ffd_data->_data[i].size (); j++)
        {
            QColor c0,c1,c2,c3;
            c0 = c1 = c2 = c3 =ffd_data->color_vertex ();
            float s0 = 0,s1 = 0,s2 = 0,s3 = 0;
            s0 = s1 = s2 = s3 = ffd_data->size_point();
            if (!this->show_select ())///不是选择模式时不用对选择点进行设置
                for (int k = 0; k < ffd_data->_select_object.size (); k++)
                {
                    int row = (ffd_data->_select_object[k] >> 12) & 0xff;
                    int col = (ffd_data->_select_object[k] >> 4) & 0xff;
                    int sta = ffd_data->_select_object[k] & 0x0f;
                    if (row == i && col == j)
                    {
                        if (sta == 0)
                        {
                            c0 = ffd_data->color_vertex_select ();
                            s0 = ffd_data->size_point_select ();
                        }
                        else if (sta == 1)
                        {
                            c1 = ffd_data->color_vertex_select ();
                            s1 = ffd_data->size_point_select ();
                        }
                        else if (sta == 2)
                        {
                            c2 = ffd_data->color_vertex_select ();
                            s2 = ffd_data->size_point_select ();
                        }
                        else if (sta == 3)
                        {
                            c3 = ffd_data->color_vertex_select ();
                            s3 = ffd_data->size_point_select ();
                        }
                        break;
                    }
                }

            Primitive primi;
            primi = ffd_data->_data[i][j].t0.p0;
            primi.color = c0;
            point_size (s0);
            primitive_draw (primi);

            primi = ffd_data->_data[i][j].t0.p1;
            primi.color = c1;
            point_size (s1);
            primitive_draw (primi);

            primi = ffd_data->_data[i][j].t0.p2;
            primi.color = c2;
            point_size (s2);
            primitive_draw (primi);

            primi = ffd_data->_data[i][j].t1.p2;
            primi.color = c3;
            point_size (s3);
            primitive_draw (primi);
       }
    }
}
void ffd_render::_draw_ctrl(const SurfaceMode su,const PolygonMode mode)
{
    polygon_mode(su, mode);
    for (int i=0; i< ffd_data->_c_point.size (); i++)
    {
        for (int j=0; j< ffd_data->_c_point[i].size (); j++)
        {
            Primitive primi;
            primi.color = ffd_data->color_ctrl ();
            point_size (ffd_data->size_point ());
            if (!this->show_select ())///不是选择模式时不用对选择点进行设置
                for (int k = 0; k < ffd_data->_select_object.size (); k++)
                {
                    int row = (ffd_data->_select_object[k] >> 8) & 0xff;
                    int col = (ffd_data->_select_object[k]) & 0xff;
                    if (row == i && col == j)
                    {
                        primi.color = ffd_data->color_ctrl_select ();
                        point_size (ffd_data->size_point_select ());
                        break;
                    }
                }

            primi.vertex = ffd_data->_c_point[i][j];
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);

            primitive_draw (primi);
       }
    }
}

void ffd_render::_draw_ctrl_line()
{
    if (ffd_data->mode_ctrl() == CTRL_MESH)
    {
        return;
    }
    line_size (ffd_data->size_ctrl_line ());
    polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_LINE);
    for (int i=0; i< ffd_data->_c_point.size ()-1; i++)
    {
        for (int j=0; j< ffd_data->_c_point[i].size ()-1; j++)
        {
            Line lin;
            Primitive primi;

            primi.vertex = ffd_data->_c_point[i][j];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p0 = primi;

            primi.vertex = ffd_data->_c_point[i][j+1];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);

            lin.p1 = primi;
            line_draw (lin);

            primi.vertex = ffd_data->_c_point[i][j];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p0 = primi;

            primi.vertex = ffd_data->_c_point[i+1][j];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p1 = primi;
            line_draw (lin);

            primi.vertex = ffd_data->_c_point[i+1][j];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p0 = primi;

            primi.vertex = ffd_data->_c_point[i+1][j+1];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p1 = primi;
            line_draw (lin);

            primi.vertex = ffd_data->_c_point[i+1][j+1];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p0 = primi;

            primi.vertex = ffd_data->_c_point[i][j+1];
            primi.color = ffd_data->color_ctrl_line ();
            primi.coord = QVector2D(0, 0);
            primi.normal = QVector3D(0, 0, 0);
            lin.p1 = primi;
            line_draw (lin);
       }
    }
}

void ffd_render::_draw_ip()
{
    if (this->show_ip())
    {

    }
}
void ffd_render::_draw_screen_id()
{
    if (this->show_screen_id())
    {

    }
}
void ffd_render::_draw_vertex_coord()
{
    if (this->show_vertex_coord())
    {

    }
}
void ffd_render::_draw_ctrl_coord()
{
    if (this->show_ctrl_coord())
    {

    }
}
void ffd_render::_draw_center_point()
{
    if (this->show_center_point())
    {
    }
}
void ffd_render::_draw_center_circle()
{
    if (this->show_center_circle())
    {

    }
}
void ffd_render::_draw_four_circle()
{
    if (this->show_four_circle())
    {

    }
}
void ffd_render::_draw_border_line()
{
    if (this->show_border_line())
    {
    }
}
