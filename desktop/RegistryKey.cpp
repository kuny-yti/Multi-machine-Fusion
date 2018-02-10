#include "RegistryKey.h"
#include <vector>
#include <Aclapi.h>
#include <QDebug>

RegistryKey::RegistryKey(HKEY rootKey, const char *entry,
                         bool createIfNotExists, SECURITY_ATTRIBUTES *sa)
{
  initialize(rootKey, entry, createIfNotExists, sa);
}

RegistryKey::RegistryKey(RegistryKey *rootKey, const char *entry,
                         bool createIfNotExists, SECURITY_ATTRIBUTES *sa)
{
  initialize(rootKey->m_key, entry, createIfNotExists, sa);
}

RegistryKey::RegistryKey(HKEY rootKey)
: m_key(rootKey), m_rootKey(rootKey)
{
    m_entry.clear();
}

RegistryKey::RegistryKey()
{
}

RegistryKey::~RegistryKey()
{
  closes();
}

void RegistryKey::open(HKEY rootKey,
                       const char *entry,
                       bool createIfNotExists,
                       SECURITY_ATTRIBUTES *sa)
{
  initialize(rootKey, entry, createIfNotExists, sa);
}

void RegistryKey::open(RegistryKey *rootKey,
                       const char *entry,
                       bool createIfNotExists,
                       SECURITY_ATTRIBUTES *sa)
{
  initialize(rootKey->m_key, entry, createIfNotExists, sa);
}

HKEY RegistryKey::getHKEY() const
{
  return m_key;
}

bool RegistryKey::createSubKey(const char *subkey)
{
  if (!isOpened()) {
    return false;
  }

  RegistryKey regKey(m_key, subkey);
  return regKey.isOpened();
}

bool RegistryKey::deleteSubKey(const char *subkey)
{
  if (!isOpened()) {
    return false;
  }

  return RegDeleteKeyA(m_key, subkey) == ERROR_SUCCESS;
}

bool RegistryKey::deleteSubKeyTree(const char *subkey)
{
  if (!isOpened()) {
    return false;
  }

  bool retVal = true;

  // Subkey
  RegistryKey key(this, subkey);

  size_t subkeys2Count = 0;
  QString subkeys2Names = 0;

  if (key.getSubKeyNames(NULL, &subkeys2Count) && subkeys2Count != 0) {
    QVector<QString> subkeys2Names;
    subkeys2Names.resize(subkeys2Count);

    key.getSubKeyNames(&subkeys2Names[0], NULL);

    for (size_t i = 0; i < subkeys2Count; i++) {
      if (!key.deleteSubKeyTree(subkeys2Names[i].toLocal8Bit().data()))
      {
        retVal = false;
      }
    }
  } else {
    retVal = false;
  }

  // Delete subkey
  if (!deleteSubKey(subkey)) {
    retVal = false;
  }

  return retVal;
}

bool RegistryKey::deleteValue(const char *name)
{
  if (!isOpened()) {
    return false;
  }

  return RegDeleteValueA(m_key, name) == ERROR_SUCCESS;
}

bool RegistryKey::setValueAsInt32(const char *name, int value)
{
  if (!isOpened()) {
    return false;
  }

  return RegSetValueExA(m_key, name, 0, REG_DWORD, (BYTE *)&value, sizeof(value)) == ERROR_SUCCESS;
}

bool RegistryKey::setValueAsInt64(const char *name, long value)
{
  if (!isOpened()) {
    return false;
  }

  return RegSetValueExA(m_key, name, 0, REG_QWORD, (BYTE *)&value, sizeof(value)) == ERROR_SUCCESS;
}

bool RegistryKey::setValueAsString(const char *name, const char *value)
{
  if (!isOpened()) {
    return false;
  }

  size_t origSize = (strlen(value) + 1) * sizeof(char);
  DWORD size = (DWORD)origSize;
  Q_ASSERT(size == origSize);
  return RegSetValueExA(m_key, name, 0, REG_SZ, (BYTE *)value, size) == ERROR_SUCCESS;
}

bool RegistryKey::setValueAsBinary(const char *name, const void *value, size_t sizeInBytes)
{
  if (!isOpened()) {
    return false;
  }

  DWORD size = (DWORD)sizeInBytes;
  Q_ASSERT(size == sizeInBytes);
  return RegSetValueExA(m_key, name, 0, REG_BINARY, (BYTE *)value, size) == ERROR_SUCCESS;
}

bool RegistryKey::getValueAsInt32(const char *name, int *out)
{
  if (!isOpened()) {
    return false;
  }

  DWORD type = REG_DWORD;
  DWORD size = sizeof(out);

  return RegQueryValueExA(m_key, name, 0, &type, (BYTE *)out, &size) == ERROR_SUCCESS;
}

bool RegistryKey::getValueAsInt64(const char *name, long *out)
{
  if (!isOpened()) {
    return false;
  }

  DWORD type = REG_QWORD;
  DWORD size = sizeof(out);

  return RegQueryValueExA(m_key, name, 0, &type, (BYTE *)out, &size) == ERROR_SUCCESS;
}

bool RegistryKey::getValueAsString(const char *name, QString &out)
{
  if (!isOpened()) {
    return false;
  }

  DWORD type = REG_SZ;
  DWORD size = sizeof(out);

  if (RegQueryValueExA(m_key, name, 0, &type, NULL, &size) != ERROR_SUCCESS) {
    return false;
  }

  QVector<char> buffer;
  buffer.resize(size + 1);

  if (RegQueryValueExA(m_key, name, 0, &type, (BYTE *)&buffer[0], &size) != ERROR_SUCCESS) {
    return false;
  }

  if (buffer[size] != ('\0')) {
    buffer[size] = ('\0');
  }

  out = QString(&buffer[0]);

  return true;
}

bool RegistryKey::getValueAsBinary(const char *name, void *value, size_t *sizeInBytes)
{
  if (!isOpened()) {
    return false;
  }

  DWORD type = REG_BINARY;
  DWORD size = (DWORD)*sizeInBytes;

  if (RegQueryValueExA(m_key, name, 0, &type, (LPBYTE)value, &size) != ERROR_SUCCESS) {
    return false;
  }
  *sizeInBytes = (size_t)size;
  return true;

}

bool RegistryKey::getSubKeyNames(QString *subKeyNames, size_t *count)
{
  if (!isOpened()) {
    return false;
  }

  DWORD ret = 0;
  DWORD i = 0;

  while (true) {
    QString keyName;

    ret = enumKey(i, keyName);

    if (ret == ERROR_SUCCESS) {
      if (subKeyNames != NULL) {
        subKeyNames[i] = (keyName);
      }
      i++;
    } else if (ret == ERROR_NO_MORE_ITEMS) {
      break;
    } else {
      break;
    }
  } // while

  if (count != NULL) {
    *count = (size_t)i;
  }

  return ret == ERROR_NO_MORE_ITEMS;
} // void

bool RegistryKey::isOpened()
{
  return m_key != 0;
}

void RegistryKey::closes()
{
  if (isOpened()) {
    if (m_key != m_rootKey) {
      RegCloseKey(m_key);
    }
    m_key = 0;
  }
}

void RegistryKey::initialize(HKEY rootKey, const char *entry, bool createIfNotExists, SECURITY_ATTRIBUTES *sa)
{
  m_rootKey = rootKey;
  m_key = 0;

  m_entry = QString(entry);

  if (!m_entry.isEmpty() && !m_entry.endsWith('\\'))
  {
    m_entry.append("\\");
  }

  tryOpenSubKey(m_rootKey, m_entry.toLocal8Bit().data(), &m_key, createIfNotExists, sa);
}

DWORD RegistryKey::enumKey(DWORD i, QString &name)
{
  DWORD length = 1024;
  DWORD increaseStep = 1024;

  QVector<char> buffer;

  DWORD ret;

  while (true)
  {
    buffer.resize(length+1);
    ret = RegEnumKeyA(m_key, i, (LPSTR)&buffer[0], length);

    if (ret == ERROR_SUCCESS)
    {
        name = QString(&buffer[0]);
      break;
    } else if (ret == ERROR_MORE_DATA) {
      length += increaseStep;
    } else {
      break;
    }
  } // while

  return ret;
}

bool RegistryKey::tryOpenSubKey(HKEY key, const char *subkey, HKEY *openedKey, bool createIfNotExists, SECURITY_ATTRIBUTES *sa)
{
  if (RegOpenKeyA(key, (LPCSTR)subkey, openedKey) != ERROR_SUCCESS) {
    if (createIfNotExists) {
      DWORD ret = 0;

      if (sa != 0) {
        DWORD dwDisposition;

        ret = RegCreateKeyExA(key, subkey, 0, "", 0,
                             KEY_READ | KEY_WRITE,
                             sa, openedKey, &dwDisposition);
      } else {
        ret = RegCreateKeyA(key, subkey, openedKey);
      }
      if (ret != ERROR_SUCCESS) {
        return false;
      } // if cannot create key.
    } else {
      return false;
    }
  } // if cannot open key

  return true;
}

Registry::Registry()
{
}

Registry::~Registry()
{
}

HKEY Registry::getCurrentUserKey()
{
  return HKEY_CURRENT_USER;
}

HKEY Registry::getCurrentLocalMachineKey()
{
  return HKEY_LOCAL_MACHINE;
}

WindowsEvent::WindowsEvent(const char *name)
{
  m_hEvent = CreateEventA(0, FALSE, FALSE, (LPCSTR)name);
  if (m_hEvent == 0) {
    int errCode = GetLastError();
    QString errMess;
    errMess = QString("Cannot create windows event with error = %1").arg(errCode);
    throw errMess;
  }

  bool needToInit = GetLastError() != ERROR_ALREADY_EXISTS;
  if (needToInit) {
    setAccessToAll(m_hEvent);
  }
}

WindowsEvent::~WindowsEvent()
{
  if (m_hEvent) {
    CloseHandle(m_hEvent);
  }
}

void WindowsEvent::notify()
{
  if (m_hEvent) {
    SetEvent(m_hEvent);
  }
}

void WindowsEvent::waitForEvent(DWORD milliseconds)
{
  if (m_hEvent) {
    WaitForSingleObject(m_hEvent, milliseconds);
  }
}

void WindowsEvent::setAccessToAll(HANDLE objHandle)
{
  DWORD errorCode = SetSecurityInfo(objHandle, SE_KERNEL_OBJECT,
                                    DACL_SECURITY_INFORMATION, // Modify DACL
                                    0,
                                    0,
                                    0, // Pointer to DACL (0 = access to all)
                                    0);
  if (errorCode != ERROR_SUCCESS &&
      errorCode != ERROR_NO_SECURITY_ON_OBJECT) {
      QString errMess;
      errMess = QString("Cannot SetSecurityInfo with error = = %1").arg(errorCode);

     throw errMess;
  }
}

#define DEFAULT_WINDOW_CLASS_NAME "WindowClass"

MessageWindow::MessageWindow(const HINSTANCE hinst,
                             const char * windowClassName,
                             WindowMessageHandler *messageHandler)
: m_hwnd(0),
  m_hinst(hinst),
  m_windowClassName(""),
  m_messageHandler(messageHandler)
{
  if (windowClassName != 0) {
    m_windowClassName = QString(windowClassName);
  } else {
    m_windowClassName = QString(DEFAULT_WINDOW_CLASS_NAME);
  }

}

MessageWindow::~MessageWindow(void)
{
  destroyWindow();

  if (m_windowClassName != 0) {
    UnregisterClassA(m_windowClassName.toLocal8Bit().data(), m_hinst);
    //free(m_windowClassName);
  }
}

bool MessageWindow::createWindow(WindowMessageHandler *messageHandler)
{
  if (messageHandler != 0)
  {
    m_messageHandler = messageHandler;
  }

  if (regClass(m_hinst, m_windowClassName.toLocal8Bit().data()) == 0)
  {
    return false;
  }

  m_hwnd = ::CreateWindowExA((DWORD)0, m_windowClassName.toLocal8Bit().data(), "MessageWindow",
                          WS_OVERLAPPEDWINDOW, 0, 0, 1, 1,
                          0, NULL, m_hinst, this);
    qDebug() << GetLastError();
  if (m_hwnd == 0)
  {
    return false;
  }

  SetWindowLongA(m_hwnd, GWLP_USERDATA, (LONG_PTR) this);
  return true;
}

void MessageWindow::destroyWindow()
{
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
    m_hwnd = 0;
  }
}

LRESULT CALLBACK MessageWindow::staticWndProc(HWND hwnd, UINT message,
                                       WPARAM wParam, LPARAM lParam)
{
  MessageWindow *_this;
  if (message == WM_CREATE)
  {
    _this = (MessageWindow *)((CREATESTRUCT *)lParam)->lpCreateParams;
    wParam = (WPARAM)hwnd; // Pass hwnd throw wParam
  } else {
    _this = (MessageWindow *)GetWindowLongA(hwnd, GWLP_USERDATA);
  }
  if (_this != NULL) {
    bool result;
    if (_this->m_messageHandler != 0) {
      result = _this->m_messageHandler->processMessage(message,
                                                       wParam,
                                                       lParam);
    } else {
      result = _this->wndProc(message, wParam, lParam);
    }
    if (result) {
      return 0;
    }
  }

  return DefWindowProcA(hwnd, message, wParam, lParam);
}

ATOM MessageWindow::regClass(HINSTANCE hinst, char *windowClassName)
{
  WNDCLASSA wcWindowClass = {0};
  wcWindowClass.lpfnWndProc = staticWndProc;
  wcWindowClass.style = NULL;
  wcWindowClass.hInstance = m_hinst;
  wcWindowClass.lpszClassName = windowClassName;
  wcWindowClass.hCursor = NULL;
  wcWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

  return RegisterClassA(&wcWindowClass);
}

