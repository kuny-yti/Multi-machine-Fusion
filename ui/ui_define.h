#ifndef UI_DEFINE
#define UI_DEFINE

#include "message.h"
#include <QByteArray>
#include <QDataStream>

typedef unsigned long ulong;
const static int  DefaultDeviceCount = 12;

#define VersionNumber(m,j,s) ((m << 16)| (j << 8)| s )
#define VersionStr(m, j, s) "_v" #m "." #j "." #s

#define FusionVersion() VersionNumber(1, 3, 1)
#define FusionVersionStr() VersionStr(1, 3, 1)

#define ProductName() "FusionSystem"
#define ProductIcon() ":/image/yantu.ico"
#define VendorName() "YanTu"
#define VendorIcon() ":/image/yantu.ico"

#define idTools(high, low) (((high << 8) &0xFF00) | (low & 0xFF))
#define idHigh(value) ((value >> 8) & 0xFF)
#define idLow(value) (value & 0xFF)

//! 命令类指令，在取反的状态为回应。
//! 工具栏命令
typedef enum
{
    Command_ComputerLayout= 1<< 4,//! 计算机布局
    Command_ComputerIndex,        //! 计算机索引
    Command_DeviceLayout,         //! 投影设备布局
    Command_Resolution,           //! 投影设备分辨率
    Command_Strategy,             //! 几何创建策略
    Command_StartDebug,           //! 启动拼接调试
    Command_StartOpenGL,

    Command_ControlPoint,         //! 设置几何控制点
    Command_MeshPrecision,        //! 设置几何网格精度
    Command_ModeDebug,            //! 几何调试模式设置
    Command_RenderingMode,        //! 渲染模式设置
    Command_SelectImage,          //! 内部图像选择
    Command_SelectOtherImage,     //! 外部图像选择
    Command_SelectSequenceCatalog,//! 选择序列图目录
    Command_SequenceSpeed,        //! 序列图播放速度
    Command_SelectMedia,          //! 选择媒体

    Command_OpenMedia,
    Command_PlayStop,             //! 播放停止
    Command_PauseContinue,        //! 播放继续
    Command_Volum,                //! 设置播放音量
    Command_PlayMode,             //! 设置播放模式
    Command_Duration,             //! 设置播放时长
    Command_Progress,             //! 设置播放进度

    Command_DeviceID,             //! 选择投影设备ID
    Command_Brightness,           //! 设置投影设备亮度
    Command_Contrast,             //! 设置投影设备对比度
    Command_Hue,                  //! 设置投影设备色调
    Command_Saturation,           //! 设置投影设备饱和度
    Command_Color,                //! 设置投影设备基色

    Command_AttachDebug,          //! 调用附加调试窗口
    Command_AttachDebugOff,
    Command_AttachDebugOn,
    Command_MoveAttach,           //! 移动附加窗口
    Command_AlphaAttach,          //! 附加窗口透明度
    Command_DockAttach,           //! 窗口的停靠设置

    Command_Administrator,        //! 设置GUI使用级别
    Command_ExitSystem,           //! 退出融合系统
    Command_HideTools,            //! 隐藏工具栏
    Command_ShowTools,            //! 显示工具栏
    Command_ShowDesktop,          //! 显示桌面
    Command_ReductionShow,        //! 还原显示

    Command_DecoderMode,
    Command_StartStopDesktop,
    Command_DesktopFrameRate,
    Command_DesktopCapture,

    Command_MovePixel,

    Command_CmdSelectMedia,

}ToolsCommand;

typedef enum
{
    Decoder_CPU = 1,
    Decoder_GPU,
    Decoder_OpenCL
}DecoderMode;

//! 调试窗口命令
typedef enum
{
    Command_BlendLeft = 1 << 16, //! 左混合设置
    Command_BlendRight,          //! 右混合设置
    Command_BlendTop,            //! 顶混合设置
    Command_BlendBottom,         //! 底混合设置

    Command_ComputerCut,         //! 更新计算机裁剪
    Command_DeviceCut,           //! 更新投影设备裁剪
}DebugCommand;

//! OpenGL窗口命令
typedef enum
{
    Command_EditSave = 1 << 24,  //! 编辑保存
    Command_EditLoad,            //! 编辑加载
    Command_EditStepSave,        //! 步骤保存
    Command_EditStepLoad,        //! 步骤加载
    Command_EditUndo,            //! 编辑后退一步
    Command_EditRedo,            //! 编辑前进一步
    Command_ConfigSave,
    Command_ConfigLoad,
    Command_GeometrySave,
    Command_GeometryLoad,
    Command_BlendSave,
    Command_BlendLoad,
    Command_ColorSave,
    Command_ColorLoad,
    Command_OtherSave,
    Command_OtherLoad,
    Command_SystemSave,
    Command_SystemLoad

}OpenGLCommand;
typedef enum
{
    Exit_ConfigNotSave = 1,
    Exit_GeometryNotSave = 1<<1,
    Exit_BlendNotSave = 1 <<2,
    Exit_ColorNotSave = 1<<3,
    Exit_OtherNotSave = 1<<4
}ExitNotSave;

typedef enum
{
    Error_StartDesktop = 1 << 26,
    Error_License,
}ErrorCommand;

typedef enum
{
    Image_AndSymbol,
    Image_Mesh,
    Image_Letter,
    Image_LogoColor,
    Image_LogoWriting,
    Image_Count
}LocalImage;
const static char* const LocalImageStr[Image_Count] =
{
    "'&' Symbol",
    "Mesh",
    "Letter",
    "LogoColor",
    "LogoWriting",
};
const static char* const LocalImagePath[Image_Count] =
{
    ":/image/symbol.png",
    ":/image/mesh.png",
    ":/image/letter.png",
    ":/image/yantucolor.png"
    ":/image/yantuwriting.png"
};

const static int EditStep = 10;

//! GUI界面使用模式
typedef enum
{
    Mode_Junior,     //! 初级模式
    Mode_Senior,     //! 高级模式
    Mode_Maintenance,//! 维护模式
}AdministratorMode;

//! 附加调试窗口方向
typedef enum
{
    Attach_Left,      //! 靠左边
    Attach_Right,     //! 靠右边
}AttachDirection;

//! 语言定义
typedef enum
{
    Language_Local,
    Language_English,
    Language_Chinese,
    Language_Count
}LanguageSupport;

const static char* const LanguageString[Language_Count] =
{
    "Local",
    "English",
    "Chinese"
};

///调整策略
typedef enum
{
    MeshMode_FFD = 0x00,
    MeshMode_MESH
}MeshMode;

///设备几何策略
typedef enum
{
    Strategy_Auto,
    Strategy_MouseSigle,
    Strategy_MouseMulit,
    Strategy_Count
}DeviceStrategy;

const static char* const DeviceStrategyStr[Strategy_Count] =
{
    "Auto",
    "MouseSigle",
    "MouseMulit"
};
typedef enum
{
    Resolution_VGA,
    Resolution_SVGA,
    Resolution_XGA ,
    Resolution_SXGA ,
    Resolution_SXGA_T ,
    Resolution_UXGA ,
    Resolution_WUXGA ,
    Resolution_QXGA ,
    Resolution_UWXGA ,
    Resolution_USXGA ,
    Resolution_QUXGA ,
    Resolution_WQUXGA ,
    Resolution_HD720_T ,
    Resolution_HD720 ,
    Resolution_HD1080_T ,
    Resolution_HD1080 ,
    Resolution_0_8K ,
    Resolution_1_3K ,
    Resolution_2K ,
    Resolution_4K ,
    Resolution_8K ,
    Resolution_IMAX ,
    Resolution_NTSC ,
    Resolution_PAL ,
    Resolution_PAL_T,
    Resolution_Count
}DeviceResolutionMode;

///设备分辨率
const static char* const DeviceResolutionStr[Resolution_Count] =
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
const static int DeviceResolution[Resolution_Count][2] =
{
    {640, 480},
    {800, 640},
    {1024,768},
    {1280,1024},
    {1400,1050},
    {1600,1200},
    {1920,1200},
    {2048,1536},
    {2560,1600},
    {2560,2048},
    {3200,2400},
    {3840,2400},
    {960, 720},
    {1280,720},
    {1440,1080},
    {1920,1080},
    {1024,768},
    {1280,1024},
    {2048,1080},
    {4096,2016},
    {7680,4320},
    {18000,13433},
    {720, 480},
    {720, 576},
    {768, 576}
};

typedef enum
{
    Rendering_Not = 0,
    Rendering_Grid = 1 ,
    Rendering_Image = 1<<1,
    Rendering_Media = 1<<2,
    Rendering_Desktop = 1<<3,
    Rendering_Blend = 1<<4,
    Rendering_Mesh = 1<<5,
    Rendering_Point = 1<<6
} RenderingMode;

typedef enum
{
    Play_Default,
    Play_Loop,
    Play_Command,
}MediaPlayMode;

typedef enum
{
    Blend_Left = 1,
    Blend_Right = 1 << 1,
    Blend_Top = 1 << 2,
    Blend_Bottom = 1 << 3
}BlendDirection;
 ///融合区域信息
typedef struct BlendArea
{
    int device_id;           /// 与其相交目标投影ID
    float area[4];           /// 融合区域 x,y,width,height
    float ratio[2];           /// 混合区
    struct BlendArea & operator = (const struct BlendArea & b)
    {
        device_id = b.device_id;
        area[0] = b.area[0];
        area[1] = b.area[1];
        area[2] = b.area[2];
        area[3] = b.area[3];
        ratio[0] = b.ratio[0];
        ratio[1] = b.ratio[1];
        return *this;
    }
    QByteArray serialize()const
    {
        QByteArray ser;
        QDataStream write(&ser, QIODevice::ReadWrite);

        write << device_id ;
        write << area[0];
        write << area[1];
        write << area[2];
        write << area[3];
        write << ratio[0];
        write << ratio[1];
        return ser;
    }
    bool unserialize(const QByteArray &in)
    {
        QByteArray ser = in;
        QDataStream read(&ser, QIODevice::ReadWrite);
        read >> device_id ;
        read >> area[0];
        read >> area[1];
        read >> area[2];
        read >> area[3];
        read >> ratio[0];
        read >> ratio[1];
        return true;
    }
}BlendArea;

typedef struct BlendEdge
{
    int device_id;
    float coord[4];  //源位置
    int dir:4;      ///BlendDirection, 1bit:left 2bit:right 3bit:top 4bit:bottom

    BlendArea edge[4];

    struct BlendEdge & operator = (const struct BlendEdge & b)
    {
        device_id = b.device_id;
        coord[0] = b.coord[0];
        coord[1] = b.coord[1];
        coord[2] = b.coord[2];
        coord[3] = b.coord[3];
        dir = b.dir;
        edge[0] = b.edge[0];
        edge[1] = b.edge[1];
        edge[2] = b.edge[2];
        edge[3] = b.edge[3];
        return *this;
    }
    QByteArray serialize()const
    {
        QByteArray ser;
        QDataStream write(&ser, QIODevice::ReadWrite);

        write << device_id;
        write << coord[0];
        write << coord[1];
        write << coord[2];
        write << coord[3] ;
        write << (int)dir;
        write << edge[0].serialize();
        write << edge[1].serialize();
        write << edge[2].serialize();
        write << edge[3].serialize();

        return ser;
    }
    bool unserialize(const QByteArray &in)
    {
        int bits;
        QByteArray temp;
        QByteArray ser = in;
        QDataStream read(&ser, QIODevice::ReadWrite);

        read >> device_id;
        read >> coord[0];
        read >> coord[1];
        read >> coord[2];
        read >> coord[3] ;
        read >> bits ;dir = bits;
        read >> temp; edge[0].unserialize(temp);
        read >> temp; edge[1].unserialize(temp);
        read >> temp; edge[2].unserialize(temp);
        read >> temp; edge[3].unserialize(temp);

        return true;
    }
}BlendEdge;

typedef struct BlendValue
{
    float area[4];
    float brightness;
    float exponent;
    float gamma;
    struct BlendValue & operator = (const struct BlendValue & b)
    {
        area[0] = b.area[0];
        area[1] = b.area[1];
        area[2] = b.area[2];
        area[3] = b.area[3];
        brightness = b.brightness;
        exponent = b.exponent;
        gamma = b.gamma;
        return *this;
    }
    QByteArray serialize()const
    {
        QByteArray ser;
        QDataStream write(&ser, QIODevice::ReadWrite);

        write << area[0] ;
        write << area[1];
        write << area[2] ;
        write << area[3] ;
        write << brightness ;
        write << exponent ;
        write << gamma ;

        return ser;
    }
    bool unserialize(const QByteArray &in)
    {
        QByteArray ser = in;
        QDataStream read(&ser, QIODevice::ReadWrite);

        read >> area[0] ;
        read >> area[1];
        read >> area[2] ;
        read >> area[3] ;
        read >> brightness ;
        read >> exponent ;
        read >> gamma ;

        return true;
    }
}BlendValue;
typedef struct BlendConfig
{
    int device_id;
    int edge:4;
    BlendValue left;
    BlendValue right;
    BlendValue top;
    BlendValue bottom;

    struct BlendConfig & operator = (const struct BlendConfig & b)
    {
        device_id = b.device_id;
        left = b.left;
        right = b.right;
        top = b.top;
        bottom = b.bottom;
        edge = b.edge;
        return *this;
    }

    QByteArray serialize()const
    {
        QByteArray ser;
        QDataStream write(&ser, QIODevice::ReadWrite);

        write << device_id ;
        write << (int)edge ;
        write << left.serialize();
        write << right.serialize();
        write << top.serialize();
        write << bottom.serialize();

        return ser;
    }
    bool unserialize(const QByteArray &in)
    {
        int bits;
        QByteArray temp;
        QByteArray ser = in;
        QDataStream read(&ser, QIODevice::ReadWrite);

        read >> device_id ;
        read >> bits; edge = bits;
        read >> temp;left.unserialize(temp);
        read >> temp;right.unserialize(temp);
        read >> temp;top.unserialize(temp);
        read >> temp;bottom.unserialize(temp);

        return true;
    }
}BlendConfig;

typedef struct ColorConfig
{
    int device_id;
    float red;
    float green;
    float blue;
    float alpha;
    float brightness;
    float contrast;
    float hue;
    float saturation;
    ColorConfig & operator = (const ColorConfig & b)
    {
        device_id = b.device_id;
        red= b.red;
        green= b.green;
        blue= b.blue;
        alpha= b.alpha;
        contrast= b.contrast;
        hue= b.hue;
        saturation= b.saturation;
        brightness = b.brightness;
        return *this;
    }

    QByteArray serialize()const
    {
        QByteArray ser;
        QDataStream write(&ser, QIODevice::ReadWrite);

        write << device_id;
        write << red;
        write << green;
        write << blue;
        write << alpha;
        write << brightness;
        write << contrast;
        write << hue;
        write << saturation;

        return ser;
    }
    bool unserialize(const QByteArray &in)
    {
        QByteArray ser = in;
        QDataStream read(&ser, QIODevice::ReadWrite);

        read >> device_id;
        read >> red;
        read >> green;
        read >> blue;
        read >> alpha;
        read >> brightness;
        read >> contrast;
        read >> hue;
        read >> saturation;

        return true;
    }

}ColorConfig;

typedef struct
{
    float widget_alpha;
    //! 几何调试模式
    MeshMode adjust_mode;//-

    int display_mode:8;//- RenderingMode
    //! 使用者模式
    AdministratorMode admin_mode;

    //! 解码器模式
    DecoderMode decoder_mode;//-
    //! 步骤保存用
    char data_step_save[1024];//-
    int data_step_index;//-

    //! 图片选择
    int  media_image_mode:2;//- 1:local 2:other
    char media_image_local_select[1024];
    char media_image_other_select[1024];
    int media_image_local_index;//-
    int media_image_other_index;//-

    //! 序列图选择
    char media_sequence_dir[1024];
    int media_sequence_index;//-
    int media_sequence_speed;//-

    //! 媒体选择和音量
    char media_video_select[1024];
    int media_video_index;//-
    float media_volum;//-
    int media_state:2; // 1bit:play/stop 2bit:pause/continue

    //! 播放模式
    MediaPlayMode media_play_mode;//-
    //! 媒体时长和进度
    ulong media_duration;
    ulong media_progress;

    //! 混合配置
    BlendConfig blend_config;////未写保存
    ColorConfig color_config;
    int color_device_id:16;//-
    int adjust_device_id:16;//-

    //! 桌面采样率
    uint desktop_frame_rate;//-

/// config
    char logs_filename[32];
    int logs_level;
    char dir_config[32];//-
    char dir_config_language[64];//-
    char dir_config_log[64];//-
    char dir_config_media[64];//-
    char dir_config_data[64];//-
    char dir_config_capture[64];//-

    char dir_config_capture_desktop[128];//-
    char dir_config_capture_media[128];//-
    char dir_config_capture_device[128];//-
    char dir_config_data_step[128];//-
    char dir_config_media_image[128];//-
    char dir_config_media_sequence[128];//-
    char dir_config_media_video[128];//-

    //! 计算机和设备布局
    int computer_layout:16;//-
    int computer_index:16;//-
    int device_layout:16;//-

    //! 几何控制点和精度
    int geometry_control:16;//-
    int geometry_precision:16;//-

    //! 几何创建策略
    DeviceStrategy device_strategy;//-
    //! 设备分辨率
    DeviceResolutionMode device_resolution;//-

    //! 系统使用的语言
    char system_language[32];//-
    int system_language_index;
    int system_setup:4; //- 1bit:随系统启动 2bit:启动最小化 3bit:映射鼠标
    int display_window[4];//- 融合输出窗口几何

    uint data_update;
    int adjust_move_pixel;
}RawConfig;


#endif // UI_DEFINE

