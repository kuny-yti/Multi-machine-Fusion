#include "global_shortcut.h"

class global_shortcut_p
{
public:
    global_shortcut_p();
    ~global_shortcut_p();

    bool enabled;
    Qt::Key key;
    Qt::KeyboardModifiers mods;

    bool setShortcut(const QKeySequence& _shortcut);
    bool unsetShortcut();

    static bool error;
#ifndef Q_WS_MAC
    static int ref;
    static QAbstractEventDispatcher::EventFilter prevEventFilter;
    static bool eventFilter(void* message);
#endif // Q_WS_MAC

    static void activateShortcut(quint32 nativeKey, quint32 nativeMods);

private:
    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

    static bool registerShortcut(quint32 nativeKey, quint32 nativeMods);
    static bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);

    static QHash<QPair<quint32, quint32>, global_shortcut*> shortcuts;
};

#ifdef WIN32
#include <windows.h>

bool global_shortcut_p::eventFilter(void* message)
{
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);
        activateShortcut(keycode, modifiers);
    }
    return false;
}

quint32 global_shortcut_p::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    // MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier)
        native |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier)
        native |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)
        native |= MOD_ALT;
    if (modifiers & Qt::MetaModifier)
        native |= MOD_WIN;
    // TODO: resolve these?
    //if (modifiers & Qt::KeypadModifier)
    //if (modifiers & Qt::GroupSwitchModifier)
    return native;
}

quint32 global_shortcut_p::nativeKeycode(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_Asterisk:
        return VK_MULTIPLY;
    case Qt::Key_Plus:
        return VK_ADD;
    case Qt::Key_Comma:
        return VK_SEPARATOR;
    case Qt::Key_Minus:
        return VK_SUBTRACT;
    case Qt::Key_Slash:
        return VK_DIVIDE;
    case Qt::Key_MediaNext:
        return VK_MEDIA_NEXT_TRACK;
    case Qt::Key_MediaPrevious:
        return VK_MEDIA_PREV_TRACK;
    case Qt::Key_MediaPlay:
        return VK_MEDIA_PLAY_PAUSE;
    case Qt::Key_MediaStop:
        return VK_MEDIA_STOP;
        // couldn't find those in VK_*
        //case Qt::Key_MediaLast:
        //case Qt::Key_MediaRecord:
    case Qt::Key_VolumeDown:
        return VK_VOLUME_DOWN;
    case Qt::Key_VolumeUp:
        return VK_VOLUME_UP;
    case Qt::Key_VolumeMute:
        return VK_VOLUME_MUTE;

        // numbers
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        return key;

        // letters
    case Qt::Key_A:
    case Qt::Key_B:
    case Qt::Key_C:
    case Qt::Key_D:
    case Qt::Key_E:
    case Qt::Key_F:
    case Qt::Key_G:
    case Qt::Key_H:
    case Qt::Key_I:
    case Qt::Key_J:
    case Qt::Key_K:
    case Qt::Key_L:
    case Qt::Key_M:
    case Qt::Key_N:
    case Qt::Key_O:
    case Qt::Key_P:
    case Qt::Key_Q:
    case Qt::Key_R:
    case Qt::Key_S:
    case Qt::Key_T:
    case Qt::Key_U:
    case Qt::Key_V:
    case Qt::Key_W:
    case Qt::Key_X:
    case Qt::Key_Y:
    case Qt::Key_Z:
        return key;

    default:
        return 0;
    }
}

bool global_shortcut_p::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
    return RegisterHotKey(0, nativeMods ^ nativeKey, nativeMods, nativeKey);
}

bool global_shortcut_p::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
    return UnregisterHotKey(0, nativeMods ^ nativeKey);
}

#else
#include <QX11Info>
#include <X11/Xlib.h>

static int (*original_x_errhandler)(Display* display, XErrorEvent* event);

static int qxt_x_errhandler(Display* display, XErrorEvent *event)
{
    Q_UNUSED(display);
    switch (event->error_code)
    {
        case BadAccess:
        case BadValue:
        case BadWindow:
            if (event->request_code == 33 /* X_GrabKey */ ||
                event->request_code == 34 /* X_UngrabKey */)
            {
                QxtGlobalShortcutPrivate::error = true;
                //TODO:
                //char errstr[256];
                //XGetErrorText(dpy, err->error_code, errstr, 256);
            }
        default:
            return 0;
    }
}

bool global_shortcut_p::eventFilter(void* message)
{
    XEvent* event = static_cast<XEvent*>(message);
    if (event->type == KeyPress)
    {
        XKeyEvent* key = (XKeyEvent*) event;
        activateShortcut(key->keycode,
            // Mod1Mask == Alt, Mod4Mask == Meta
            key->state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask));
    }
    return false;
}

quint32 global_shortcut_p::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    // ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, and Mod5Mask
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier)
        native |= ShiftMask;
    if (modifiers & Qt::ControlModifier)
        native |= ControlMask;
    if (modifiers & Qt::AltModifier)
        native |= Mod1Mask;
    if (modifiers & Qt::MetaModifier)
        native |= Mod4Mask;

    // TODO: resolve these?
    //if (modifiers & Qt::MetaModifier)
    //if (modifiers & Qt::KeypadModifier)
    //if (modifiers & Qt::GroupSwitchModifier)
    return native;
}

quint32 global_shortcut_p::nativeKeycode(Qt::Key key)
{
    Display* display = QX11Info::display();
    return XKeysymToKeycode(display, XStringToKeysym(QKeySequence(key).toString().toLatin1().data()));
}

bool global_shortcut_p::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    Bool owner = True;
    int pointer = GrabModeAsync;
    int keyboard = GrabModeAsync;
    error = false;
    original_x_errhandler = XSetErrorHandler(qxt_x_errhandler);
    XGrabKey(display, nativeKey, nativeMods, window, owner, pointer, keyboard);
    XGrabKey(display, nativeKey, nativeMods | Mod2Mask, window, owner, pointer, keyboard); // allow numlock
    XSync(display, False);
    XSetErrorHandler(original_x_errhandler);
    return !error;
}

bool global_shortcut_p::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    error = false;
    original_x_errhandler = XSetErrorHandler(qxt_x_errhandler);
    XUngrabKey(display, nativeKey, nativeMods, window);
    XUngrabKey(display, nativeKey, nativeMods | Mod2Mask, window); // allow numlock
    XSync(display, False);
    XSetErrorHandler(original_x_errhandler);
    return !error;
}

#endif



bool global_shortcut_p::error = false;

#ifndef Q_WS_MAC
int global_shortcut_p::ref = 0;
QAbstractEventDispatcher::EventFilter global_shortcut_p::prevEventFilter = 0;
#endif // Q_WS_MAC

QHash<QPair<quint32, quint32>, global_shortcut*> global_shortcut_p::shortcuts;

global_shortcut_p::global_shortcut_p() :
    enabled(true), key(Qt::Key(0)), mods(Qt::NoModifier)
{
#ifndef Q_WS_MAC
    if (!ref++)
        prevEventFilter =
                QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
#endif // Q_WS_MAC
}

global_shortcut_p::~global_shortcut_p()
{
#ifndef Q_WS_MAC
    if (!--ref)
        QAbstractEventDispatcher::instance()->setEventFilter(prevEventFilter);
#endif // Q_WS_MAC
}

bool global_shortcut_p::setShortcut(const QKeySequence& shortcut)
{
    Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier |
            Qt::AltModifier | Qt::MetaModifier;
    key = shortcut.isEmpty() ? Qt::Key(0) : Qt::Key((shortcut[0] ^ allMods) &
                               shortcut[0]);
    mods = shortcut.isEmpty() ? Qt::KeyboardModifiers(0) :
                                Qt::KeyboardModifiers(shortcut[0] & allMods);
    const quint32 nativeKey = nativeKeycode(key);
    const quint32 nativeMods = nativeModifiers(mods);
    const bool res = registerShortcut(nativeKey, nativeMods);
    if (res)
        shortcuts.insert(qMakePair(nativeKey, nativeMods), &qxt_p());
    else
        qWarning() << "QxtGlobalShortcut failed to register:" <<
                      QKeySequence(key + mods).toString();
    return res;
}

bool global_shortcut_p::unsetShortcut()
{
    bool res = false;
    const quint32 nativeKey = nativeKeycode(key);
    const quint32 nativeMods = nativeModifiers(mods);
    if (shortcuts.value(qMakePair(nativeKey, nativeMods)) == &qxt_p())
        res = unregisterShortcut(nativeKey, nativeMods);
    if (res)
        shortcuts.remove(qMakePair(nativeKey, nativeMods));
    else
        qWarning() << "QxtGlobalShortcut failed to unregister:" <<
                      QKeySequence(key + mods).toString();
    key = Qt::Key(0);
    mods = Qt::KeyboardModifiers(0);
    return res;
}

void global_shortcut_p::activateShortcut(quint32 nativeKey, quint32 nativeMods)
{
    global_shortcut* shortcut = shortcuts.value(qMakePair(nativeKey, nativeMods));
    if (shortcut && shortcut->isEnabled())
        emit shortcut->activated();
}

/*!
    \class QxtGlobalShortcut
    \inmodule QxtGui
    \brief The QxtGlobalShortcut class provides a global shortcut aka "hotkey".

    A global shortcut triggers even if the application is not active. This
    makes it easy to implement applications that react to certain shortcuts
    still if some other application is active or if the application is for
    example minimized to the system tray.

    Example usage:
    \code
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(window);
    connect(shortcut, SIGNAL(activated()), window, SLOT(toggleVisibility()));
    shortcut->setShortcut(QKeySequence("Ctrl+Shift+F12"));
    \endcode

    \bold {Note:} Since Qxt 0.6 QxtGlobalShortcut no more requires QxtApplication.
 */

/*!
    \fn QxtGlobalShortcut::activated()

    This signal is emitted when the user types the shortcut's key sequence.

    \sa shortcut
 */

/*!
    Constructs a new QxtGlobalShortcut with \a parent.
 */
global_shortcut::global_shortcut(QObject* parent)
        : QObject(parent),
          d(new global_shortcut())
{
}

/*!
    Constructs a new QxtGlobalShortcut with \a shortcut and \a parent.
 */
global_shortcut::global_shortcut(const QKeySequence& shortcut, QObject* parent)
        : QObject(parent),
          d(new global_shortcut())
{
    setShortcut(shortcut);
}

/*!
    Destructs the QxtGlobalShortcut.
 */
global_shortcut::~global_shortcut()
{
    if (d->key != 0)
        d->unsetShortcut();
}

/*!
    \property QxtGlobalShortcut::shortcut
    \brief the shortcut key sequence

    \bold {Note:} Notice that corresponding key press and release events are not
    delivered for registered global shortcuts even if they are disabled.
    Also, comma separated key sequences are not supported.
    Only the first part is used:

    \code
    qxtShortcut->setShortcut(QKeySequence("Ctrl+Alt+A,Ctrl+Alt+B"));
    Q_ASSERT(qxtShortcut->shortcut() == QKeySequence("Ctrl+Alt+A"));
    \endcode
 */
QKeySequence global_shortcut::shortcut() const
{
    return QKeySequence(d->key | d->mods);
}

bool global_shortcut::setShortcut(const QKeySequence& shortcut)
{
    if (d->key != 0)
        d->unsetShortcut();
    return d->setShortcut(shortcut);
}

/*!
    \property QxtGlobalShortcut::enabled
    \brief whether the shortcut is enabled

    A disabled shortcut does not get activated.

    The default value is \c true.

    \sa setDisabled()
 */
bool global_shortcut::isEnabled() const
{
    return d->enabled;
}

void global_shortcut::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

/*!
    Sets the shortcut \a disabled.

    \sa enabled
 */
void global_shortcut::setDisabled(bool disabled)
{
    d->enabled = !disabled;
}
