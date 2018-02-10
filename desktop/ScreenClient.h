#ifndef QSCREENCLIENT_H
#define QSCREENCLIENT_H

#include "ScreenDriverInterface.h"
#include "Threads.h"

class ScreenClient : public ScreenDriverInterface, GuiThread
{
public:
    ScreenClient();
    virtual ~ScreenClient();

    // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual Dimension getScreenDimension();
      virtual void *getScreenBuffer();
      virtual bool grab(const Rect *rect = 0);

      virtual bool getPropertiesChanged();
      virtual bool getScreenSizeChanged();

      virtual bool applyNewProperties();

    private:
      void initFrameBuffer();

      void startUpdateSearching();

      virtual void execute();
      virtual void onTerminate();

      MirrorDriver *m_mirrorClient;
      unsigned long m_lastCounter;

      WindowsEvent m_updateTimeout;

      Dimension m_dimension;

    dPixelFormat m_pixelFormat;
    void *m_buffer;

};

#endif // QSCREENCLIENT_H
