#ifndef SYSTEM_VOLUME_P_H
#define SYSTEM_VOLUME_P_H
#include <QtGlobal>

class system_volume_p
{
public:
    virtual ~system_volume_p(){}

    virtual void setVolume(const ushort val) = 0;
    virtual ushort volume() = 0;

    virtual void setMute(const bool val) = 0;
    virtual bool mute() = 0;
};

#endif // SYSTEM_VOLUME_P_H
