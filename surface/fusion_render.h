#ifndef RENDER_GROUP_H
#define RENDER_GROUP_H
#include "type_define.h"
#include "ui_define.h"
#include "render_kernel.h"
#include "ffd_property.h"
#include "render_property.h"

class fusion_render_p;
class fusion_render : public render_kernel, public render_property, public ffd_property
{
    fusion_render_p *_dptr;
    arrayPrimitive _temp_create;

    void auto_create();
    void mouse_create();
    void _property(render_property & prop);
    void _property(ffd_property & prop);

public:
    fusion_render();
    ~fusion_render();

    //! [1]
    void update_comput(const RawConfig &);
    void param_set(const RawConfig &pro);
    //! [1]

    //! [2]
    void mesh_create();
    //! [2]

    //! [3]
    void texture_id(const uint id, const uint tex_id);
    //! [3]

    //! [4]
    void render(const uint id, const uint tex_id);
    //! [4]

    void movs(const int ps);
    void select_begin(const QPoint &p);
    void select_begin(const QRectF &p);
    void select_move(const QPoint &p);
    void select_end();

    void key_move(int did, int dindex,const QVector3D &val);

    void coord_update(const uint id, const int dir,
                      const QVector2D min, const QVector2D max);
};

#endif // RENDER_GROUP_H
