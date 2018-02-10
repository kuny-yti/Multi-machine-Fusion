#ifndef TEXTURE_UPLOAD_H
#define TEXTURE_UPLOAD_H

#include "render_kernel.h"

class texture_upload_p;

class texture_upload :public render_kernel
{
    texture_upload_p  *d;
public:
    texture_upload();

    uint &texture_bgra32();

    void init(const QSize &size);
    void deinit();

    void upload(const QSize &size, uchar *plane);
};

#endif // TEXTURE_UPLOAD_H
