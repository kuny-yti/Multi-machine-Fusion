#include "ScreenClient.h"

ScreenClient::ScreenClient()
{
    m_buffer = 0;
    m_mirrorClient = new MirrorDriver();
    initFrameBuffer();
    executeDetection();
}

ScreenClient::~ScreenClient()
{
    terminateDetection();
    if (m_mirrorClient)
    {
        delete m_mirrorClient;
    }
}

void ScreenClient::executeDetection()
{
    resume();
}

void ScreenClient::terminateDetection()
{
    terminate();
    wait();
}

void ScreenClient::initFrameBuffer()
{
    m_dimension = m_mirrorClient->getDimension();
    m_pixelFormat = m_mirrorClient->getPixelFormat();

    if (m_buffer != 0)
    {
        delete []m_buffer;
    }
    if ((m_buffer = new UINT8[(m_dimension.area() * m_pixelFormat.bitsPerPixel) / 8]) == 0)
    {
        //return false;
    }
    //return true;
}

Dimension ScreenClient::getScreenDimension()
{
    return m_dimension;
}

bool ScreenClient::grab(const Rect *rect)
{
    //AutoLock al(m_fbMutex);

    if (m_mirrorClient == 0)
    {
        throw QString("Mirror driver client didn't initilized.");
    }

    Rect fbRect = m_dimension.getRect();
    Rect croppedRect;
    if (rect != 0)
    {
        croppedRect = rect->intersection(&fbRect);
    }
    else
    {
        croppedRect = fbRect;
    }


    char *dstPtr = (char *)m_buffer;
    dstPtr += (croppedRect.top * m_dimension.width + croppedRect.left) * ((UINT8)(m_pixelFormat.bitsPerPixel / 8));

    size_t offset = dstPtr - (char *)m_buffer;
    char *srcPtr = (char *)m_mirrorClient->getBuffer();
    if (srcPtr == 0)
    {
        return false;
    }
    srcPtr += offset;

    size_t count = croppedRect.getWidth() * ((UINT8)(m_pixelFormat.bitsPerPixel / 8));
    size_t stride = m_dimension.width * m_pixelFormat.bitsPerPixel / 8;

    for (size_t i = croppedRect.top; i < croppedRect.bottom; i++,
         dstPtr += stride,
         srcPtr += stride)
    {
        memcpy(dstPtr, srcPtr, count);
    }
    return true;
}

bool ScreenClient::getPropertiesChanged()
{
    //AutoLock al(m_fbMutex);
    if (m_mirrorClient != 0)
    {
        return m_mirrorClient->getPropertiesChanged();
    } else
    {
        throw QString("Mirror driver client didn't initilized.");
    }
}

bool ScreenClient::getScreenSizeChanged()
{
    //AutoLock al(m_fbMutex);
    if (m_mirrorClient != 0)
    {
        return m_mirrorClient->getScreenSizeChanged();
    } else
    {
        throw QString("Mirror driver client didn't initilized.");
    }
}

bool ScreenClient::applyNewProperties()
{
    //AutoLock al(m_fbMutex);

    delete m_mirrorClient;
    m_mirrorClient = 0;
    m_mirrorClient = new MirrorDriver();

    m_dimension = m_mirrorClient->getDimension();
    m_pixelFormat = m_mirrorClient->getPixelFormat();

    if (m_buffer != 0)
    {
        delete []m_buffer;
    }
    if ((m_buffer = new UINT8[(m_dimension.area() * m_pixelFormat.bitsPerPixel) / 8]) == 0)
    {
        //return false;
    }
    m_lastCounter = 0;

    return true;
}

void ScreenClient::startUpdateSearching()
{
    resume();
}

void *ScreenClient::getScreenBuffer()
{
    return m_buffer;
}

void ScreenClient::execute()
{
    //Region changedRegion;
    Rect changedRect;
    unsigned long currentCounter = 0;

    while (!isTerminating())
    {
        m_updateTimeout.waitForEvent(40);

        {
            //AutoLock al(m_fbMutex);
            if (m_mirrorClient != 0)
            {
                CHANGES_BUF *changesBuf = m_mirrorClient->getChangesBuf();
                if (changesBuf != 0)
                {
                    currentCounter = changesBuf->counter;
                    for (unsigned long i = m_lastCounter; i != currentCounter;
                         i++, i%= MAXCHANGES_BUF)
                    {
                        changedRect.fromWindowsRect(&changesBuf->pointrect[i].rect);
                        if (changedRect.isValid())
                        {
                            //changedRegion.addRect(&changedRect);
                        }
                    }

                    // m_updateKeeper->addChangedRegion(&changedRegion);
                    m_lastCounter = currentCounter;
                }
            }
        }

        /*if (!changedRegion.isEmpty())
    {
      doUpdate();
      changedRegion.clear();
    }*/
    }
}

void ScreenClient::onTerminate()
{
    m_updateTimeout.notify();
}
