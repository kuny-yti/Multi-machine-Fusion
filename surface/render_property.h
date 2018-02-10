#ifndef RENDER_PROPERTY_H
#define RENDER_PROPERTY_H
#include "type_define.h"

///
/// \brief The render_property class
/// 1.显示信息
/// 2.纹理id
/// 3.渲染属性
/// 4.渲染模式
///
class render_property
{
public:
    render_property();

    inline bool  &show_ip() {return _s_ip;}
    inline bool  &show_screen_id() {return _s_screen_id;}
    inline bool  &show_vertex_coord() {return _s_vertex_coord;}
    inline bool  &show_ctrl_coord() {return _s_ctrl_coord;}
    inline bool  &show_center_point() {return _s_center_point;}
    inline bool  &show_center_circle() {return _s_center_circle;}
    inline bool  &show_four_circle() {return _s_four_circle;}
    inline bool  &show_border_line() {return _s_border_line;}
    inline bool  &show_cross_line() {return _s_cross_line;}
    inline bool  &show_texture_backgroud() {return _s_texture_backgroud;}
    inline bool  &show_texture() {return _s_texture;}
    inline bool  &show_select() {return _s_select;}

    inline uint &texture_backgroud_id() {return _t_backgroud_id;}
    inline uint &texture_id() {return _t_texture;}

    inline float &render_brightness() {return _r_brightness;}
    inline float &render_contrast(){return _r_contrast;}
    inline float &render_hue(){return _r_hue;}
    inline float &render_saturation(){return _r_saturation;}

    inline float &render_red() {return _r_red;}
    inline float &render_green() {return _r_green;}
    inline float &render_blue() {return _r_blue;}
    inline float &render_alpha() {return _r_alpha;}

    inline float &fusion_brightness() {return _f_brightness;}
    inline float &fusion_gamma() {return _f_gamma;}

    inline DrawMode &mode_draw() {return _m_draw;}
    inline RenderMode &mode_rend() {return _m_rend;}

    inline render_property *operator()(){return this;}
    static QByteArray to_byte(render_property *);
    static void to_render(QByteArray &byte_data, render_property &f);
protected:
    /// 绘制时的属性
    bool  _s_ip;            /// ip地址(随时)
    bool  _s_screen_id;     /// 屏幕id(随时)
    bool  _s_vertex_coord;  /// 顶点坐标(随时)
    bool  _s_ctrl_coord;    /// 控制点坐标(随时)
    bool  _s_cross_line;    /// 十字架,交叉线(随时)
    bool  _s_center_point;  /// 中心点(随时)
    bool  _s_center_circle; /// 中线圆(随时)
    bool  _s_four_circle;   /// 四角圆(随时)
    bool  _s_border_line;   /// 边界线(随时)
    bool  _s_texture_backgroud;/// 纹理背景(true为纹理背景, false为颜色背景)
    bool  _s_texture;       /// 对象纹理绘制(用于调试图像纹理)
    bool  _s_select;        /// 对象选择(true 选择，false 不选择 )

    DrawMode _m_draw;   //绘制模式
    RenderMode _m_rend; //渲染模式

    /// 绘制时的纹理数据
    uint _t_backgroud_id;   /// 纹理背景id
    uint  _t_texture;       /// 纹理图像id

    /// 绘制时的颜色数据
    float _r_brightness;///亮度 -1...1
    float _r_contrast;///对比度 -1...1
    float _r_hue;///色调 -1...1
    float _r_saturation;///饱和度 -1...1

    float _r_red;///红基色 0...1(0为全黑，<1减低亮度，>1加大亮度，会出现变色)
    float _r_green;///绿基色 0...1
    float _r_blue;///蓝基色 0...1
    float _r_alpha;///透明 0...1

    /// 融合带参数
    float _f_brightness;
    float _f_gamma;
};

inline render_property::render_property()
{
    _m_draw = DRAW_POINT_AND_LINE;
    _m_rend = RENDER_RENDER;
    _s_texture = true;

    _s_center_point = false;  /// 中心点
    _s_border_line = false;   /// 边界线
    _s_ip = false;            /// ip地址
    _s_screen_id = false;     /// 屏幕id
    _s_vertex_coord = false;  /// 顶点坐标
    _s_ctrl_coord = false;    /// 控制点坐标
    _s_cross_line = false;    /// 十字架,交叉线
    _s_center_circle = false; /// 中线圆
    _s_four_circle = false;   /// 四角圆
    _s_texture_backgroud = false;/// 纹理背景(true为纹理背景,false为颜色背景)

    _s_select = false;

    _t_texture = 0;
    _t_backgroud_id = 0;   /// 纹理背景id

    _r_brightness = 0.;///亮度
    _r_contrast = 0.;///对比度
    _r_hue = 0.;///色调
    _r_saturation = 0.;///饱和度

    _r_red = 1;///红基色 0...1
    _r_green = 1;///绿基色 0...1
    _r_blue = 1;///蓝基色 0...1
    _r_alpha = 1;///透明 0...1
}

inline QByteArray render_property::to_byte(render_property *r)
{
    QByteArray byte_data;
    QDataStream out(&byte_data, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_1);

    out << r->show_ip() ;
    out << r->show_screen_id();
    out << r->show_vertex_coord();
    out << r->show_ctrl_coord();
    out << r->show_center_point() ;
    out << r->show_center_circle();
    out << r->show_four_circle();
    out << r->show_border_line() ;
    out << r->show_cross_line();
    out << r->show_texture_backgroud();
    out << r->show_texture();
    out << r->show_select();

    //out << r->texture_backgroud_id() ;
    //out << r->texture_id();

    out << r->render_brightness() ;
    out << r->render_contrast();
    out << r->render_hue();
    out << r->render_saturation();

    out << r->render_red();
    out << r->render_green() ;
    out << r->render_blue() ;
    out << r->render_alpha();

    out << r->fusion_brightness() ;
    out << r->fusion_gamma() ;

    out << (uint)r->mode_draw();
    out << (uint)r->mode_rend();
    return byte_data;
}
inline void render_property::to_render(QByteArray &byte_data, render_property &r)
{
    QDataStream in(&byte_data, QIODevice::ReadOnly);
    in.setVersion (QDataStream::Qt_5_1);

    int tem = 0;
    in >> r.show_ip() ;
    in >> r.show_screen_id();
    in >> r.show_vertex_coord();
    in >> r.show_ctrl_coord();
    in >> r.show_center_point() ;
    in >> r.show_center_circle();
    in >> r.show_four_circle();
    in >> r.show_border_line() ;
    in >> r.show_cross_line();
    in >> r.show_texture_backgroud();
    in >> r.show_texture();
    in >> r.show_select();

    //out << r->texture_backgroud_id() ;
    //out << r->texture_id();

    in >> r.render_brightness() ;
    in >> r.render_contrast();
    in >> r.render_hue();
    in >> r.render_saturation();

    in >> r.render_red();
    in >> r.render_green() ;
    in >> r.render_blue() ;
    in >> r.render_alpha();

    in >> r.fusion_brightness() ;
    in >> r.fusion_gamma() ;

    in >> tem; r.mode_draw() = (DrawMode)tem;
    in >> tem; r.mode_rend() = (RenderMode)tem;
}
#endif // RENDER_PROPERTY_H
