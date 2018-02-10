#ifndef FFD_RENDER_H
#define FFD_RENDER_H

#include "render_property.h"
#include "render_kernel.h"

class ffd_render :public render_property, public render_kernel
{
private:
    friend  class ffd_surface;
    class ffd_surface *ffd_data;
protected:
    void _draw_point();
    void _draw_surface();
    void _draw_Line();
    void _draw_ctrl_line() ;
    void _draw_cross_line() ;
    void _draw_border_line() ;
    void _draw_ip();
    void _draw_screen_id();
    void _draw_vertex_coord() ;
    void _draw_ctrl_coord() ;
    void _draw_center_point();
    void _draw_center_circle() ;
    void _draw_four_circle() ;

    void _draw_point(const SurfaceMode su,const PolygonMode mode);
    void _draw_ctrl(const SurfaceMode su,const PolygonMode mode);

    void _draw_Line(const SurfaceMode su,const PolygonMode mode);
    void _draw_surface(const SurfaceMode, const PolygonMode mode);
public:
    ffd_render();

    render_property &property() {return (*this);}
    void execute(ffd_surface *ffd_data, const uint tex_id);

};

#endif // FFD_RENDER_H
