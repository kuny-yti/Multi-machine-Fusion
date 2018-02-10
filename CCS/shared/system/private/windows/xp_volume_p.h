#ifndef XP_VOLUME_P_H
#define XP_VOLUME_P_H
#include "private/system_volume_p.h"
class xp_volume_p : public system_volume_p
{
public:
    xp_volume_p();
    ~xp_volume_p();

    virtual void setVolume(const ushort val);
    virtual ushort volume() ;

    virtual void setMute(const bool val) ;
    virtual bool mute() ;
};

#endif // VOLUME_P_H
