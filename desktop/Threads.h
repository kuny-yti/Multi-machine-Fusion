#ifndef THREADS_H
#define THREADS_H

#include <windows.h>
#include <QString>
#include <QDebug>

/**
 * Thread priority enumeration.
 */
enum THREAD_PRIORITY
{
  PRIORITY_IDLE,
  PRIORITY_LOWEST,
  PRIORITY_BELOW_NORMAL,
  PRIORITY_NORMAL,
  PRIORITY_ABOVE_NORMAL,
  PRIORITY_HIGHEST,
  PRIORITY_TIME_CRITICAL
};

/**
 * Thread class.
 *
 * @fixme some of methods return bool instead of raising exceptions.
 * @fixme some methods seems to be not thread-safe (that uses m_active member).
 * @fixme member of HDESK type in THREAD class???
 */
class Thread
{
public:
  /**
   * Creates new thread.
   * @remark thread is suspended by default.
   */
  Thread();
  /**
   * Deletes thread.
   * @remark does not stops thread execution if it's still running.
   */
  virtual ~Thread();

  /**
   * Waits until thread stops.
   * @return false on error.
   */
  bool wait();
  /**
   * Suspends thread execution.
   * @return false on error.
   */
  bool suspend();
  /**
   * Resume thread execution.
   * @return false on error.
   */
  bool resume();
  /**
   * Terminates thread execution.
   * @remark thread-safe.
   */
  virtual void terminate();

  /**
   * Checks if thread is not dead.
   * @return true if thread is not dead (still running or suspended).
   */
  bool isActive() const;

  /**
   * Returns thread id.
   */
  DWORD getThreadId() const;

  /**
   * Sets thread priority.
   * @param value thread priority.
   */
  bool setPriority(THREAD_PRIORITY value);

  /**
   * Suspends the execution of the current thread until the time-out interval elapses.
   * @param millis time to sleep.
   */
  static void sleep(DWORD millis);

protected:
  /**
   * Returns true if terminate() method was called.
   * @remark thread-safe.
   */
  bool isTerminating();

  /**
   * Slot of terminate() signal.
   * Method called from terminate() method.
   * Can be overrided by subclasses to gracefully shutdown thread.
   */
  virtual void onTerminate();

  /**
   * Thread's runnable body.
   */
  virtual void execute() = 0;

private:
  /**
   * WinApi thread func.
   */
  static DWORD WINAPI threadProc(LPVOID pThread);

  // This function calling before call a derived execute() function to
  // perform any additional action.
  virtual void initByDerived();

private:
  /**
   * Win32 thread handle.
   */
  HANDLE m_hThread;
  /**
   * Thread ID.
   */
  DWORD m_threadID;
  /**
   * Activity flag.
   */
  bool m_active;
  /**
   * Terminating flag.
   */
  volatile bool m_terminated;
};

class DesktopSelector
{
public:
  // This funtion gets a handle to a desktop that receive user inputs.
  // @return If success the function returns a handle to the desktop.
  // On fail the function returns zero.
  // @remark After use the returned handle must be called the
  // DesktopSelector::closeDesktop() function.
  static HDESK getInputDesktop();

  // This funtion gets a handle to a named desktop by the name.
  // @param: name - pointer to a StringStorage object that contain a valid
  // desktop name such as "Winlogon".
  // @return if success the function returns a handle to the desktop
  // that receives user inputs. On fail the function returns zero.
  // @remark After use the returned handle must be called the
  // DesktopSelector::closeDesktop() function.
  static HDESK getDesktop(const char *name);

  // This function closes a handle to a desktop.
  // @param hdesk is a handle to a desktop that will be closed.
  // @return If success the function return true else false.
  static bool closeDesktop(HDESK hdesk);

  // This function sets a desktop to a thread from that it was called.
  // @param newDesktop - is a handle to a desktop.
  // @return If success the function return true else false.
  static bool setDesktopToCurrentThread(HDESK newDesktop);

  // This function select a desktop that assigned by name or not to a current
  // thread from that it was called.
  // @param: name - pointer to a StringStorage object that contain a valid
  // desktop name such as "Winlogon" that will be assigned to the thread. If
  // name is null the input desktop will be assigned.
  // @return If success the function return true else false.
  static bool selectDesktop(const char *name = 0);

  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the name of a current
  // input desktop stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getCurrentDesktopName(QString &desktopName);

  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the desktop name of the
  // current thread stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getThreadDesktopName(QString &desktopName);

private:
  // This function gets desktop name by a handle to a desktop.
  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the desktop name
  // stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getDesktopName(HDESK desktop, QString &desktopName);
};

class GuiThread : public Thread
{
public:
  GuiThread();
  virtual ~GuiThread();
private:
   // Replacing the base function.
  static DWORD WINAPI threadProc(LPVOID pThread);

  virtual void initByDerived();

   // Desktop for current thread.
  HDESK m_hDesk;
};

#endif // THREAD_H
