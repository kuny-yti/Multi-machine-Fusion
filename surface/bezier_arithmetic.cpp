#include "bezier_arithmetic.h"

QVector3D bezier_arithmetic::bezier_vertex(float uv, const
                                          QVector3D &p1,const QVector3D &p2)
{
    QVector3D o;
    o.setX (((1.0-uv) * p1.x()) + (uv * p2.x()));
    o.setY (((1.0-uv) * p1.y()) + (uv * p2.y()));
    o.setZ (((1.0-uv) * p1.z()) + (uv * p2.z()));
    return o;
}

QVector2D bezier_arithmetic::bezier_coord(const QVector2D &min, const QVector2D &max,
                                         const float j, const float i,
                                         const float xpos, const float ypos)
{
    QVector2D o;
    o.setX (min.x() + (max.x() - min.x()) *j / ypos) ;
    //o.setY ((1.0f-min.y()) - (max.y() -min.y()) *i / xpos);

    o.setY (min.y() +
           (max.y() -min.y()) *i / xpos);
    return o;
}

QVector3D bezier_arithmetic::bezier_point(const float u,const float v,
                                          const array3Dtwo &ctrl_v)
{
    int i, j, r;

    array3Dtwo vertex = ctrl_v;
    if (vertex.size () <=0) return QVector3D();

    j = 0;
    while (j < vertex[0].size ())
    {
        for (r=1; r < vertex.size (); r++)
        {
            for (i=0; i < vertex.size ()- r; i++)
            {
                vertex[i][j] = bezier_vertex(u,  vertex[i][j], vertex[i+1][j]);
            }
        }
        j++;
    }

    for (r=1; r < vertex[0].size (); r++)
    {
        for (j=0; j < vertex[0].size () - r; j++)
        {
            vertex[0][j] = bezier_vertex(v, vertex[0][j], vertex[0][j+1]);
        }
    }

    return vertex[0][0];
}

arrayQuadsTwo bezier_arithmetic::bezier_mesh(int row,int col,
                                     const QVector2D tex_min, const QVector2D tex_max,
                                     const array3Dtwo &ctrl_v)
{
    QVector3D P0, P1, P2, P3;
    if ((row <= 0)||(col <= 0)) return arrayQuadsTwo();

    arrayQuadsTwo _data;
    _data.resize (row);
    for (int i = 0; i < _data.size (); i++)
    {
        _data[i].resize (col);
    }
    int iXVpos = row-1;
    int iYVpos = col-1;
    float u,v;

    for (int i=0; i < row-1; i++)
    {
       for (int j=0; j < col-1; j++)
      {
            u = (float)i/(float)(row-1);
            v = (float)j/(float)(col-1);
            P0 = bezier_point(u, v, ctrl_v);

            u = (float)(i+1)/(float)(row-1);
            v = (float)j/(float)(col-1);
            P1 = bezier_point(u, v,ctrl_v);

            u = (float)i/(float)(row-1);
            v = (float)(j+1)/(float)(col-1);
            P2 = bezier_point(u, v,ctrl_v);

            u = (float)(i+1)/(float)(row-1);
            v = (float)(j+1)/(float)(col-1);
            P3 = bezier_point(u, v,ctrl_v);

            _data[i][j].t0.p0.vertex = P0;
            _data[i][j].t0.p0.color = QColor(255,255,255);
            _data[i][j].t0.p0.coord = bezier_coord(tex_min, tex_max,
                                    (float)j, (float)i, (float)iXVpos, (float)iYVpos);

            _data[i][j].t0.p1.vertex = P1;
            _data[i][j].t0.p1.color = QColor(255,255,255);
            _data[i][j].t0.p1.coord = bezier_coord(tex_min, tex_max,
                (float)j, (float)i+1., (float)iXVpos, (float)iYVpos);

            _data[i][j].t0.p2.vertex = P2;
            _data[i][j].t0.p2.color = QColor(255,255,255);
            _data[i][j].t0.p2.coord = bezier_coord(tex_min, tex_max,
               (float)j +1., (float)i, (float)iXVpos, (float)iYVpos);

            _data[i][j].t1.p0.vertex = P2;
            _data[i][j].t1.p0.color = QColor(255,255,255);
            _data[i][j].t1.p0.coord = _data[i][j].t0.p2.coord;

            _data[i][j].t1.p1.vertex = P1;
            _data[i][j].t1.p1.color = QColor(255,255,255);
            _data[i][j].t1.p1.coord = _data[i][j].t0.p1.coord;

            _data[i][j].t1.p2.vertex = P3;
            _data[i][j].t1.p2.color = QColor(255,255,255);
            _data[i][j].t1.p2.coord = bezier_coord(tex_min, tex_max,
               (float)j+1., (float)i+1., (float)iXVpos, (float)iYVpos);
        }
    }
    return _data;
}

///
/// \brief bezier_arithmetic::bezier_mesh
/// \param _data
/// \param tex_min
/// \param tex_max
/// \param ctrl_v
///
array3Dtwo bezier_arithmetic::bezier_mesh(int row, int col,
                        const QVector3D &ctrl_0,const QVector3D &ctrl_1,
                        const QVector3D &ctrl_2,const QVector3D &ctrl_3)
{
    if ((row <= 0) && (col <=0)) return array3Dtwo();

    float u,v;

    array3Dtwo ctrl_v;
    array3Dtwo _data;
    _data.resize (row);
    for (int i = 0; i < _data.size (); i++)
    {
        _data[i].resize (col);
    }
    ctrl_v.resize (2);
    for (int i = 0; i < ctrl_v.size (); i++)
    {
        ctrl_v[i].resize (2);
    }
    ctrl_v[0][0] = ctrl_0;
    ctrl_v[0][1] = ctrl_1;
    ctrl_v[1][0] = ctrl_2;
    ctrl_v[1][1] = ctrl_3;

    for (int i=0; i < row; i++)
    {
       for (int j=0; j < col; j++)
      {
            u = (float)i/(float)(row-1);
            v = (float)j/(float)(col-1);
            _data[i][j] = bezier_point(u, v, ctrl_v);
        }
    }
    return _data;
}
