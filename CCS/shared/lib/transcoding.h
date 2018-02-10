#ifndef TRANSCODING_H
#define TRANSCODING_H
#include <QString>
#include <QByteArray>

class transcoding
{
public:
    transcoding();

    //10进制转2进制字符串两个字节
    static QString DecimalToStrBin2(int decimal);
    //10进制转2进制字符串一个字节
    static QString DecimalToStrBin1(int decimal);
    //16进制字符串转2进制字符串
    static QString StrHexToStrBin(QString strHex);
    //2进制字符串转10进制
    static int StrBinToDecimal(QString strBin);
    //10进制字符串转10进制
    static int StrDecimalToDecimal(QString strDecimal);
    //16进制字符串转10进制
    static int StrHexToDecimal(QString strHex);
    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(QByteArray data);
    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(QString str);

    static char ConvertHexChar(char ch);
};

#endif // TRANSCODING_H
