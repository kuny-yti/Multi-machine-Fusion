#ifndef WIN7_VOLUME_P_H
#define WIN7_VOLUME_P_H
#include "private/system_volume_p.h"
class win7_volume_p: public system_volume_p
{
public:
    win7_volume_p();
    ~win7_volume_p();

    virtual void setVolume(const ushort val);
    virtual ushort volume() ;

    virtual void setMute(const bool val) ;
    virtual bool mute() ;
};

#endif // WIN7_VOLUME_P_H
