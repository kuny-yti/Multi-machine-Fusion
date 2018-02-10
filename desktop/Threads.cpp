#include "Threads.h"

Thread::Thread()
: m_terminated(false), m_active(false)
{
  m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc,
                           (LPVOID) this, CREATE_SUSPENDED, (LPDWORD) &m_threadID);
}

Thread::~Thread()
{
  CloseHandle(m_hThread);
}

DWORD WINAPI Thread::threadProc(LPVOID pThread)
{
  Thread *_this = ((Thread *)pThread);
  try {
    _this->initByDerived();
    _this->execute();
  } catch (QString &e)
  {
    qDebug() << "Abnormal thread termination."
             << " ThreadId = " << _this->m_threadID
             << " message = " << e.toLocal8Bit().data();

  }
  _this->m_active = false;
  return 0;
}

void Thread::initByDerived()
{
  // If is needed this function will be inherited by a derived class.
}

bool Thread::wait()
{
  return (WaitForSingleObject(m_hThread, INFINITE) != WAIT_FAILED);
}

// FIXME: not thread-safe (m_active).
bool Thread::suspend()
{
  m_active = !(SuspendThread(m_hThread) != -1);

  return !m_active;
}

// FIXME: not thread-safe (m_active).
bool Thread::resume()
{
  m_active = ResumeThread(m_hThread) != -1;

  return m_active;
}

void Thread::terminate()
{
  m_terminated = true;

  onTerminate();
}

bool Thread::isActive() const
{
  return m_active;
}

DWORD Thread::getThreadId() const
{
  return m_threadID;
}

bool Thread::setPriority(THREAD_PRIORITY value)
{
  int priority;

  switch(value)
  {
  case PRIORITY_IDLE:
    priority = THREAD_PRIORITY_IDLE;
    break;
  case PRIORITY_LOWEST:
    priority = THREAD_PRIORITY_LOWEST;
    break;
  case PRIORITY_BELOW_NORMAL:
    priority = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case PRIORITY_NORMAL:
    priority = THREAD_PRIORITY_NORMAL;
    break;
  case PRIORITY_ABOVE_NORMAL:
    priority = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case PRIORITY_HIGHEST:
    priority = THREAD_PRIORITY_HIGHEST;
    break;
  case PRIORITY_TIME_CRITICAL:
    priority = THREAD_PRIORITY_TIME_CRITICAL;
    break;
  default:
    priority = THREAD_PRIORITY_NORMAL;
  }

  return SetThreadPriority(m_hThread, priority) != 0;
}

void Thread::sleep(DWORD millis)
{
  Sleep(millis);
}

bool Thread::isTerminating()
{
  return m_terminated;
}

void Thread::onTerminate()
{
}

HDESK DesktopSelector::getInputDesktop()
{
  return OpenInputDesktop(0, TRUE,
                          DESKTOP_CREATEMENU |
                          DESKTOP_CREATEWINDOW |
                          DESKTOP_ENUMERATE |
                          DESKTOP_HOOKCONTROL |
                          DESKTOP_WRITEOBJECTS |
                          DESKTOP_READOBJECTS |
                          DESKTOP_SWITCHDESKTOP |
                          GENERIC_WRITE);
}

HDESK DesktopSelector::getDesktop(const char *name)
{
  return OpenDesktopA((LPSTR)name, 0, TRUE,
                     DESKTOP_CREATEMENU |
                     DESKTOP_CREATEWINDOW |
                     DESKTOP_ENUMERATE |
                     DESKTOP_HOOKCONTROL |
                     DESKTOP_WRITEOBJECTS |
                     DESKTOP_READOBJECTS |
                     DESKTOP_SWITCHDESKTOP |
                     GENERIC_WRITE);
}

bool DesktopSelector::closeDesktop(HDESK hdesk)
{
  return CloseDesktop(hdesk) != 0;
}

bool DesktopSelector::setDesktopToCurrentThread(HDESK newDesktop)
{
  return SetThreadDesktop(newDesktop) != 0;
}

bool DesktopSelector::selectDesktop(const char *name)
{
  HDESK desktop;
  if (name)
  {
    desktop = getDesktop(name);
  }
  else
  {
    desktop = getInputDesktop();
  }

  bool result = setDesktopToCurrentThread(desktop) != 0;
  closeDesktop(desktop);

  return result;
}

bool DesktopSelector::getDesktopName(HDESK desktop, QString &desktopName)
{
    desktopName.clear();

  DWORD nameLength = 0;
  // Do not check returned value because the function will return FALSE always.
  GetUserObjectInformationA(desktop, UOI_NAME, 0, 0, &nameLength);

  if (nameLength != 0)
  {
      QString name;name = QString("%1").arg(nameLength);
    bool result = !!GetUserObjectInformationA(desktop,
                                             UOI_NAME,
                                             name.data(),
                                             nameLength,
                                             0);
    if (result) {
        desktopName = name;
      return true;
    }
  }
  return false;
}

bool DesktopSelector::getCurrentDesktopName(QString &desktopName)
{
  HDESK inputDesktop = getInputDesktop();
  bool result = getDesktopName(inputDesktop, desktopName);
  closeDesktop(inputDesktop);
  return result;
}

bool DesktopSelector::getThreadDesktopName(QString &desktopName)
{
  return getDesktopName(GetThreadDesktop(GetCurrentThreadId()), desktopName);
}

GuiThread::GuiThread()
: m_hDesk(0)
{
  m_hDesk = DesktopSelector::getInputDesktop();
}

GuiThread::~GuiThread()
{
}

void GuiThread::initByDerived()
{
  DesktopSelector::setDesktopToCurrentThread(m_hDesk);
  DesktopSelector::closeDesktop(m_hDesk);
  m_hDesk = 0;
}
