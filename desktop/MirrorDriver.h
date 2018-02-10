#ifndef QMIRRORDRIVER_H
#define QMIRRORDRIVER_H

#include <windows.h>
#include <qglobal.h>
#include "RegistryKey.h"
#include "Threads.h"
#include <QDebug>


//namespace df_mirage_drv
//{

enum
{
    DMF_ESCAPE_BASE_1_VB	= 1030,
    DMF_ESCAPE_BASE_2_VB	= 1026,
    DMF_ESCAPE_BASE_3_VB	= 24
};

//VB++
#ifdef  _WIN64

#define CLIENT_64BIT   0x8000

enum
{
    DMF_ESCAPE_BASE_1	= CLIENT_64BIT | DMF_ESCAPE_BASE_1_VB,
    DMF_ESCAPE_BASE_2	= CLIENT_64BIT | DMF_ESCAPE_BASE_2_VB,
    DMF_ESCAPE_BASE_3	= CLIENT_64BIT | DMF_ESCAPE_BASE_3_VB,
};

#else

enum
{
    DMF_ESCAPE_BASE_1	= DMF_ESCAPE_BASE_1_VB,
    DMF_ESCAPE_BASE_2	= DMF_ESCAPE_BASE_2_VB,
    DMF_ESCAPE_BASE_3	= DMF_ESCAPE_BASE_3_VB,
};

#endif

typedef enum
{
    dmf_esc_qry_ver_info	= DMF_ESCAPE_BASE_2+ 0,

    dmf_esc_usm_pipe_map	= DMF_ESCAPE_BASE_1+ 0,		// create the R3 mapping of update-stream pipe
    dmf_esc_usm_pipe_unmap	= DMF_ESCAPE_BASE_1+ 1,	// release the mapping; deprecated (read docs)

    dmf_esc_test = DMF_ESCAPE_BASE_1+ 20,
    dmf_esc_usm_pipe_mapping_test = DMF_ESCAPE_BASE_1+ 21,

// 1.0.9.0
    dmf_esc_pointer_shape_get = DMF_ESCAPE_BASE_3,

} dmf_escape;

#define MAXCHANGES_BUF	20000
#define CLIP_LIMIT		50

// operations
typedef enum
{
    dmf_dfo_IGNORE		= 0,
    dmf_dfo_FROM_SCREEN = 1,
    dmf_dfo_FROM_DIB	= 2,
    dmf_dfo_TO_SCREEN	= 3,

    dmf_dfo_SCREEN_SCREEN	= 11,
    dmf_dfo_BLIT		= 12,
    dmf_dfo_SOLIDFILL	= 13,
    dmf_dfo_BLEND		= 14,
    dmf_dfo_TRANS		= 15,
    dmf_dfo_PLG			= 17,
    dmf_dfo_TEXTOUT		= 18,

    dmf_dfo_Ptr_Shape	= 19,
    dmf_dfo_Ptr_Engage	= 48,	// point is used with this record
    dmf_dfo_Ptr_Avert	= 49,

    dmf_dfn_assert_on	= 64,	// DrvAssert(TRUE): PDEV reenabled
    dmf_dfn_assert_off	= 65,	// DrvAssert(FALSE): PDEV disabled

} dmf_UpdEvent;

#define NOCACHE 1
#define OLDCACHE 2
#define NEWCACHE 3


struct	CHANGES_RECORD
{
    ULONG	type;		//screen_to_screen, blit, newcache,oldcache
    RECT	rect;
#ifndef DFMIRAGE_LEAN
    RECT	origrect;
    POINT	point;
    ULONG	color;		// number used in cache array
    ULONG	refcolor;	// slot used to pass bitmap data
#endif
};
// sizeof (CHANGES_RECORD) = 52, 8-aligned: 56

typedef CHANGES_RECORD *PCHANGES_RECORD;

struct	CHANGES_BUF
{
    ULONG	counter;
    CHANGES_RECORD	pointrect[MAXCHANGES_BUF];
};
// 4+ 56*2000 = 112004

#define	DMF_PIPE_SEC_SIZE_DEFAULT	ALIGN64K(sizeof(CHANGES_BUF))

struct GETCHANGESBUF
{
     CHANGES_BUF *	buffer;
     PVOID	Userbuffer;
};

typedef	enum
{
    dmf_sprb_internal_error			= 0x0001,	// non-specific fatal trap
    dmf_sprb_miniport_gen_error		= 0x0004,	// non-specific miniport error
    dmf_sprb_memory_alloc_failed	= 0x0008,	// ...and there were no recover (trap)

    dmf_sprb_pipe_buff_overflow		= 0x0010,
    dmf_sprb_pipe_buff_insufficient	= 0x0020,	// too large blob post attempted. need bigger buffer
    dmf_sprb_pipe_not_ready			= 0x0040,

    dmf_sprb_gdi_err				= 0x0100,	// internal error attributed to GDI

    dmf_sprb_owner_died				= 0x0400,	// owner app died; emergency cleanup have been performed
#define	dmf_sprb_ERRORMASK			0x07ff

    dmf_sprb_tgtwnd_gone			= 0x0800,	// target wnd is gone; nothing to capture

#define	dmf_sprb_STRICTSESSION_AFF	0x1fff

    dmf_sprb_pdev_detached			= 0x2000,	// DrvAssertMode: FALSE; transient problem, in general

} dmf_session_prob_status;

#define	DMF_ESC_RET_FAILF		0x80000000

#define	DMF_ESC_RET_SSTMASK		0x0000FFFF

#define	DMF_ESC_RET_IMMMASK		0x7FFF0000

typedef	enum
{
    dmf_escret_generic_ok		= 0x00010000,	// everything is hunky dory 8;-)

    dmf_escret_bad_state		= 0x00100000,
    dmf_escret_access_denied	= 0x00200000,	// not yet connected or a left guy
    dmf_escret_bad_buffer_size	= 0x00400000,
    dmf_escret_internal_err		= 0x00800000,

    dmf_escret_out_of_memory	= 0x02000000,

    dmf_escret_already_connected= 0x04000000,
    dmf_escret_oh_boy_too_late	= 0x08000000,	// i'm already torn ;))
    dmf_escret_bad_window		= 0x10000000,

    dmf_escret_drv_ver_higher	= 0x20000000,	// drv ver higher than app
    dmf_escret_drv_ver_lower	= 0x40000000,	// app ver higher than drv


} dmf_esc_retcode;

struct	Esc_dmf_Qvi_IN
{
    ULONG	cbSize;

    ULONG	app_actual_version;
    ULONG	display_minreq_version;

    ULONG	connect_options;		// reserved. must be 0.
};

enum
{
    esc_qvi_prod_name_max	= 16,
};

#define	ESC_QVI_PROD_MIRAGE	"MIRAGE"
#define	ESC_QVI_PROD_QUASAR	"QUASAR"

struct	Esc_dmf_Qvi_OUT
{
    ULONG	cbSize;

    ULONG	display_actual_version;
    ULONG	miniport_actual_version;
    ULONG	app_minreq_version;
    ULONG	display_buildno;
    ULONG	miniport_buildno;

    char	prod_name[esc_qvi_prod_name_max];
};

struct	Esc_dmf_pointer_shape_get_IN
{
    ULONG	cbSize;
    char *	pDstBmBuf;
    ULONG	nDstBmBufSize;		// (64* 64)* (33/ 8)+ 256* 4 = 17920
};

struct	Esc_dmf_pointer_shape_get_OUT
{
    ULONG	cbSize;
    POINTL	BmSize;		// XxY

    char *	pMaskBm;
    ULONG	nMaskBmSize;

    char *	pColorBm;
    ULONG	nColorBmSize;

    char *	pColorBmPal;
    ULONG	nColorBmPalEntries;
};


struct dPixelFormat
{
  dPixelFormat();
  void initBigEndianByNative();

  bool isEqualTo(const dPixelFormat *pf) const;

  unsigned short bitsPerPixel;
  unsigned short colorDepth;

  unsigned short redMax;
  unsigned short greenMax;
  unsigned short blueMax;

  unsigned short redShift;
  unsigned short greenShift;
  unsigned short blueShift;

  bool bigEndian;
};

class Rect
{
public:
  Rect(void) : left(0), top(0), right(0), bottom(0) {}
  Rect(const Rect *rect) { setRect(rect); }
  Rect(const Rect &rect) { setRect(&rect); }
  Rect(int lt, int tp, int rt, int bm) { setRect(lt, tp, rt, bm); }
  Rect(int width, int height) { setRect(0, 0, width, height); }
  Rect(const RECT *rect) {fromWindowsRect(rect);}

  virtual ~Rect(void) {}

  int left;
  int top;
  int right;
  int bottom;

  inline void setRect(int lt, int tp, int rt, int bm)
  {
    left = lt;
    top = tp;
    right = rt;
    bottom = bm;
  }

  inline void setRect(const Rect *rect)
  {
    left    = rect->left;
    top     = rect->top;
    right   = rect->right;
    bottom  = rect->bottom;
  }

  inline Rect & operator=(const Rect &rect)
  {
    setRect(&rect);
    return *this;
  }

  inline bool isValid() const
  {
    if (right < left || bottom < top) {
      return false;
    }
    return true;
  }

  inline bool isPointInRect(int pointX, int pointY) {
    if (pointX < left || pointX >= right) {
      return false;
    }
    if (pointY < top || pointY >= bottom) {
      return false;
    }
    return true;
  }

  inline RECT toWindowsRect() const {
                                RECT winRect;
                                winRect.left    = left;
                                winRect.top     = top;
                                winRect.right   = right;
                                winRect.bottom  = bottom;
                                return winRect; } const

  inline void fromWindowsRect(const RECT *winRect)
  {
    left    = winRect->left;
    top     = winRect->top;
    right   = winRect->right;
    bottom  = winRect->bottom;
  }

  inline void move(int offsetX, int offsetY)
  {
    left    += offsetX;
    right   += offsetX;
    top     += offsetY;
    bottom  += offsetY;
  }

  inline void setLocation(int destX, int destY)
  {
    int offsetX = destX - left;
    int offsetY = destY - top;
    left    = destX;
    right   += offsetX;
    top     = destY;
    bottom  += offsetY;
  }

  inline bool isEqualTo(const Rect *rect)  const { return  rect->left == left &&
                                                    rect->top == top &&
                                                    rect->right == right &&
                                                    rect->bottom == bottom; }

  inline void setWidth(int value)   { right = left + value; }
  inline void setHeight(int value)  { bottom = top + value; }

  inline int getWidth()  const { return right - left; }
  inline int getHeight() const { return bottom - top; }

  inline bool isEmpty() const { return getWidth() <= 0 || getHeight() <= 0; }
  inline int area() const { return isEmpty() ? 0 : getWidth() * getHeight(); }

  inline void clear() { left = top = right = bottom = 0; }

  Rect intersection(const Rect *other) const {
    Rect result;
    result.setRect((left > other->left) ? left : other->left,
                   (top > other->top) ? top : other->top,
                   (right < other->right) ? right : other->right,
                   (bottom < other->bottom) ? bottom : other->bottom);
    if (!result.isValid()) {
      result.clear();
    }
    return result;
  }
};

class Dimension
{
public:
  Dimension(void) : width(0), height(0) {}
  Dimension(const int w, const int h) { setDim(w, h); }
  Dimension(const Rect *r) { width = r->getWidth(); height = r->getHeight(); }
  Dimension(const Dimension &other): width(other.width), height(other.height) {}

  const Dimension &operator=(const Dimension &other) { width = other.width;
                                                       height = other.height;
                                                       return *this; }
  bool operator==(const Dimension &other) { return isEqualTo(&other); }
  bool operator!=(const Dimension &other) { return !isEqualTo(&other); }

  virtual ~Dimension(void) {}

  Rect getRect() const
  {
    Rect r(width, height);
    return r;
  }

  inline void setDim(const int w, const int h) { width = w; height = h; }
  inline void setDim(const Rect *rect) { width = rect->getWidth();
                                         height = rect->getHeight(); }

  inline bool cmpDim(const Dimension *dim) const { return dim->width == width &&
                                                          dim->height == height; }

  inline bool isEmpty() const { return width <= 0 || height <= 0; }
  inline bool isEqualTo(const Dimension *other) const
  {
    return (width == other->width)&& (height == other->height);
  }
  inline int area() const { return isEmpty() ? 0 : width * height; }

  inline void clear() { width = height = 0; }

  inline Dimension intersection(const Dimension *other) const
  {
    return Dimension(width < other->width ? width : other->width,
                     height < other->height ? height : other->height);
  }

  int width;
  int height;
};

struct Point
{
  Point() : x(0), y(0) {}
  Point(int x_, int y_) : x(x_), y(y_) {}

  inline void clear() { x = 0; y = 0; }
  inline void setPoint(int x_, int y_) { x = x_; y = y_; }
  inline void move(int deltaX, int deltaY) { x += deltaX; y += deltaY; }
  bool isEqualTo(const Point *other) const { return x == other->x &&
                                                    y == other->y; }

  int x;
  int y;
};

class Screen
{
public:
  Screen();
  ~Screen();

  void update();

  dPixelFormat getPixelFormat();

  Dimension getDesktopDimension();

  Rect getDesktopRect();

  struct BMI
  {
    BITMAPINFOHEADER bmiHeader;
    UINT32 red;
    UINT32 green;
    UINT32 blue;
  };

  struct Palette8bitBMI
  {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD rgbQuad[256];
  };

  void getBMI(BMI *bmi, HDC dc);

private:
  void fillPixelFormat(const BMI *bmi);
  static inline int findFirstBit(const UINT32 bits);

  void fillScreenRect();

  dPixelFormat m_pixelFormat;
  Rect m_virtDesktopRect;
};


class MirrorDriver :private WindowMessageHandler, private GuiThread
{
private:
  static const char MINIPORT_REGISTRY_PATH[];

  static const int EXT_DEVMODE_SIZE_MAX = 3072;
  struct DFEXT_DEVMODE : DEVMODEA
  {
    char extension[EXT_DEVMODE_SIZE_MAX];
  };
private:
  virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam);

  virtual void execute();
  virtual void onTerminate();

  void dispose();

  void extractDeviceInfo(char *driverName);
  void openDeviceRegKey(char *miniportName);

  void initScreenPropertiesByCurrent();
  void setAttachToDesktop(bool value);
  void commitDisplayChanges(DEVMODEA *pdm);

  bool isWin2000();
  void init();

  bool m_isDriverOpened;
  bool m_isDriverLoaded;
  bool m_isDriverAttached;
  bool m_isDriverConnected;

  DWORD m_deviceNumber;
  DISPLAY_DEVICEA m_deviceInfo;
  RegistryKey m_regkeyDevice;
  DFEXT_DEVMODE m_deviceMode;
  HDC m_driverDC;

  CHANGES_BUF *m_changesBuffer;
  void *m_screenBuffer;

  WindowsEvent m_initListener;
  bool m_isDisplayChanged;
  MessageWindow m_propertyChangeListenerWindow;

  dPixelFormat m_pixelFormat;
  Dimension m_dimension;
  Point m_leftTopCorner;
  Screen m_screen;

  OSVERSIONINFOA m_osVerInfo;

public:
    MirrorDriver();

    virtual ~MirrorDriver();

    dPixelFormat getPixelFormat() const;

    //返回当前屏幕的尺寸。
    Dimension getDimension() const;

    void *getBuffer();
    CHANGES_BUF *getChangesBuf() const;

    //检查屏幕（桌面）属性的变化
    bool getPropertiesChanged();
    bool getScreenSizeChanged();

    bool applyNewProperties();

    void open();
    void close();

    void load();
    void unload();

    void connect();
    void disconnect();
};

#endif // QMIRRORDRIVER_H
