#ifndef FFD_MULTI_H
#define FFD_MULTI_H

#include "ffd_group.h"

class ffd_multi_p;
class ffd_multi :public ffd_property, public render_property
{
private:
    ffd_multi_p *_dptr;

    void coord_update();//更新纹理坐标
    void _property(render_property & prop);
    void _property(ffd_property & prop);
public:
    ffd_multi();
    ~ffd_multi();

    //! [属性操作]
    render_property &property_render() {return (*this);}
    ffd_property &property_ffd() {return (*this);}

    QVector2D &ffd_min();
    QVector2D &ffd_max();
    void ffd_min(const QVector2D &);
    void ffd_max(const QVector2D &);
    void surface_count(const int row, const int col);
    void update();
    void coord_update(const int dir, const QVector2D min, const QVector2D max);
    //  ------------
    //  |p2       p3|
    //  |           |
    //  |           |
    //  |p0       p1|
    //  ------------
    void create(const FusionDir dir,
                const QVector3D P0, const QVector3D P1, //根据点位置生成多ffd
                const QVector3D P2, const QVector3D P3 );

    //! [渲染输出]
    void render(const uint tex_id);

    void select_begin(const QPoint &p, const QVector3D &pos);
    void select_begin(const QRectF &p, const QVector3D &pos);
    void select_move(const QVector3D &pos);
    void key_move(const int p);
    void key_move(int dindex, const QVector3D &val);
    void select_end();

    QVector3D sel_point;
};

#endif // FFD_MULTI_H
