#include "tcp_comm_implement.h"
#include <QDataStream>

tcp_comm_implement::tcp_comm_implement()
{
    clean_byte();
    clean_struct();
}
tcp_comm_implement::~tcp_comm_implement()
{

}

static QByteArray _encry_struct(tcp_data_struct & data, quint64 key)
{
    QByteArray tcp_data;
    simple_encrypt _encry;

    _encry.setKey (key);
    _encry.setCompressionMode (simple_encrypt::CompressionAlways);
    _encry.setIntegrityProtectionMode(simple_encrypt::ProtectionHash);

    QDataStream in(&tcp_data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_1);

    in << data.src_addres;///写入源地址
    in << data.dst_addres;///写入目标地址
    in << data.ser_addres;///写入服务器地址
    in << (qint32)data.command;///写入发送数据类型
    in << (qint32)data.protocol;///写入发送命令
    in << data.data;///写入发送数据
    tcp_data = _encry.encryptToByteArray(tcp_data);///数据加密
    return tcp_data;
}
static QByteArray _summary(QByteArray & data, quint64 key)
{
    QByteArray tcp_out;

    QDataStream out(&tcp_out, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);

    out << key;///写入加密key[2]
    out << data;///写入加密数据[3]
    return tcp_out;
}
static QByteArray _ciphertext_summary(tcp_data_struct & data)
{
    QByteArray tcp_data;
    QByteArray tcp_out;

    random_number _random;
    quint64 k = _random.genrand_int64 ();//随机key

    tcp_data = _encry_struct(data, k);

    QDataStream out(&tcp_out, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);

    out << _summary(tcp_data, k);
    return tcp_out;
}

static quint64 _key(QByteArray &data)
{
    quint64 key = 0;
    quint32 k1 = 0;
    QByteArray out_s;
    QDataStream out(&data, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_1);

    out >> k1 >>key  >> out_s;
    data = out_s;
    return key;
}
static void _ciphertext_analyze(tcp_data_struct &out_data, QByteArray & data)
{
    QByteArray temp = data;
    quint64 key = _key(temp);
    QByteArray tcp_data;
    simple_encrypt _encry;
    qint32 type = 0;

    _encry.setKey (key);
    _encry.setCompressionMode (simple_encrypt::CompressionAlways);
    _encry.setIntegrityProtectionMode(simple_encrypt::ProtectionHash);

    tcp_data = _encry.decryptToByteArray (temp);

    QDataStream out(&tcp_data, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_1);

    out >> out_data.src_addres;
    out >> out_data.dst_addres;
    out >> out_data.ser_addres;
    out >> type; out_data.command = (command_enum)type;
    out >> type; out_data.protocol = (protocol_enum)type;
    out >> out_data.data;
}

void tcp_comm_implement::_analyze()
{
    clean_struct();

    if (!tcp_data.isEmpty ())
    {
        _ciphertext_analyze(anal_data, tcp_data);
        clean_byte();
    }

}
void tcp_comm_implement::_summary()
{
    clean_byte();
    tcp_data = _ciphertext_summary(anal_data);
}

tcp_data_struct *tcp_comm_implement::data_struct()
{
    return &anal_data;
}
void tcp_comm_implement::clean_struct()
{
    anal_data.src_addres = QString();//发送方 地址
    anal_data.dst_addres = QString();//目的方 地址(有多个地址时用';'隔开)
    anal_data.ser_addres = QString();//服务器 地址
    anal_data.command = Cmd_Null;//发送数据类型
    anal_data.protocol = Prot_Null;//命令
    anal_data.data = QByteArray();//发送数据
}
void tcp_comm_implement::clean_byte()
{
    tcp_data.clear();
}
QByteArray tcp_comm_implement::data_byte()
{
    return tcp_data;
}

void tcp_comm_implement::analyze(const QByteArray &d)
{
    tcp_data = d;
    _analyze();
}
void tcp_comm_implement::summary(tcp_data_struct *dat)
{
    if (!dat)
    {
        _summary();
    }
    else
    {
        anal_data.src_addres = dat->src_addres;//发送方 地址
        anal_data.dst_addres = dat->dst_addres;//目的方 地址(有多个地址时用';'隔开)
        anal_data.ser_addres = dat->ser_addres;//服务器 地址
        anal_data.command = dat->command;//发送数据类型
        anal_data.protocol = dat->protocol;//命令
        anal_data.data = dat->data;//发送数据

        _summary();
    }
}

QString tcp_comm_implement::source(const QString &addr)
{
    if (!addr.isEmpty())
    {
        anal_data.src_addres = addr;
    }
    return anal_data.src_addres;
}
QStringList tcp_comm_implement::target(const QStringList &addr)
{
    if (!addr.isEmpty())
    {
        anal_data.dst_addres.clear();
        for (int i = 0; i < addr.size(); i++)
        {
            anal_data.dst_addres += addr[i];
            if (i != addr.size()-1)
            {
                anal_data.dst_addres += ";";
            }
        }
    }
    QStringList list = anal_data.dst_addres.split(";");
    return list;
}
QString tcp_comm_implement::server(const QString &addr)
{
    if (!addr.isEmpty())
    {
        anal_data.ser_addres = addr;
    }
    return anal_data.ser_addres;
}
QByteArray tcp_comm_implement::data(const QByteArray &addr)
{
    if (!addr.isEmpty())
    {
        anal_data.data = addr;
    }
    return anal_data.data;
}
command_enum tcp_comm_implement::command(command_enum ty)
{
    if (ty)
    {
        anal_data.command = ty;
    }
    return anal_data.command;
}
protocol_enum tcp_comm_implement::protocol(const protocol_enum cmd)
{
    if (cmd != Prot_Null)
    {
       anal_data.protocol =  cmd;
    }
    return anal_data.protocol;
}
