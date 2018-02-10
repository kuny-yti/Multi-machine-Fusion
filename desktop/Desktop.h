#ifndef F_DESKTOP_H
#define F_DESKTOP_H

#include "ScreenClient.h"
#include <QImage>

class Desktop
{
public:
    Desktop();
    ~Desktop();

    void apply_new();
    bool is_init();
    void init();
    QSize size()const;
    QImage image()const;
private:
    ScreenClient *Screen;
    Dimension dim;
};

#endif // F_DESKTOP_H
