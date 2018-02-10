#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <QByteArray>

class serialize
{
    friend class network_sync;
    class network_sync *sync;
public:
    explicit serialize(class network_sync *net);

    //! 保存为文件或读取文件(将类数据保存或初始化)
    int save(const QString &filepath);
    int read(const QString &filepath);

    //! 将数据转换成数组或将数组转换为内部数据
    QByteArray byte();
    void byte(const QByteArray &);
};

#endif // SERIALIZE_H
