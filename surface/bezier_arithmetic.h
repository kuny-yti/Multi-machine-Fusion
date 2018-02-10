#ifndef BEZIER_ARITHMETIC_H
#define BEZIER_ARITHMETIC_H
#include "type_define.h"

class bezier_arithmetic
{
public:
    static QVector3D bezier_vertex(float uv, const QVector3D &p1, const QVector3D &p2);

    static QVector2D bezier_coord(const QVector2D &min, const QVector2D &max,
                                 const float j, const float i,
                                 const float xpos, const float ypos);

    static QVector3D bezier_point(const float u,const float v,
                           const array3Dtwo &ctrl_v);

    static array3Dtwo bezier_mesh(int row, int col,
                            const QVector3D &ctrl_0,const QVector3D &ctrl_1,
                            const QVector3D &ctrl_2,const QVector3D &ctrl_3);

    static arrayQuadsTwo bezier_mesh(int row,int col,
                const QVector2D tex_min, const QVector2D tex_max,
                const array3Dtwo &ctrl_v);

};

#endif // BEZIER_ARITHMETIC_H
