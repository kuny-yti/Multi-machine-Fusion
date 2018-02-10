#ifndef FFD_SURFACE_H
#define FFD_SURFACE_H

#include "render_kernel.h"
#include "ffd_property.h"

class ffd_surface : public ffd_property
{
private:
    friend  class ffd_render;
private:
    arrayQuadsTwo _data;/// 当前面数据
    array3Dtwo _c_point;/// 当前面控制点数据

    arrayUint _select_object;/// 当前面被选中的对象
    QVector3D _bak_pos;/// 备份上次选中鼠标的位置
private:
    void _move(const QVector3D &local);//移动内部数据操作
    void coord_update();//纹理坐标更新

public:
    explicit ffd_surface();
    ~ffd_surface();

    ffd_property &property();//返回当前面的属性

    void coord_min_set(const QVector2D &min) ;//设置纹理坐标的范围
    void coord_max_set(const QVector2D &max) ;

    void update();//更新内部数据
    //  ------------
    //  |p2       p3|
    //  |           |
    //  |           |
    //  |p0       p1|
    //  ------------
    void create(const QVector3D &p0, const QVector3D &p1,
                const QVector3D &p2, const QVector3D &p3);//创建内部数据

    void select_begin(const QPoint &p, const QVector3D &pos);//选中操作
    void select_begin(const QRectF &p, const QVector3D &pos);
    void select_move(const QVector3D &p);
    void key_move(const int p);
    void key_move(int index, const QVector3D &val);
    QVector3D key_move_select(int index);
    void select_end();
};

#endif // FFD_SURFACE_H
