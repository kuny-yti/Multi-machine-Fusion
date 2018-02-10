#ifndef REGISTRYKEY_H
#define REGISTRYKEY_H

#include <windows.h>
#include <QString>
#include <QVector>

class RegistryKey
{
public:
  RegistryKey(HKEY rootKey, const char *entry, bool createIfNotExists = true, SECURITY_ATTRIBUTES *sa = 0);
  RegistryKey(RegistryKey *rootKey, const char *entry, bool createIfNotExists = true, SECURITY_ATTRIBUTES *sa = 0);
  RegistryKey(HKEY rootKey);
  RegistryKey();

public:

  virtual ~RegistryKey();

  void open(HKEY rootKey, const char *entry,
            bool createIfNotExists = true,
            SECURITY_ATTRIBUTES *sa = 0);

  void open(RegistryKey *rootKey,
            const char *entry,
            bool createIfNotExists = true,
            SECURITY_ATTRIBUTES *sa = 0);

  HKEY getHKEY() const;

  bool createSubKey(const char *subkey);
  bool deleteSubKey(const char *subkey);
  bool deleteSubKeyTree(const char *subkey);
  bool deleteValue(const char *name);

  bool setValueAsInt32(const char *name, int value);
  bool setValueAsInt64(const char *name, long value);
  bool setValueAsString(const char *name, const char *value);
  bool setValueAsBinary(const char *name, const void *value,
                        size_t sizeInBytes);

  bool getValueAsInt32(const char *name, int *out);
  bool getValueAsInt64(const char *name, long *out);
  bool getValueAsString(const char *name, QString &out);
  bool getValueAsBinary(const char *name, void *value, size_t *sizeInBytes);

  bool getSubKeyNames(QString *subKeyNames, size_t *count);

  bool isOpened();
  void closes();

private:

  void initialize(HKEY rootKey, const char *entry, bool createIfNotExists, SECURITY_ATTRIBUTES *sa);

  DWORD enumKey(DWORD i, QString &name);

  static bool tryOpenSubKey(HKEY key, const char *subkey,
                            HKEY *openedKey, bool createIfNotExists,
                            SECURITY_ATTRIBUTES *sa);

protected:
  HKEY m_key;
  HKEY m_rootKey;
  QString m_entry;

  friend class Registry;
};

class Registry
{
public:
  const static int STRING_TYPE = REG_SZ;
  const static int EXPAND_STRING_TYPE = REG_EXPAND_SZ;
  const static int BINARY_TYPE = REG_BINARY;
  const static int DWORD_TYPE = REG_DWORD;
  const static int QWORD_TYPE = REG_QWORD;

public:
  static HKEY getCurrentUserKey();
  static HKEY getCurrentLocalMachineKey();

private:
  Registry();
  ~Registry();
};

class WindowsEvent
{
public:
  WindowsEvent(const char *name = 0);
  virtual ~WindowsEvent();

  void notify();
  void waitForEvent(DWORD milliseconds = INFINITE);

  HANDLE getHandle() const { return m_hEvent; }

protected:
  void setAccessToAll(HANDLE objHandle);

  HANDLE m_hEvent;
};

class WindowMessageHandler
{
public:
  virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
};

class MessageWindow
{
public:
  MessageWindow(const HINSTANCE hinst, const char *windowClassName,
                WindowMessageHandler *messageHandler = 0);
  virtual ~MessageWindow(void);

  virtual bool createWindow(WindowMessageHandler *messageHandler = 0);
  virtual void destroyWindow();

  HWND getHWND() const { return m_hwnd; }

protected:
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam) { return true; }

  HWND m_hwnd;
  WindowMessageHandler *m_messageHandler;

  HINSTANCE m_hinst;
  QString m_windowClassName;

private:
  ATOM regClass(HINSTANCE hinst, char *windowClassName);

  static LRESULT CALLBACK staticWndProc(HWND hwnd,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam);
};
#endif // REGISTRYKEY_H
