#include "serialize.h"
#include <QFile>
#include <QDataStream>
#include "network_sync.h"

serialize::serialize(class network_sync *net):
    sync(net)
{
}
int serialize::save(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return -1;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_2);
    out << byte();
    file.close();
    return 0;
}
QByteArray serialize::byte()
{
    QByteArray byte_;

    QDataStream out(&byte_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    {
        out << this->sync->_node.size();
        for (int i = 0; i < this->sync->_node.size(); i++)
        {
            out << sync->_node[i];
            out << sync->_node_name[i];
        }
    }
    return byte_;
}
void serialize::byte(const QByteArray &ar)
{
    QByteArray byte_ = ar;

    QDataStream in(&byte_, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_2);
    int tem = 0;

    {
        in >> tem;
        this->sync->_node.reserve(tem);
        this->sync->_node_name.reserve(tem);
        for (int i = 0; i < this->sync->_node.size(); i++)
        {
            in >> sync->_node[i];
            in >> sync->_node_name[i];
        }
    }

}
int serialize::read(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_2);
    QByteArray arr;
    in >> arr;
    file.close();
    byte(arr);
    return 0;
}
