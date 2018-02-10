#include "ffd_group.h"
#include "bezier_arithmetic.h"

ffd_group::ffd_group()
{
}

void ffd_group::render_select(const QVector3D &po)
{
    if ((this->mode_draw () & DRAW_POINT) == 0)
        return;
    polygon_mode(SURFACE_FRONT_AND_BACK, POLYGON_POINT);
    point_size (this->size_point_select ());
    Primitive primi;
    primi.color = QColor(0,50,200,255);
    primi.vertex = po;
    primi.coord = QVector2D(0, 0);
    primi.normal = QVector3D(0, 0, 0);

    primitive_draw (primi);
}
void ffd_group::render(const uint tex_id)
{
    this->execute (this, tex_id);
}
