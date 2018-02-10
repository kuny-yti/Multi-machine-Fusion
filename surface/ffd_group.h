#ifndef FFD_GROUP_H
#define FFD_GROUP_H

#include "ffd_surface.h"
#include "ffd_render.h"

class ffd_group : public ffd_surface, public ffd_render
{
private:

public:
    ffd_group();

    //! [属性操作]
    render_property &property_render() {return (*this);}
    ffd_property &property_ffd() {return (*this);}

    //! [渲染输出]
    void render(const uint tex_id);
    void render_select(const QVector3D &po);

};

#endif // FFD_GROUP_H
