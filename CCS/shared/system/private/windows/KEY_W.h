#ifndef KEY_W_H
#define KEY_W_H

typedef enum
{
    KEY_LBUTTON= 0x01, //鼠标左键
    KEY_RBUTTON= 0x02, //鼠标右键
    KEY_CANCEL= 0x03,
    KEY_MBUTTON= 0x04, //鼠标中键
    KEY_XBUTTON1= 0x05,
    KEY_XBUTTON2= 0x06,
    KEY_BACK= 0x08, //BACKSPACE key 0x01000003
    KEY_TAB= 0x09, // TAB key 0x01000001
    KEY_CLEAR= 0x0C, //CLEAR key 0x0100000b
    KEY_RETURN= 0x0D, // ENTER key 0x01000004 (Key_Enter0x01000005)
    KEY_SHIFT= 0x10, // SHIFT key 0x01000020
    KEY_CONTROL= 0x11, // CTRL key 0x01000021
    KEY_MENU= 0x12, // ALT key Key_Alt = 0x01000023 Key_Menu = 0x01000055
    KEY_PAUSE= 0x13, // PAUSE key 0x01000008
    KEY_CAPITAL= 0x14, // CAPS LOCK key 0x01000024
    KEY_KANA= 0x15,
    KEY_HANGEUL= 0x15,
    KEY_HANGUL= 0x15,
    KEY_JUNJA= 0x17,
    KEY_FINAL= 0x18,
    KEY_HANJA= 0x19,
    KEY_KANJI= 0x19,
    KEY_ESCAPE= 0x1B, // ESC key 0x01000000
    KEY_CONVERT= 0x1C,
    KEY_NONCONVERT= 0x1D,
    KEY_ACCEPT= 0x1E,
    KEY_MODECHANGE= 0x1F,
    KEY_SPACE= 0x20, // SPACEBAR  Key_Space = 0x20
    KEY_PRIOR= 0x21, // PAGE UP key  Key_PageUp = 0x01000016
    KEY_NEXT= 0x22, // PAGE DOWN key  Key_PageDown = 0x01000017
    KEY_END= 0x23, // END key  Key_End = 0x01000011
    KEY_HOME= 0x24, // HOME key Key_Home = 0x01000010
    KEY_LEFT= 0x25, // LEFT ARROW key 0x01000012
    KEY_UP= 0x26, // UP ARROW key
    KEY_RIGHT= 0x27, // RIGHT ARROW key
    KEY_DOWN= 0x28, // DOWN ARROW key
    KEY_SELECT= 0x29, // SELECT key
    KEY_PRINT= 0x2A, // PRINT key Key_Print = 0x01000009,
    KEY_EXECUTES= 0x2B, // EXECUTE key Key_Execute = 0x01020003,
    KEY_SNAPSHOT= 0x2C, // PRINT SCREEN key
    KEY_INSERT= 0x2D, // INS key Key_Insert = 0x01000006,
    KEY_DELETE= 0x2E, // DEL key Key_Delete = 0x01000007,
    KEY_HELP= 0x2F, // HELP key Key_Help = 0x01000058,

    KEY_LWIN= 0x5B, // Left Windows key (Natural keyboard)
    KEY_RWIN= 0x5C, // Right Windows key (Natural keyboard)
    KEY_APPS= 0x5D, // Applications key (Natural keyboard)
    KEY_SLEEP= 0x5F, // Computer Sleep key
    KEY_NUMPAD0= 0x60, // Numeric keypad
    KEY_NUMPAD1= 0x61,
    KEY_NUMPAD2= 0x62,
    KEY_NUMPAD3= 0x63,
    KEY_NUMPAD4= 0x64,
    KEY_NUMPAD5= 0x65,
    KEY_NUMPAD6= 0x66,
    KEY_NUMPAD7= 0x67,
    KEY_NUMPAD8= 0x68,
    KEY_NUMPAD9= 0x69,
    KEY_MULTIPLY= 0x6A, //* Multiply key
    KEY_ADD= 0x6B, // + Add key
    KEY_SEPARATOR= 0x6C, //分隔符 Separator key
    KEY_SUBTRACT= 0x6D, // - Subtract key
    KEY_DECIMAL= 0x6E, // Decimal key
    KEY_DIVIDE= 0x6F, //. Divide key
    KEY_F1= 0x70,
    KEY_F2= 0x71,
    KEY_F3= 0x72,
    KEY_F4= 0x73,
    KEY_F5= 0x74,
    KEY_F6= 0x75,
    KEY_F7= 0x76,
    KEY_F8= 0x77,
    KEY_F9= 0x78,
    KEY_F10= 0x79,
    KEY_F11= 0x7A,
    KEY_F12= 0x7B,
    KEY_F13= 0x7C,
    KEY_F14= 0x7D,
    KEY_F15= 0x7E,
    KEY_F16= 0x7F,
    KEY_F17= 0x80,
    KEY_F18= 0x81,
    KEY_F19= 0x82,
    KEY_F20= 0x83,
    KEY_F21= 0x84,
    KEY_F22= 0x85,
    KEY_F23= 0x86,
    KEY_F24= 0x87,
    KEY_NUMLOCK= 0x90, // NUM LOCK key Key_NumLock = 0x01000025,
    KEY_SCROLL= 0x91, // SCROLL LOCK key Key_ScrollLock = 0x01000026,
    KEY_OEM_NEC_EQUAL= 0x92,
    KEY_OEM_FJ_JISHO= 0x92,
    KEY_OEM_FJ_MASSHOU= 0x93,
    KEY_OEM_FJ_TOUROKU= 0x94,
    KEY_OEM_FJ_LOYA= 0x95,
    KEY_OEM_FJ_ROYA= 0x96,
    KEY_LSHIFT= 0xA0, // Left SHIFT key
    KEY_RSHIFT= 0xA1, // Right SHIFT key
    KEY_LCONTROL= 0xA2, // Left CONTROL key
    KEY_RCONTROL= 0xA3, // Right CONTROL key
    KEY_LMENU= 0xA4, // Left MENU key
    KEY_RMENU= 0xA5, // Right MENU key
    KEY_BROWSER_BACK= 0xA6, // Browser Back key
    KEY_BROWSER_FORWARD= 0xA7,
    KEY_BROWSER_REFRESH= 0xA8,
    KEY_BROWSER_STOP= 0xA9,
    KEY_BROWSER_SEARCH= 0xAA,
    KEY_BROWSER_FAVORITES= 0xAB,
    KEY_BROWSER_HOME= 0xAC,
    KEY_VOLUME_MUTE= 0xAD, // Volume Mute key
    KEY_VOLUME_DOWN= 0xAE,
    KEY_VOLUME_UP= 0xAF,
    KEY_MEDIA_NEXT_TRACK= 0xB0, // Next Track key
    KEY_MEDIA_PREV_TRACK= 0xB1,
    KEY_MEDIA_STOP= 0xB2,
    KEY_MEDIA_PLAY_PAUSE= 0xB3,
    KEY_LAUNCH_MAIL= 0xB4, // Start Mail key
    KEY_LAUNCH_MEDIA_SELECT= 0xB5, // Select Media key
    KEY_LAUNCH_APP1= 0xB6, // Start Application 1 key
    KEY_LAUNCH_APP2= 0xB7,// Start Application 2 key
    KEY_OEM_1= 0xBA, // ; or :
    KEY_OEM_PLUS= 0xBB, // +
    KEY_OEM_COMMA= 0xBC, // ,
    KEY_OEM_MINUS= 0xBD, // -
    KEY_OEM_PERIOD= 0xBE, // .
    KEY_OEM_2= 0xBF, // / or ?
    KEY_OEM_3= 0xC0, // ` or ~
    KEY_OEM_4= 0xDB, // [ or {
    KEY_OEM_5= 0xDC, // \ or |
    KEY_OEM_6= 0xDD, // ] or }
    KEY_OEM_7= 0xDE, // ' or "
    KEY_OEM_8= 0xDF,
    KEY_OEM_AX= 0xE1,
    KEY_OEM_102= 0xE2,
    KEY_ICO_HELP= 0xE3,
    KEY_ICO_00= 0xE4,
    KEY_PROCESSKEY= 0xE5,
    KEY_ICO_CLEAR= 0xE6,
    KEY_PACKET= 0xE7,
    KEY_OEM_RESET= 0xE9,
    KEY_OEM_JUMP= 0xEA,
    KEY_OEM_PA1= 0xEB,
    KEY_OEM_PA2= 0xEC,
    KEY_OEM_PA3= 0xED,
    KEY_OEM_WSCTRL= 0xEE,
    KEY_OEM_CUSEL= 0xEF,
    KEY_OEM_ATTN= 0xF0,
    KEY_OEM_FINISH= 0xF1,
    KEY_OEM_COPY= 0xF2,
    KEY_OEM_AUTO= 0xF3,
    KEY_OEM_ENLW= 0xF4,
    KEY_OEM_BACKTAB =0xF5,
    KEY_ATTN= 0xF6,
    KEY_CRSEL= 0xF7,
    KEY_EXSEL =0xF8,
    KEY_EREOF=0xF9,
    KEY_PLAY =0xFA,
    KEY_ZOOM =0xFB, // Zoom key
    KEY_NONAME =0xFC,
    KEY_PA1 =0xFD,
    KEY_OEM_CLEAR =0xFE,
}KEY_W;

#endif // KEY_W_H
