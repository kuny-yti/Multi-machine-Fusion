#include "xp_volume_p.h"

#include <QObject>
#include <windows.h>
#include <mmsystem.h>

#define MINIMUM_VOLUME  0
#define MAXIMUM_VOLUME  65535
#define PIECE_VOLUME MAXIMUM_VOLUME/655   //用户输入值每变化一个单位，系统音量的变化幅度
#define SET_MAX_VALUE 100          //用户输入的最大设置值
#define SET_MIDDLE_VALUE 50
#define SET_MUTE_VALUE 0           //用户输入的最小设置值，也是静音的音量值

class volume_sys
{
public:
    explicit volume_sys();
    ~volume_sys();
    //打开设备
    BOOL InitMixer(HMIXER *m_hMixer, HWND hwnd);
    //获得当前系统音量值
    BOOL GetVolume(HMIXER m_hMixer,ushort* pdwValue, BOOL bMono = FALSE);
    //设置音量值
    BOOL SetVolumeIn(HMIXER m_hMixer,ushort dwValue,BOOL bMono = FALSE);
    //使系统音量静音/取消静音设置
    BOOL SetMuteIn(HMIXER m_hMixer, BOOL bValue);
    //检测当前系统音量是否静音
    BOOL GetMute(HMIXER m_hMixer, BOOL* PbValue);

public :
    //通过输入设置音量大小
   void setVolume(const ushort value);
   ushort getVolume();
    //通过点击均匀调节音量大小
   void upVolume();
   void downVolume();
    //使系统静音
   void setMute();
   bool getMute();
private:
    HMIXER m_mixerHandle;
    HWND m_hwnd;
};

static volume_sys *_volume = 0;
xp_volume_p::xp_volume_p()
{
    _volume = new volume_sys();
}
xp_volume_p::~xp_volume_p()
{
    delete _volume;
}

void xp_volume_p::setVolume(const ushort val)
{
    _volume->setVolume(val);
}
ushort xp_volume_p::volume()
{
    return _volume->getVolume();
}

void xp_volume_p::setMute(const bool)
{
    _volume->setMute();
}
bool xp_volume_p::mute()
{
    return _volume->getMute();
}

volume_sys::volume_sys()
{
    (this->InitMixer(&m_mixerHandle, m_hwnd));
}

volume_sys::~volume_sys()
{
}

/*
 函数功能：通过用户输入来设置系统音量大小
 输入参数：取值范围：0-655
*/
void volume_sys::setVolume(const ushort value)
{
    if((SET_MUTE_VALUE <= value) && (value <= SET_MAX_VALUE))
    {
        SetVolumeIn(m_mixerHandle, value);
    }
}
ushort volume_sys::getVolume()
{
    ushort value = 0;
    GetVolume(m_mixerHandle, &value);
    return value;
}
/*
 函数功能：通过用户点击“+”按钮均匀调节系统音量
         每点击一次，系统音量增加100.
 参数检验：当到达最大音量时，会有相关提示
*/
void volume_sys::upVolume()
{
    ushort Volume;
    GetVolume(m_mixerHandle, &Volume);

    if(Volume < SET_MAX_VALUE)
     {
       Volume = Volume + 1;
       SetVolumeIn(m_mixerHandle,Volume);
     }
}
/*
 函数功能：通过用户点击“-”按钮均匀调节系统音量
         每点击一次，系统音量增加100.
 参数检验：当到达最小音量时，会有相关提示
*/
void volume_sys::downVolume()
{
    ushort Volume;
    GetVolume(m_mixerHandle, &Volume);
    if(Volume > SET_MUTE_VALUE)
     {
       Volume = Volume - 1;
       SetVolumeIn(m_mixerHandle,Volume);
     }
}
/*
 函数功能：通过用户点击“setMute”或者成为“closeMute”按钮设置或者取消系统静音
 操作步骤：程序启动开始，就开始检测当前的混合音设置状态，若已静音，则mute按钮内容为“closeMute”
         当用户点击这个按钮时，系统会取消静音。若没静音，同理
*/
void volume_sys::setMute()
{
    BOOL PbValue;
    if(GetMute(m_mixerHandle,&PbValue))
    {
        if(PbValue)    //目前已经静音，且用户要求恢复有声设置
        {
            SetMuteIn(m_mixerHandle,FALSE);
        }
        else if((!PbValue)) //目前系统有声，但是用户要求静音
        {
             SetMuteIn(m_mixerHandle, TRUE);
        }

    }

}
bool volume_sys::getMute()
{
    BOOL PbValue;
    GetMute(m_mixerHandle,&PbValue);
    return PbValue;
}
/*----------------------------------------------------windows音量设置调节函数------------------------------------------------------------------------*/
/*
函数功能：打开音量设备，并获得当前的系统音量信息
*/
BOOL volume_sys::InitMixer(HMIXER *m_hMixer, HWND hwnd)
{
    //初始化声音控制
   MIXERCAPS mxcaps;    //混频器容量
   ZeroMemory(&mxcaps, sizeof(MIXERCAPS));
   if (mixerGetNumDevs() != 0)
   {
       if (mixerOpen(m_hMixer, 0, (DWORD)hwnd, NULL, 0)!= MMSYSERR_NOERROR) //因为第三个参数的不对出错了
       {
           return FALSE;
       }
       //The mixerGetDevCaps function queries a specified mixer device to determine its capabilities
       if (mixerGetDevCaps((UINT)*m_hMixer, &mxcaps, sizeof(MIXERCAPS))!= MMSYSERR_NOERROR)
       {
          return FALSE;
      }
   }
   if (*m_hMixer == NULL)
   {
       return FALSE;
   }
   //检查系统静音设置
   BOOL PbValue;
   if(GetMute(m_mixerHandle,&PbValue))
   {
   }
   else
   {
       return FALSE;
   }
   //获得系统现在的音量
   ushort pdwValue;
   if(GetVolume(m_mixerHandle,&pdwValue))
   {
   }
   else
   {
       return FALSE;
   }
   return TRUE;
}
/*
函数功能：获得当前系统的音量大小
*/
BOOL volume_sys::GetVolume(HMIXER m_hMixer,  ushort* pdwValue, BOOL bMono)
{
   MIXERLINE mxl;       //混频器线路数
   mxl.cbStruct = sizeof(MIXERLINE);//必须初始化
   mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
   if (mixerGetLineInfo((HMIXEROBJ)m_hMixer,&mxl,
       MIXER_OBJECTF_HMIXER |MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR)
       return FALSE;

   DWORD m_controlnum = mxl.cChannels; //声道数量

   LPMIXERCONTROL pmxc;
   DWORD cbmxctrl = sizeof(MIXERCONTROL)*mxl.cControls;
   pmxc = (LPMIXERCONTROL)LocalAlloc(LPTR,cbmxctrl);
   MIXERLINECONTROLS mxlc;
   mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
   mxlc.dwLineID = mxl.dwLineID;
   mxlc.dwControlType =MIXERCONTROL_CONTROLTYPE_VOLUME;
   mxlc.cControls =mxl.cControls;
   mxlc.cbmxctrl = cbmxctrl;
   mxlc.pamxctrl = pmxc;
   if (mixerGetLineControls((HMIXEROBJ)m_hMixer,
       &mxlc,MIXER_OBJECTF_HMIXER |MIXER_GETLINECONTROLSF_ONEBYTYPE )!= MMSYSERR_NOERROR)
   {
       LocalFree(pmxc);
       return FALSE;
   }
   DWORD m_controlid = pmxc->dwControlID;
   long m_min=pmxc->Bounds.lMinimum;
   long m_max= pmxc->Bounds.lMaximum;
   LocalFree(pmxc);

   MIXERCONTROLDETAILS_SIGNED mxcdVolume;
   MIXERCONTROLDETAILS mxcd;
   mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
   mxcd.dwControlID = m_controlid;
   mxcd.cChannels = m_controlnum;
   mxcd.cMultipleItems = 0;
   mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_SIGNED);
   mxcd.paDetails = &mxcdVolume;
   if (mixerGetControlDetails((HMIXEROBJ)m_hMixer,
       &mxcd,MIXER_OBJECTF_HMIXER |
       MIXER_GETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
       return FALSE;
   *pdwValue =mxcdVolume.lValue/655;
   return TRUE;
}
/*
函数功能：设置当前系统音量大小
*/
BOOL volume_sys::SetVolumeIn(HMIXER m_hMixer,ushort dwValue, BOOL bMono)
{
   MIXERLINE mxl;       //混频器线路数
   mxl.cbStruct = sizeof(MIXERLINE);//必须初始化
   mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
   if (mixerGetLineInfo((HMIXEROBJ)m_hMixer,&mxl,
       MIXER_OBJECTF_HMIXER |MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR)
       return FALSE;

    DWORD m_controlnum = mxl.cChannels; //声道数量

   LPMIXERCONTROL pmxc;
   DWORD cbmxctrl = sizeof(MIXERCONTROL)*mxl.cControls;
   pmxc = (LPMIXERCONTROL)LocalAlloc(LPTR,cbmxctrl);
   MIXERLINECONTROLS mxlc;
   mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
   mxlc.dwLineID = mxl.dwLineID;
   mxlc.dwControlType =MIXERCONTROL_CONTROLTYPE_VOLUME;
   mxlc.cControls =mxl.cControls;
   mxlc.cbmxctrl =cbmxctrl;
   mxlc.pamxctrl = pmxc;
   if (mixerGetLineControls((HMIXEROBJ)m_hMixer,
       &mxlc,MIXER_OBJECTF_HMIXER |MIXER_GETLINECONTROLSF_ONEBYTYPE )!= MMSYSERR_NOERROR)
   {
       LocalFree(pmxc);
       return FALSE;
   }
   DWORD m_controlid = pmxc->dwControlID;
   long m_min=pmxc->Bounds.lMinimum;
   long m_max= pmxc->Bounds.lMaximum;
   LocalFree(pmxc);

   MIXERCONTROLDETAILS_SIGNED mxcdVolume;
   MIXERCONTROLDETAILS mxcd;
   mxcdVolume.lValue=dwValue*655;
   mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
   mxcd.dwControlID = m_controlid;
   mxcd.cChannels = 1;
   mxcd.cMultipleItems = 0;

   mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_SIGNED);
   mxcd.paDetails = &mxcdVolume;
   if (mixerSetControlDetails((HMIXEROBJ)m_hMixer,
       &mxcd,MIXER_OBJECTF_HMIXER |
       MIXER_SETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
       return FALSE;

   return TRUE;
}
/*
函数功能：设置/取消当前系统静音
*/
BOOL volume_sys::SetMuteIn(HMIXER m_hMixer,BOOL bValue)
{
   MIXERLINE mxl;       //混频器线路数
   mxl.cbStruct = sizeof(MIXERLINE);//必须初始化
   mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
   if (mixerGetLineInfo((HMIXEROBJ)m_hMixer,&mxl,
       MIXER_OBJECTF_HMIXER |MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR)
       return FALSE;

   DWORD m_Channelnum = mxl.cChannels; //声道数量

   LPMIXERCONTROL pmxc;
   DWORD cbmxctrl = sizeof(MIXERCONTROL)*mxl.cControls;
   pmxc = (LPMIXERCONTROL)LocalAlloc(LPTR,cbmxctrl);
   MIXERLINECONTROLS mxlc;
   mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
   mxlc.dwLineID = mxl.dwLineID;
   mxlc.dwControlType =MIXERCONTROL_CONTROLTYPE_MUTE;
   mxlc.cControls =mxl.cControls;
   mxlc.cbmxctrl = cbmxctrl;
   mxlc.pamxctrl = pmxc;
   if (mixerGetLineControls((HMIXEROBJ)m_hMixer,
       &mxlc,MIXER_OBJECTF_HMIXER |MIXER_GETLINECONTROLSF_ONEBYTYPE )!= MMSYSERR_NOERROR)
   {
       LocalFree(pmxc);
       return FALSE;
   }

   DWORD m_controlid = pmxc->dwControlID;
   LocalFree(pmxc);


   MIXERCONTROLDETAILS_BOOLEAN  mxcd_f; mxcd_f.fValue = bValue;
   MIXERCONTROLDETAILS mxcd;
   mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
   mxcd.dwControlID = m_controlid;
   mxcd.cChannels = 1;
   mxcd.cMultipleItems = 0;
   mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
   mxcd.paDetails = &mxcd_f;
   if (mixerSetControlDetails((HMIXEROBJ)m_hMixer,
       &mxcd,MIXER_OBJECTF_HMIXER |
       MIXER_SETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
   {

       return FALSE;
   }

   return TRUE;
}
/*
函数功能：检测当前系统是否静音
*/
BOOL volume_sys::GetMute(HMIXER m_hMixer, BOOL* PbValue)
{
   MIXERLINE mxl;       //混频器线路数
   mxl.cbStruct = sizeof(MIXERLINE);//必须初始化
   mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
   if (mixerGetLineInfo((HMIXEROBJ)m_hMixer,&mxl,
       MIXER_OBJECTF_HMIXER |MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR)
       return FALSE;

   DWORD m_controlnum = mxl.cChannels; //声道数量

   LPMIXERCONTROL pmxc;
   DWORD cbmxctrl = sizeof(MIXERCONTROL)*mxl.cControls;
   pmxc = (LPMIXERCONTROL)LocalAlloc(LPTR,cbmxctrl);
   MIXERLINECONTROLS mxlc;
   mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
   mxlc.dwLineID = mxl.dwLineID;
   mxlc.dwControlType =MIXERCONTROL_CONTROLTYPE_MUTE;
   mxlc.cControls =mxl.cControls;
   mxlc.cbmxctrl =cbmxctrl;
   mxlc.pamxctrl = pmxc;
   if (mixerGetLineControls((HMIXEROBJ)m_hMixer,
       &mxlc,MIXER_OBJECTF_HMIXER |MIXER_GETLINECONTROLSF_ONEBYTYPE )!= MMSYSERR_NOERROR)
   {
       LocalFree(pmxc);
       return FALSE;
   }
   DWORD m_controlid = pmxc->dwControlID;
   LocalFree(pmxc);

   MIXERCONTROLDETAILS_BOOLEAN  mxcd_f;

   MIXERCONTROLDETAILS mxcd;
   mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
   mxcd.dwControlID = m_controlid;
   mxcd.cChannels = m_controlnum;
   mxcd.cMultipleItems = 0;
   mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
   mxcd.paDetails = &mxcd_f;
   if (mixerGetControlDetails((HMIXEROBJ)m_hMixer,
       &mxcd,MIXER_OBJECTF_HMIXER |
       MIXER_GETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
       return FALSE;
   *PbValue = mxcd_f.fValue;
   return TRUE;
}
