#include "texture_upload.h"
#include <QOpenGLBuffer>

static int next_multiple_of_4(int x)
{
    return (x / 4 + (x % 4 == 0 ? 0 : 1)) * 4;
}

class texture_upload_p
{
public:
    uint _input_bgra32_tex;      // 格式
    QOpenGLBuffer *_input_pbo;      // 纹理上传像素缓冲区对象
    QSize _size;
};

texture_upload::texture_upload():
    d(new texture_upload_p)
{
    d->_input_pbo = 0;
    d->_input_bgra32_tex  =0;
}

uint &texture_upload::texture_bgra32()
{
    return d->_input_bgra32_tex;
}

void texture_upload::init(const QSize &size)
{
    int bytes_per_pixel = 4;
    d->_size = size;

    d->_input_pbo = new QOpenGLBuffer(QOpenGLBuffer::PixelUnpackBuffer);
    d->_input_pbo->create();
    d->_input_pbo->bind ();
    d->_input_pbo->setUsagePattern(QOpenGLBuffer::StreamDraw);
    int row_size = next_multiple_of_4(d->_size.width() * bytes_per_pixel);
    d->_input_pbo->allocate(row_size * d->_size.height());
    d->_input_pbo->release();

    d->_input_bgra32_tex = texture_gen(size,
                                   PIXEL_RGBA, DATA_BGRA,
                                   DATA_UNSIGNED_BYTE,
                                      NULL);
     texture_wrap(WRAP_CLAMP, WRAP_CLAMP);
     texture_filter(FILTER_LINEAR, FILTER_LINEAR);
}
void texture_upload::deinit()
{
    if (d->_input_pbo)
        delete d->_input_pbo;
    d->_input_pbo = 0;
    if (d->_input_bgra32_tex != 0)
    {
        texture_del(d->_input_bgra32_tex);
        d->_input_bgra32_tex = 0;
    }
    d->_size = QSize();
}

void texture_upload::upload(const QSize &size, uchar *plane)
{
    int bytes_per_pixel = 4;

    if (size != d->_size)
    {
        deinit();
        init(size);
    }
    int row_size = next_multiple_of_4(size.width() * bytes_per_pixel);
    d->_input_pbo->bind();

    void *pboptr = d->_input_pbo->map(QOpenGLBuffer::WriteOnly);
    if (!pboptr)
        throw QString("Cannot create a PBO buffer.");
    Q_ASSERT(reinterpret_cast<uintptr_t>(pboptr) % 4 == 0);

    memcpy(pboptr, plane ,row_size * d->_size.height());

    d->_input_pbo->unmap();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, row_size / bytes_per_pixel);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    activate_texture(MULTI_TEXTURE_0);
    texture_sub( d->_input_bgra32_tex, d->_size, DATA_BGRA, DATA_UNSIGNED_BYTE, NULL);

    d->_input_pbo->release();
}
