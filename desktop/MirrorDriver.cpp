#include "MirrorDriver.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

dPixelFormat::dPixelFormat()
{
  memset(this, 0, sizeof(dPixelFormat));
}

void dPixelFormat::initBigEndianByNative()
{
  union {
    char test;
    int i;
  } testBigEndian;
  testBigEndian.i = 1;
  bigEndian = (testBigEndian.test == 0);
}

bool dPixelFormat::isEqualTo(const dPixelFormat *pf) const {
  return bitsPerPixel == pf->bitsPerPixel &&
         colorDepth   == pf->colorDepth &&
         redMax       == pf->redMax &&
         greenMax     == pf->greenMax &&
         blueMax      == pf->blueMax &&
         redShift     == pf->redShift &&
         greenShift   == pf->greenShift &&
         blueShift    == pf->blueShift &&
         bigEndian    == pf->bigEndian;
}

Screen::Screen()
{
  update();
}

Screen::~Screen()
{
}

void Screen::update()
{
  BMI bmi;
  getBMI(&bmi, 0);

  fillPixelFormat(&bmi);
  fillScreenRect();
}

dPixelFormat Screen::getPixelFormat()
{
  return m_pixelFormat;
}

Dimension Screen::getDesktopDimension()
{
  return Dimension(&m_virtDesktopRect);
}

Rect Screen::getDesktopRect()
{
  return m_virtDesktopRect;
}

void getErrorFunc(QString &out)
{
    DWORD errCode = GetLastError();
    char buffer[1024];

     // FIXME: Remove "new line" character from buffer.
     if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, errCode,
                       MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       (LPSTR)&buffer[0],
                       sizeof(buffer), NULL) == 0)
     {
       out = QString("<<Cannot get text error describing>> (%1)").arg(errCode);
     }
     else
     {
       out= QString("%1 (%2)").arg(buffer).arg(errCode);
     }
}

void getErrStr(char *str, QString &out)
{
    QString sysErrText;
    getErrorFunc(sysErrText);
    out = QString("%1 (%2)").arg(str).arg(sysErrText.toLocal8Bit().data());
}

void Screen::getBMI(BMI *bmi, HDC dc)
{
  HDC bitmapDC = dc;
  if (bitmapDC == 0) {
    bitmapDC = GetDC(0);
    if (bitmapDC == NULL)
    {
      throw QString("Can't get a bitmap dc");
    }
  }

  memset(bmi, 0, sizeof(BMI));
  bmi->bmiHeader.biBitCount = 0;
  bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

  HBITMAP hbm;
  hbm = (HBITMAP)GetCurrentObject(bitmapDC, OBJ_BITMAP);
  if (GetDIBits(bitmapDC, hbm, 0, 0, NULL, (LPBITMAPINFO)bmi, DIB_RGB_COLORS) == 0)
  {
        QString errMess;
        getErrStr("Can't get a DIBits", errMess);
        DeleteObject(hbm);
        DeleteDC(bitmapDC);
        throw errMess;
  }

  // The color table is filled only if it is used BI_BITFIELDS
  if (bmi->bmiHeader.biCompression == BI_BITFIELDS)
  {
    if (GetDIBits(bitmapDC, hbm, 0, 0, NULL, (LPBITMAPINFO)bmi, DIB_RGB_COLORS) == 0)
    {
      QString errMess;
      getErrStr("Can't get a DIBits", errMess);
      DeleteObject(hbm);
      DeleteDC(bitmapDC);
      throw errMess;
    }
  }

  DeleteObject(hbm);
  if (dc == 0) {
    DeleteDC(bitmapDC);
  }
}

void Screen::fillPixelFormat(const BMI *bmi)
{
  memset(&m_pixelFormat, 0, sizeof(dPixelFormat));

  m_pixelFormat.initBigEndianByNative();

  m_pixelFormat.bitsPerPixel = bmi->bmiHeader.biBitCount;

  if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
    m_pixelFormat.redShift   = findFirstBit(bmi->red);
    m_pixelFormat.greenShift = findFirstBit(bmi->green);
    m_pixelFormat.blueShift  = findFirstBit(bmi->blue);

    m_pixelFormat.redMax   = bmi->red    >> m_pixelFormat.redShift;
    m_pixelFormat.greenMax = bmi->green  >> m_pixelFormat.greenShift;
    m_pixelFormat.blueMax  = bmi->blue   >> m_pixelFormat.blueShift;

  } else {
    m_pixelFormat.bitsPerPixel = 32;
    m_pixelFormat.colorDepth = 24;
    m_pixelFormat.redMax = m_pixelFormat.greenMax = m_pixelFormat.blueMax = 0xff;
    m_pixelFormat.redShift   = 16;
    m_pixelFormat.greenShift = 8;
    m_pixelFormat.blueShift  = 0;
  }

  if (m_pixelFormat.bitsPerPixel == 32) {
    m_pixelFormat.colorDepth = 24;
  } else {
    m_pixelFormat.colorDepth = 16;
  }
}

int Screen::findFirstBit(const UINT32 bits)
{
  UINT32 b = bits;
  int shift;

  for (shift = 0; (shift < 32) && ((b & 1) == 0); shift++) {
    b >>= 1;
  }

  return shift;
}

void Screen::fillScreenRect()
{
  m_virtDesktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  m_virtDesktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  //m_virtDesktopRect.setWidth(GetSystemMetrics(SM_CXVIRTUALSCREEN));
  //m_virtDesktopRect.setHeight(GetSystemMetrics(SM_CYVIRTUALSCREEN));
  m_virtDesktopRect.setWidth(GetSystemMetrics(SM_CXSCREEN));
  m_virtDesktopRect.setHeight(GetSystemMetrics(SM_CYSCREEN));
}

const char MirrorDriver::MINIPORT_REGISTRY_PATH[] =
  "SYSTEM\\CurrentControlSet\\Hardware Profiles\\"
  "Current\\System\\CurrentControlSet\\Services"
;


MirrorDriver::MirrorDriver()
    : m_isDriverOpened(false),
      m_isDriverLoaded(false),
      m_isDriverAttached(false),
      m_isDriverConnected(false),
      m_isDisplayChanged(false),
      m_deviceNumber(0),
      m_driverDC(0),
      m_changesBuffer(0),
      m_screenBuffer(0),
      m_propertyChangeListenerWindow(GetModuleHandleA(0),
        "MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME")
{
      memset(&m_deviceMode, 0, sizeof(m_deviceMode));
      m_deviceMode.dmSize = sizeof(DEVMODE);

      qWarning() << "QMirrorDriver";
      open();
      load();
      connect();

      resume();
      m_initListener.waitForEvent();
      if (m_propertyChangeListenerWindow.getHWND() == 0)
      {
        dispose();
        throw QString("Can't create a client for a mirror driver because "
                      "can't create a message window to listen changing"
                      " screen properties.");
      }
}

MirrorDriver::~MirrorDriver()
{
  try {
        terminate();
        wait();

        dispose();
   }
    catch (QString &e)
    {
        qDebug() <<"An error occured during the" << " mirror driver deinitialization: "
                << e.toLocal8Bit().data();
   }
}

void MirrorDriver::dispose()
{
  if (m_isDriverConnected) {
    disconnect();
  }
  if (m_isDriverLoaded) {
    unload();
  }
  if (m_isDriverOpened) {
    close();
  }
}

dPixelFormat MirrorDriver::getPixelFormat() const
{
  return m_pixelFormat;
}

Dimension MirrorDriver::getDimension() const
{
  return m_dimension;
}

void *MirrorDriver::getBuffer()
{
  return m_screenBuffer;
}

CHANGES_BUF *MirrorDriver::getChangesBuf() const
{
  return m_changesBuffer;
}

bool MirrorDriver::getPropertiesChanged()
{
  return m_isDisplayChanged;
}

bool MirrorDriver::getScreenSizeChanged()
{
  return m_isDisplayChanged;
}

bool MirrorDriver::applyNewProperties()
{
  try {
        qWarning() << "Disposing the mirror driver to apply new properties";
        dispose();

        m_isDisplayChanged = false;

        qWarning() << "Try load new mirror driver to apply new properties";
        open();
        load();
        connect();
    }
    catch (QString &e)
    {
        qDebug() << "Can't apply new screen properties for the mirror driver:"
                 << e.toLocal8Bit().data();
        return false;
    }

  return true;
}

void MirrorDriver::open()
{
  Q_ASSERT(!m_isDriverOpened);

  if (false && ::GetSystemMetrics(SM_REMOTESESSION))
  {
      //qWarning() << "We're running in a Remote Desktop session." << " Can't operate in this environment!";
        throw QString("We're running in a Remote Desktop session."
                      "Can't operate in this environment!");
  }

  extractDeviceInfo("Fusion Driver");
  openDeviceRegKey("mirrorse");

  m_isDriverOpened = true;
}

void MirrorDriver::extractDeviceInfo(char *driverName)
{
  memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));
  m_deviceInfo.cb = sizeof(m_deviceInfo);

  qWarning() << "Searching for  ..." << driverName;

  m_deviceNumber = 0;
  BOOL result;
  while (result = EnumDisplayDevicesA(0, m_deviceNumber, &m_deviceInfo, 0))
  {
      qWarning() << "Found:" << m_deviceInfo.DeviceString;
      qWarning() << "RegKey:" << m_deviceInfo.DeviceKey;
        QString deviceString(m_deviceInfo.DeviceString);
        QString EqualTo(driverName);
        if (deviceString == EqualTo)
        {
            qWarning() << driverName << " is found";
          break;
        }
        m_deviceNumber++;
  }
  if (!result)
  {
    //qDebug() << "Can't find " << driverName;
    throw QString("Can't find %1!").arg(driverName);
  }
}

void MirrorDriver::openDeviceRegKey(char *miniportName)
{
    int startIndex = 1;
    int endIndex = 7;
  QString deviceKey(m_deviceInfo.DeviceKey);
  deviceKey.toUpper();
  char *substrPos = strstr(deviceKey.toLocal8Bit().data(), "\\DEVICE");
  QString subKey("DEVICE0");
  if (substrPos != 0)
  {
    QString str(substrPos);
    if (str.length() >= 8)
    {
         startIndex = min(startIndex, str.length());
         endIndex = min(endIndex, str.length());
         if (endIndex < startIndex)
         {
           endIndex = startIndex;
         }

         size_t length = endIndex - startIndex;
         Q_ASSERT(length <= str.length());

         QString autoBuffer;//(length + 1);
         for (int i = startIndex-1; i < endIndex; i++)
         {
             if (endIndex-1 == i)
             {
                 autoBuffer.append("\0");
             }
             else
             {
                autoBuffer.append(str[startIndex]);
             }
         }

         subKey = (autoBuffer);
    }
  }

  qWarning() << "Opening registry key " << MINIPORT_REGISTRY_PATH << "\\"
             << miniportName << "\\" << subKey.toLocal8Bit().data();

  RegistryKey regKeyServices(HKEY_LOCAL_MACHINE, MINIPORT_REGISTRY_PATH, true);
  RegistryKey regKeyDriver(&regKeyServices, miniportName, true);
  m_regkeyDevice.open(&regKeyDriver, subKey.toLocal8Bit().data(), true);
  if (!regKeyServices.isOpened() || !regKeyDriver.isOpened() ||
      !m_regkeyDevice.isOpened())
  {
      //qWarning() <<"Can't open registry for the mirror driver";
        throw QString("Can't open registry for the mirror driver");
  }
}

void MirrorDriver::close()
{
  m_regkeyDevice.closes();
  m_isDriverOpened = false;
}

void MirrorDriver::load()
{
  Q_ASSERT(m_isDriverOpened);
  if (!m_isDriverLoaded)
  {
      qWarning() <<"Loading mirror driver...";

    initScreenPropertiesByCurrent();

    WORD drvExtraSaved = m_deviceMode.dmDriverExtra;
    // IMPORTANT: we dont touch extension data and size
    memset(&m_deviceMode, 0, sizeof(DEVMODE));
    // m_deviceMode.dmSize = sizeof(m_deviceMode);
    m_deviceMode.dmSize = sizeof(DEVMODE);
    // 2005.10.07
    m_deviceMode.dmDriverExtra = drvExtraSaved;

    m_deviceMode.dmPelsWidth = m_dimension.width;
    m_deviceMode.dmPelsHeight = m_dimension.height;
    m_deviceMode.dmBitsPerPel = m_pixelFormat.bitsPerPixel;
    m_deviceMode.dmPosition.x = m_leftTopCorner.x;
    m_deviceMode.dmPosition.y = m_leftTopCorner.y;

    m_deviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH |
                            DM_PELSHEIGHT | DM_POSITION;
    m_deviceMode.dmDeviceName[0] = '\0';

    setAttachToDesktop(true);
    commitDisplayChanges(&m_deviceMode);

    qWarning() << "Device name: "<< m_deviceInfo.DeviceName;
    m_driverDC = CreateDCA(m_deviceInfo.DeviceName, 0, 0, 0);
    if (!m_driverDC)
    {
         //qWarning() << "Can't create device context on mirror driver";
         throw QString("Can't create device context on mirror driver");
    }
    qWarning() << "Device context is created";

    m_isDriverLoaded = true;
    qWarning() << "Mirror driver is now loaded";
  }
}

void MirrorDriver::initScreenPropertiesByCurrent()
{
  m_pixelFormat.initBigEndianByNative();
  m_pixelFormat.bitsPerPixel = 32;
  m_pixelFormat.redMax = 255;
  m_pixelFormat.redShift = 16;
  m_pixelFormat.greenMax = 255;
  m_pixelFormat.greenShift = 8;
  m_pixelFormat.blueMax = 255;
  m_pixelFormat.blueShift = 0;
  m_pixelFormat.colorDepth = 24;

  Rect virtDeskRect = m_screen.getDesktopRect();
  m_dimension.setDim(&virtDeskRect);
  m_leftTopCorner.setPoint(virtDeskRect.left, virtDeskRect.top);
}

void MirrorDriver::setAttachToDesktop(bool value)
{
  if (!m_regkeyDevice.setValueAsInt32("Attach.ToDesktop",
      (int)value))
  {
        //qWarning() <<"Can't set the Attach.ToDesktop.";
        throw QString("Can't set the Attach.ToDesktop.");
  }
  m_isDriverAttached = value;
}

void MirrorDriver::commitDisplayChanges(DEVMODEA *pdm)
{
    qWarning() << "commitDisplayChanges(1):" << m_deviceInfo.DeviceName;
  if (pdm)
  {
    LONG code = ChangeDisplaySettingsExA(m_deviceInfo.DeviceName, pdm, 0, CDS_UPDATEREGISTRY, 0);
    if (code < 0)
    {
      throw QString("1st ChangeDisplaySettingsEx() failed with code %1").arg(code);
    }
    qWarning() << "CommitDisplayChanges(2): " << m_deviceInfo.DeviceName;

    code = ChangeDisplaySettingsExA(m_deviceInfo.DeviceName, pdm, 0, 0, 0);
    if (code < 0)
    {

        throw QString("2nd ChangeDisplaySettingsEx() failed with code %1").arg(code);
    }
  } else {
    LONG code = ChangeDisplaySettingsExA(m_deviceInfo.DeviceName, 0, 0, 0, 0);
    if (code < 0)
    {

        throw QString("ChangeDisplaySettingsEx() failed with code %1").arg(code);
    }
  }
  qWarning() << "ChangeDisplaySettingsEx() was successfull";
}

void MirrorDriver::init()
{
  if (m_osVerInfo.dwOSVersionInfoSize == 0)
  {
    m_osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (!GetVersionExA(&m_osVerInfo)) {
      m_osVerInfo.dwOSVersionInfoSize = 0;
    }
  }
}

bool MirrorDriver::isWin2000()
{
  init();
  return m_osVerInfo.dwMajorVersion == 5 && m_osVerInfo.dwMinorVersion == 0;
}

void MirrorDriver::unload()
{
  if (m_driverDC != 0)
  {
    DeleteDC(m_driverDC);
    m_driverDC = 0;
    qWarning() << "The mirror driver device context released";
  }

  if (m_isDriverAttached)
  {
      qWarning() << "Unloading mirror driver...";

    setAttachToDesktop(false);

    m_deviceMode.dmPelsWidth = 0;
    m_deviceMode.dmPelsHeight = 0;

    // IMPORTANT: Windows 2000 fails to unload the driver
    // if the mode passed to ChangeDisplaySettingsEx() contains DM_POSITION set.
    DEVMODEA *pdm = 0;
    if (!isWin2000()) {
      pdm = &m_deviceMode;
    }

    try {
      commitDisplayChanges(pdm);
      qWarning() << "Mirror driver is unloaded";
    } catch (QString &e)
    {
      qDebug() << "Failed to unload the mirror driver:"  << e.toLocal8Bit().data();
    }
  }

  // NOTE: extension data and size is also reset
  memset(&m_deviceMode, 0, sizeof(m_deviceMode));
  m_deviceMode.dmSize = sizeof(DEVMODE);

  m_isDriverLoaded = false;
}

void MirrorDriver::connect()
{
    qWarning() <<"Try to connect to the mirror driver.";
  if (!m_isDriverConnected) {
    GETCHANGESBUF buf = {0};
    int res = ExtEscape(m_driverDC, dmf_esc_usm_pipe_map, 0, 0, sizeof(buf), (LPSTR)&buf);
    if (res <= 0)
    {
       qDebug() <<  "Can't set a connection for the mirror driver: " << "ExtEscape() failed with " << res;

    }

    m_changesBuffer = buf.buffer;
    m_screenBuffer = buf.Userbuffer;

    m_isDriverConnected = true;
  }
}

void MirrorDriver::disconnect()
{
     qWarning() <<"Try to disconnect the mirror driver.";
  if (m_isDriverConnected)
  {
    GETCHANGESBUF buf;
    buf.buffer = m_changesBuffer;
    buf.Userbuffer = m_screenBuffer;

    int res = ExtEscape(m_driverDC, dmf_esc_usm_pipe_unmap, sizeof(buf), (LPSTR)&buf, 0, 0);
    if (res <= 0)
    {
         qDebug() <<"Can't unmap buffer: error code = " << res;
    }
    m_isDriverConnected = false;
  }
}

bool MirrorDriver::processMessage(UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
  if (message == WM_DISPLAYCHANGE)
  {
    m_isDisplayChanged = true;
    qWarning() << "Display changing detecting";
  }
  return true;
}

void MirrorDriver::onTerminate()
{
  PostMessageA(m_propertyChangeListenerWindow.getHWND(), WM_QUIT, 0, 0);
}

void MirrorDriver::execute()
{
  if (!isTerminating())
  {
    m_propertyChangeListenerWindow.createWindow(this);
    qWarning() << "Mirror driver client window has been created (hwnd = " << (int)m_propertyChangeListenerWindow.getHWND() << ")";

  }

  m_initListener.notify();

  MSG msg;
  while (!isTerminating())
  {
    if (PeekMessageA(&msg, m_propertyChangeListenerWindow.getHWND(),
                    0, 0, PM_REMOVE) != 0)
    {
      if (msg.message == WM_DISPLAYCHANGE)
      {
        m_isDisplayChanged = true;
      }
      else
      {
        DispatchMessageA(&msg);
      }
    }
    else
    {
      if (WaitMessage() == 0)
      {
          qDebug() <<  "Mirror driver client thread has failed";
        terminate();
      }
    }
  }
}
