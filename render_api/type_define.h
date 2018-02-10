#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H

#include <QtGlobal>
#include <QColor>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QPoint>
#include <QSizeF>
#include <QRectF>
#include <cmath>
#include <QDebug>

typedef enum
{
    FILTER_NEAREST = 0x2600,
    FILTER_LINEAR,
    WRAP_CLAMP = 0x2900,
    WRAP_REPEAT,
    WRAP_CLAMP_TO_EDGE =  0x812F
}TextureParameter;

typedef enum
{
    UI_NULL = 0,
    UI_GRID = 1 ,
    UI_IMAGE = 1<<1,
    UI_MEDIA = 1<<2,
    UI_DESKTOP = 1<<3,
    UI_ZONE = 1<<4,
    UI_MESH = 1<<5,
    UI_POINT = 1<<6
} RenderModeUi;
typedef enum
{
    DATA_BYTE = 0x1400,
    DATA_UNSIGNED_BYTE,
    DATA_SHORT,
    DATA_UNSIGNED_SHORT,
    DATA_INT,
    DATA_UNSIGNED_INT,
    DATA_FLOAT,
    DATA_2_BYTES ,
    DATA_3_BYTES ,
    DATA_4_BYTES,
    DATA_DOUBLE ,
    DATA_UNSIGNED_BYTE_3_3_2 = 0x8032,
    DATA_UNSIGNED_SHORT_4_4_4_4,
    DATA_UNSIGNED_SHORT_5_5_5_1,
    DATA_UNSIGNED_INT_8_8_8_8 ,
    DATA_UNSIGNED_INT_10_10_10_2,
    DATA_UNSIGNED_BYTE_2_3_3_REV  = 0x8362,
    DATA_UNSIGNED_SHORT_5_6_5,
    DATA_UNSIGNED_SHORT_5_6_5_REV,
    DATA_UNSIGNED_SHORT_4_4_4_4_REV,
    DATA_UNSIGNED_SHORT_1_5_5_5_REV,
    DATA_UNSIGNED_INT_8_8_8_8_REV
}DataType;
typedef enum
{
    DATA_NO = 0,
    DATA_RGB = 0x1907,
    DATA_RGBA = 0x1908,
    DATA_LUMINANCE,
    DATA_LUMINANCE_ALPHA,
    DATA_BGR = 0x80E0,
    DATA_BGRA,
}DataFormat;

typedef enum
{
    PIXEL_RED = 0x1903,
    PIXEL_GREEN,
    PIXEL_BLUE,
    PIXEL_ALPHA,
    PIXEL_RGB,
    PIXEL_RGBA,
    PIXEL_LUMINANCE,
    PIXEL_LUMINANCE_ALPHA,
    PIXEL_ALPHA4 = 0x803B,
    PIXEL_ALPHA8 ,
    PIXEL_ALPHA12 ,
    PIXEL_ALPHA16 ,
    PIXEL_LUMINANCE4 ,
    PIXEL_LUMINANCE8 ,
    PIXEL_LUMINANCE12 ,
    PIXEL_LUMINANCE16 ,
    PIXEL_LUMINANCE4_ALPHA4 ,
    PIXEL_LUMINANCE6_ALPHA2 ,
    PIXEL_LUMINANCE8_ALPHA8 ,
    PIXEL_LUMINANCE12_ALPHA4 ,
    PIXEL_LUMINANCE12_ALPHA12 ,
    PIXEL_LUMINANCE16_ALPHA16 ,
    PIXEL_INTENSITY ,
    PIXEL_INTENSITY4 ,
    PIXEL_INTENSITY8 ,
    PIXEL_INTENSITY12 ,
    PIXEL_INTENSITY16 ,
    PIXEL_R3_G3_B2 = 0x2A10,
    PIXEL_RGB4 = 0x804F,
    PIXEL_RGB5 ,
    PIXEL_RGB8 ,
    PIXEL_RGB10 ,
    PIXEL_RGB12 ,
    PIXEL_RGB16 ,
    PIXEL_RGBA2 ,
    PIXEL_RGBA4 ,
    PIXEL_RGB5_A1,
    PIXEL_RGBA8 ,
    PIXEL_RGB10_A2 ,
    PIXEL_RGBA12 ,
    PIXEL_RGBA16 ,
}PixelFormat;

typedef enum
{
    SHADER_FRAGMENT = 0x8B30,
    SHADER_VERTEX
}ShaderType;

typedef enum
{
    MATRIX_MODELVIEW = 0x1700,
    MATRIX_PROJECTION
}MatrixMode;

typedef enum
{
    ENABLE_BLEND = 0x0BE2,
    ENABLE_ALPHA_TEST =0x0BC0,
    ENABLE_DEPTH_TEST = 0x0B71,
    ENABLE_TEXTURE_2D = 0x0DE1
}Enable;
typedef enum
{
    DISABLE_ALPHA_TEST =0x0BC0,
    DISABLE_DEPTH_TEST = 0x0B71,
    DISABLE_BLEND = 0x0BE2,
    DISABLE_CULL_FACE = 0x0B44,
    DISABLE_CULL_FACE_MODE,
    DISABLE_FRONT_FACE,
    DISABLE_LIGHTING,
    DISABLE_TEXTURE_2D = 0x0DE1,
}Disable;
typedef enum
{
    ALPHA_NEVER =0x0200,
    ALPHA_LESS ,
    ALPHA_EQUAL ,
    ALPHA_LEQUAL ,
    ALPHA_GREATER ,
    ALPHA_NOTEQUAL ,
    ALPHA_GEQUAL ,
    ALPHA_ALWAYS
}AlphaFunc;

typedef enum
{
    BLEND_ZERO =0,
    BLEND_ONE =1,
    BLEND_SRC_COLOR =0x0300,
    BLEND_ONE_MINUS_SRC_COLOR =0x0301,
    BLEND_SRC_ALPHA =0x0302,
    BLEND_ONE_MINUS_SRC_ALPHA =0x0303,
    BLEND_DST_ALPHA =0x0304,
    BLEND_ONE_MINUS_DST_ALPHA =0x0305
}BlendFunc;

typedef enum
{
    BIND_TEXTURE_1D = 0x0DE0,
    BIND_TEXTURE_2D = 0x0DE1
}Bind;

typedef enum
{
    DEPTH_NEVER = 0x0200,
    DEPTH_LESS ,
    DEPTH_EQUAL ,
    DEPTH_LEQUAL ,
    DEPTH_GREATER,
    DEPTH_NOTEQUAL ,
    DEPTH_GEQUAL ,
    DEPTH_ALWAYS ,
}DepthFunc;

typedef enum
{
    MULTI_TEXTURE_0 = 0x84C0,
    MULTI_TEXTURE_1 ,
    MULTI_TEXTURE_2 ,
    MULTI_TEXTURE_3 ,
    MULTI_TEXTURE_4 ,
    MULTI_TEXTURE_5 ,
    MULTI_TEXTURE_6,
    MULTI_TEXTURE_7,
    MULTI_TEXTURE_8,
    MULTI_TEXTURE_9,
    MULTI_TEXTURE_10
}TextureMulti;

typedef enum
{
    EYE_NO,
    EYE_LEFT,
    EYE_RIGHT
}Eye;

typedef enum
{
    RENDER_RENDER =0x1C00,
    RENDER_FEEDBACK ,
    RENDER_SELECT
}RenderMode;///渲染模式

typedef enum
{
    POLYGON_POINT = 0x1B00,
    POLYGON_LINE ,
    POLYGON_FILL ,
}PolygonMode;

typedef enum
{
    DRAW_POINT = 1,
    DRAW_LINE = 1 << 1,
    DRAW_FILL = 1 << 2,

    DRAW_MULTI = 1 << 3,
    DRAW_POINT_AND_LINE = DRAW_POINT | DRAW_LINE,
    DRAW_POINT_AND_FILL = DRAW_POINT | DRAW_FILL,
    DRAW_LINE_AND_FILL = DRAW_LINE | DRAW_FILL,
    DRAW_POINT_LINE_FILL = DRAW_POINT | DRAW_LINE | DRAW_FILL,
    DRAW_MULTI_POINT_AND_LINE = DRAW_MULTI | DRAW_POINT | DRAW_LINE,
}DrawMode;///绘制模式

typedef enum
{
    SURFACE_FRONT_LEFT = 0x0400,
    SURFACE_FRONT_RIGHT ,
    SURFACE_BACK_LEFT ,
    SURFACE_BACK_RIGHT,
    SURFACE_FRONT ,
    SURFACE_BACK ,
    SURFACE_LEFT ,
    SURFACE_RIGHT ,
    SURFACE_FRONT_AND_BACK
}SurfaceMode;///面模式

typedef enum
{
    Key_Fit                 = 0x01200002,///适合
    Key_TopView             = 0x01200003,///顶视图
    Key_LeftView            = 0x01200004,///左视图
    Key_RightView           = 0x01200005,///右视图
    Key_FrontView           = 0x01200006,///前视图
    Key_BottomView          = 0x01200007,///底视图
    Key_BackView            = 0x01200008,///后视图
    Key_RotateCW90          = 0x01200009,
    Key_RotateCCW90         = 0x0120000a,
    Key_ISO1                = 0x0120000b,
    Key_ISO2                = 0x0120000c,
    Key_Button1             = 0x0120000d,
    Key_Button2             = 0x0120000e,
    Key_Button3             = 0x0120000f,
    Key_Button4             = 0x01200010,
    Key_Button5             = 0x01200011,
    Key_Button6             = 0x01200012,
    Key_Button7             = 0x01200013,
    Key_Button8             = 0x01200014,
    Key_Button9             = 0x01200015,
    Key_Button10            = 0x01200016,
    Key_Rotations           = 0x0120001b,///旋转
    Key_Translations        = 0x0120001c,///平移
    Key_DominantAxis        = 0x0120001d,///主坐标轴
    Key_IncreaseSensitivity = 0x0120001e,///增加灵敏度
    Key_DecreaseSensitivity = 0x0120001f,///降低灵敏度
    Key_Move                = 0x01200020,
    Key_RectSelect          = 0x01200021,///框选
    Key_Select              = 0x01200022,///选择物体
    Key_Group               = 0x01200023,///选择物体
    Key_Create              = 0x01200023,///选择物体

    Key_No                  = 0x012000ff
}Mouse3DKeys;///

typedef enum
{
    ADJUST_KEYBOARD,
    ADJUST_MOUSE,
    ADJUST_NETWORK
} AdjustMode;///调试模式

typedef enum
{
    SELECT_POINT,
    SELECT_HORIZONTAL,
    SELECT_VERTICAL,
    SELECT_BLOCK,
    SELECT_SURFACK,
    SELECT_NO
} SelectMode;///选择模式

typedef enum
{
    Perspective,
    Orthographic
} ProjectionType;///投影类型

typedef enum
{
    TiltPanRoll,
    TiltRollPan,
    PanTiltRoll,
    PanRollTilt,
    RollTiltPan,
    RollPanTilt
}RotateOrder;///旋转规则

typedef enum
{
    CTRL_FFD = 0x00,
    CTRL_MESH
}CtrlMode;///调整策略

typedef enum
{
    CREATE_AUTO,
    CREATE_MOUSE_SINGLE,
    CREATE_MOUSE_MULIT,
    CREATE_NO
}CreatePolicy;///创建面的策略

const static QString ResolutionStr[] =
{
    "VGA (640×480)",
    "SVGA (800×640)",
    "XGA (1024×768)",
    "SXGA (1280×1024)",
    "SXGA+ (1400×1050)",
    "UXGA (1600×1200)",
    "WUXGA (1920×1200)",
    "QXGA (2048×1536)",
    "UWXGA (2560×1600)",
    "USXGA (2560×2048)",
    "QUXGA (3200×2400)",
    "WQUXGA (3840×2400)",
    "HD720 (960x720)",
    "HD720 (1280x720)",
    "HD1080 (1440x1080)",
    "HD1080 (1920x1080)",
    "0.8K (1024×768)",
    "1.3K (1280×1024)",
    "2K (2048×1080)",
    "4K (4096×2016)",
    "8K (7680x4320)",
    "IMAX (18000x13433)",
    "NTSC (720x480)",
    "PAL (720x576)",
    "PAL+ (768×576)"
};
const static QSize Resolution[] =
{
    QSize(640,480),
    QSize(800,640),
    QSize(1024,768),
    QSize(1280,1024),
    QSize(1400,1050),
    QSize(1600,1200),
    QSize(1920,1200),
    QSize(2048,1536),
    QSize(2560,1600),
    QSize(2560,2048),
    QSize(3200,2400),
    QSize(3840,2400),
    QSize(960,720),
    QSize(1280,720),
    QSize(1440,1080),
    QSize(1920,1080),
    QSize(1024,768),
    QSize(1280,1024),
    QSize(2048,1080),
    QSize(4096,2016),
    QSize(7680,4320),
    QSize(18000,13433),
    QSize(720,480),
    QSize(720,576),
    QSize(768,576)
};
typedef enum
{
    RESOL_VGA,
    RESOL_SVGA,
    RESOL_XGA ,
    RESOL_SXGA ,
    RESOL_SXGA_T ,
    RESOL_UXGA ,
    RESOL_WUXGA ,
    RESOL_QXGA ,
    RESOL_UWXGA ,
    RESOL_USXGA ,
    RESOL_QUXGA ,
    RESOL_WQUXGA ,
    RESOL_HD720_T ,
    RESOL_HD720 ,
    RESOL_HD1080_T ,
    RESOL_HD1080 ,
    RESOL_0_8K ,
    RESOL_1_3K ,
    RESOL_2K ,
    RESOL_4K ,
    RESOL_8K ,
    RESOL_IMAX ,
    RESOL_NTSC ,
    RESOL_PAL ,
    RESOL_PAL_T
}ResolutionType;

typedef enum
{
    FUSION_NO = 0,

    FUSION_LEFT = 1,
    FUSION_RIGHT = 1 << 1,
    FUSION_TOP = 1 << 2,
    FUSION_BOTTOM = 1 << 3,

    FUSION_TOP_LEFT = FUSION_TOP | FUSION_LEFT,
    FUSION_TOP_RIGHT = FUSION_TOP | FUSION_RIGHT,
    FUSION_BOTTOM_LEFT = FUSION_BOTTOM | FUSION_LEFT,
    FUSION_BOTTOM_RIGHT = FUSION_BOTTOM | FUSION_RIGHT,
    FUSION_TOP_BOTTOM = FUSION_TOP | FUSION_BOTTOM,
    FUSION_LEFT_RIGHT = FUSION_LEFT | FUSION_RIGHT,
    FUSION_LEFT_RIGHT_TOP = FUSION_LEFT | FUSION_RIGHT | FUSION_TOP ,
    FUSION_LEFT_RIGHT_BOTTOM = FUSION_LEFT | FUSION_RIGHT | FUSION_BOTTOM ,
    FUSION_LEFT_RIGHT_TOP_BOTTOM = FUSION_LEFT | FUSION_RIGHT | FUSION_BOTTOM | FUSION_TOP
}FusionDir;///混合图像朝向

typedef enum
{
    ARITH_NO,
    ARITH_LINEAR,///线性算法(边对边生成)
    ARITH_LINEAR_DIAGONAL,///线性算法(对角生成)
    ARITH_NONLINEAR,///非线性算法(边对边生成)
    ARITH_NONLINEAR_DIAGONAL,///非线性算法(对角生成)
}FuisonArithmetic ;///混合图像生成算法

typedef enum
{
    CTRL_REPLAY_STOP, ///重播、停止
    CTRL_PLAY_PAUSE, ///播放、暂停
    CTRL_FORWARD,   ///快进
    CTRL_BACKWARD,  ///快退
    CTRL_NEXT_FRAME,///下一帧
    CTRL_LAST_FRAME,///上一帧
    CTRL_VOLUME,///音量
    CTRL_MUTE,///静音
    CTRL_SEEK,///移动
    CTRL_VOLUME_PLUS = 1<<6,
    CTRL_VOLUME_MINUS = 1<<7
}PlayerCtrl;///播放器控制

typedef enum
{
    CTRL_DESKTOP_STOP,
    CTRL_DESKTOP_START,
    CTRL_DESKTOP_CAPTURE
}DesktopCtrl;///播放器控制

typedef enum
{
    COLOR_BRGHTNESS, ///亮度
    COLOR_CONTRAST, ///对比度
    COLOR_HUE,   ///色调
    COLOR_SATURATION,  ///饱和度
    COLOR_RED,///红基色
    COLOR_GREEN,///绿基色
    COLOR_BLUE,///蓝基色
    COLOR_ALPHA,///透明通道
}ColorAdjust;///颜色调整

typedef struct
{
    int _pc_count_row;   ///计算机
    int _pc_count_col;
    int _pc_count_row_c;   ///当前计算机
    int _pc_count_col_c;
    int _p_count_row;   ///投影
    int _p_count_col;
    int _c_count_row;  /// 控制点
    int _c_count_col;
    int _v_count_row;  /// 顶点
    int _v_count_col;
    CreatePolicy _policy;
    ResolutionType _resol;
}PorjectParam ;///全局几何生成信息

typedef struct
{
    QRectF area;
    float brightness;
    float gamma;
    float exponent;
}FusionZone;

typedef struct
{
    uint pro_id;
    FusionZone left;
    FusionZone right;
    FusionZone top;
    FusionZone bottom;
}ProjectionFusionZone;

typedef struct
{
    uint pro_id;
    FusionZone all_zonne;
    ProjectionFusionZone all;
    ProjectionFusionZone zone[12];
}ProjectionFusion;

typedef struct
{
    uint pro_id;
    float brightness;
    float contrast;
    float hue;
    float saturation;
    float red;
    float green;
    float blue;
    float alpha;
}ProjectionColor;

typedef struct
{
    uint id;            /// 与其相交目标投影ID
    QRectF area;  //融合区域
    QSizeF ratio;       ///混合区尺寸
    FuisonArithmetic arith;///当前区混合生成算法
    uchar *data;        ///当前区混合数据
}FusionArea;        ///融合区域信息

typedef struct
{
    uint id;
    QRectF coord;  //源位置
    FusionDir dir;      ///当前区混合方向
    FusionArea edge[4];
}EdgeFusion;

typedef struct
{
    QSizeF ratio;
    int   pixel;
    uchar *data;
}Image;///图像信息

typedef struct
{
    QVector3D vertex;
    QVector3D normal;
    QVector2D coord;
    QColor color;
}Primitive;///基本图元

typedef struct
{
    Primitive p0;
    Primitive p1;
}Line;///线

typedef struct
{
    Primitive p0;
    Primitive p1;
    Primitive p2;
}Triangle;///三角形

typedef struct
{
    Triangle t0;
    Triangle t1;
}Quads;///四边形


typedef QVector <uint> arrayUint;
typedef QVector <arrayUint> arrayUinttwo;

typedef QVector <QVector2D> array2D;
typedef QVector <array2D> array2Dtwo;

typedef QVector <QVector3D> array3D;
typedef QVector <array3D> array3Dtwo;

typedef QVector <Line> arrayLine;
typedef QVector <arrayLine> arrayLineTwo;

typedef QVector <Primitive> arrayPrimitive;
typedef QVector <arrayPrimitive> arrayPrimitiveTwo;

typedef QVector <Triangle> arrayTriangle;
typedef QVector <arrayTriangle> arrayTriangleTwo;

typedef QVector <Quads> arrayQuads;
typedef QVector <arrayQuads> arrayQuadsTwo;

class ffd_surface;
typedef QVector<ffd_surface> arrayFFD;
typedef QVector<arrayFFD > arrayFFDtwo;

class ffd_render;
typedef QVector<ffd_render> arrayFFDRend;
typedef QVector<arrayFFDRend > arrayFFDRendTwo;

class ffd_group;
typedef QVector<ffd_group> arrayFFDGroup;
typedef QVector<arrayFFDGroup > arrayFFDGroupTwo;


typedef QVector <QRectF> arrayRectF;
typedef QVector <arrayRectF> arrayRectFtwo;

typedef QVector <EdgeFusion> arrayEdge;
typedef QVector <arrayEdge> arrayEdgeTwo;

typedef QVector<ProjectionFusionZone> arrayFusionZone;
typedef QVector<arrayFusionZone> arrayFusionZoneTwo;

typedef QVector<ProjectionFusion> arrayProjectionFusion;
typedef QVector<arrayProjectionFusion> arrayProjectionFusionTwo;


typedef QVector<ProjectionColor> arrayColor;
typedef QVector<arrayColor> arrayColorTwo;

class ffd_multi;
typedef QVector<ffd_multi> arrayFFDMulti;
typedef QVector<arrayFFDMulti> arrayFFDMultiTwo;

#ifndef PI
    #define PI       3.14159265358979323846
#endif

#ifndef TWO_PI
    #define TWO_PI   6.28318530717958647693
#endif

#ifndef M_TWO_PI
    #define M_TWO_PI   6.28318530717958647693
#endif

#ifndef FOUR_PI
    #define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
    #define HALF_PI  1.57079632679489661923
#endif
#ifndef DEG_TO_RAD
    #define DEG_TO_RAD (PI/180.0)
#endif

#ifndef RAD_TO_DEG
    #define RAD_TO_DEG (180.0/PI)
#endif

#ifndef MIN
    #define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
    #define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
    #define CLAMP(val,min,max) ((val) < (min) ? (min) : ((val > max) ? (max) : (val)))
#endif

#ifndef ABS
    #define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

float wrap(float value, float from, float to);
float deg_rad(float degrees);
float rad_deg(float radians);
float clamp(float value, float min, float max);
float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp= false);
float lerp(float start, float stop, float amt) ;
#endif // TYPE_DEFINE_H
