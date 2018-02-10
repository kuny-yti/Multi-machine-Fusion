#ifndef QSCREENDRIVER_H
#define QSCREENDRIVER_H

#include "MirrorDriver.h"

class ScreenDriverInterface
{
public:
  ScreenDriverInterface(){}
  virtual ~ScreenDriverInterface(){}

  // 启动屏幕更新检测，如果它尚未开始。
  virtual void executeDetection() = 0;

  // 停止屏幕更新检测。
  virtual void terminateDetection() = 0;

  // 返回当前屏幕的尺寸。
  virtual Dimension getScreenDimension() = 0;

  // 返回一个指向内部的屏驱动程序的FrameBuffer
  //virtual FrameBuffer *getScreenBuffer() = 0;

  /* Provides grabbing.
  Parameters:     *rect 指向一个RECT对象的相对workRect坐标。
  Return value:   true 为成功.
  */
  virtual bool grab(const Rect *rect = 0) = 0;

  // 检查屏幕（桌面）属性的变化
  virtual bool getPropertiesChanged() = 0;
  virtual bool getScreenSizeChanged() = 0;

  // 设置新值为默认的WorkRect（全屏矩形坐标）
  // 如果桌面属性已被更改。
  // 此外，帧缓冲区像素格式设置的实际价值。
  virtual bool applyNewProperties() = 0;
};
#endif // QSCREENDRIVER_H
