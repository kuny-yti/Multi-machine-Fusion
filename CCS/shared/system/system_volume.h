#ifndef SYSTEM_VOLUME_H
#define SYSTEM_VOLUME_H
#include <QString>
#include "system_type.h"

class system_volume_p;
class system_volume
{
    system_type *os_tyep;
    system_volume_p *d_ptr;
    QString cmd_str;
    uint cmd_val;
public:
    system_volume();
    ~system_volume();
    system_volume(const QString &cmd, const uint val);

    QString &cmd_table();
    uint &cmd_table_value();

    void setVolume(const ushort val);
    ushort volume() ;

    void setMute(const bool val) ;
    bool mute() ;
};

#endif // SYSTEM_VOLUME_H
