#ifndef FFD_PROPERTY_H
#define FFD_PROPERTY_H

#include "type_define.h"

///
/// \brief The ffd_property class
/// 1.颜色属性
/// 2.点线大小
/// 3.各种模式
/// 4.纹理坐标范围
/// 5.当前参数
///
class ffd_property
{
public:
    ffd_property();

    inline QColor &color_vertex() {return _c_vertex;}
    inline QColor &color_ctrl() {return _c_ctrl;}
    inline QColor &color_vertex_select() {return _c_vertex_select;}
    inline QColor &color_ctrl_select() {return _c_ctrl_select;}
    inline QColor &color_cross_line() {return _c_cross_line;}
    inline QColor &color_center_point() {return _c_center_point;}
    inline QColor &color_center_circle() {return _c_center_circle;}
    inline QColor &color_four_circle() {return _c_four_circle;}
    inline QColor &color_border_line() {return _c_border_line;}
    inline QColor &color_ctrl_line() {return _c_ctrl_line;}
    inline QColor &color_backgroud() {return _c_backgroud;}
    inline uint &number_cross_line() {return _n_cross_line;}

    inline float &size_point() {return _s_point;}
    inline float &size_point_select() {return _s_point_select;}
    inline float &size_line() {return _s_line;}
    inline QColor &color_line() {return _c_line;}
    inline float &size_ctrl_line() {return _s_ctrl_line;}
    inline float &size_accuracy() {return _s_acc;}

    inline CtrlMode &mode_ctrl() {return _m_ctrl;}
    inline SelectMode &mode_select(){return _m_select;}
    inline AdjustMode &mode_adjust(){return _m_adjust;}
    inline FusionDir &fusion_dir() {return _dir;}

    inline QVector2D &coord_min(){return _t_min;}
    inline QVector2D &coord_max(){return _t_max;}

    inline PorjectParam &param() {return _param;}

    inline ffd_property *operator()(){return this;}
protected:
    QColor _c_vertex;       /// 顶点颜色
    QColor _c_ctrl;         /// 控制点颜色
    QColor _c_vertex_select;/// 顶点选中颜色
    QColor _c_ctrl_select;  /// 控制点选中颜色
    QColor _c_cross_line;   /// 交叉线颜色
    QColor _c_center_point; /// 中心点颜色
    QColor _c_center_circle;/// 中心圆颜色
    QColor _c_four_circle;  /// 四角圆颜色
    QColor _c_border_line;  /// 边界线颜色
    QColor _c_ctrl_line;    /// 控制线颜色
    QColor _c_line;         /// 顶点线颜色
    QColor _c_backgroud;    /// 背景颜色
    uint _n_cross_line;     /// 交叉线数量

    float _s_point;         /// 点大小
    float _s_point_select;
    float _s_line;          /// 线宽度
    float _s_ctrl_line;     /// 线宽度
    float _s_acc;

    CtrlMode _m_ctrl ;  //当前使用的控制方式(ffd和mesh)
    SelectMode _m_select;
    AdjustMode _m_adjust;
    FusionDir _dir; /// 所属方向

    QVector2D  _t_min;   /// 纹理坐标(范围)
    QVector2D  _t_max;

    PorjectParam _param;/// 当前面基本参数
};

inline ffd_property::ffd_property()
{
    memset (&_param, 0, sizeof(PorjectParam));
    _param._v_count_row = 2;
    _param._v_count_col = 2;
    _param._c_count_row = 2;
    _param._c_count_col = 2;

    _t_min  = QVector2D(0,0);
    _t_max  = QVector2D(1,1);

    _m_ctrl = CTRL_FFD;
    _m_select = SELECT_NO;
    _m_adjust = ADJUST_MOUSE;
    _dir = FUSION_NO;

    _c_vertex = QColor(200, 0, 0);       /// 顶点颜色
    _c_ctrl = QColor(200, 0, 0);         /// 控制点颜色
    _c_vertex_select = QColor(0, 200, 0);/// 顶点选中颜色
    _c_ctrl_select = QColor(0, 0, 200);  /// 控制点选中颜色
    _c_cross_line = QColor(0, 0, 0);   /// 交叉线颜色
    _c_center_point = QColor(0, 200, 200); /// 中心点颜色
    _c_center_circle = QColor(0, 0, 0);/// 中心圆颜色
    _c_four_circle = QColor(0, 0, 0);  /// 四角圆颜色
    _c_border_line = QColor(200, 200, 0);  /// 边界线颜色
    _c_ctrl_line = QColor(200, 0, 200);    /// 控制线颜色
    _c_backgroud = QColor(0, 0, 0);    /// 背景颜色
    _c_line = QColor(255, 255, 255);

    _n_cross_line = 2;     /// 交叉线数量
    _s_point = 6.;          /// 点大小
    _s_point_select = 8.;
    _s_line = 1.;           /// 线宽度
    _s_ctrl_line = 2.;
    _s_acc = 0.001;
}

#endif // FFD_PROPERTY_H
