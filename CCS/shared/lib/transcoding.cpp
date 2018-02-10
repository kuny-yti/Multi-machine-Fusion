#include "transcoding.h"

transcoding::transcoding()
{
}

char transcoding::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
//16进制字符串转字节数组
QByteArray transcoding::HexStrToByteArray(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;

    for(int i =0; i < len; )
    {
        hstr= *(QString(str[i]).toUtf8().data());
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = *(QString(str[i]).toUtf8().data());
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

//字节数组转16进制字符串
QString transcoding::ByteArrayToHexStr(QByteArray data)
{
    QString temp="";
    QString hex=data.toHex();
    for (int i=0;i<hex.length();i=i+2)
    {
        temp+=hex.mid(i,2)+" ";
    }
    return temp.trimmed().toUpper();
}

//16进制字符串转10进制
int transcoding::StrHexToDecimal(QString strHex)
{
    bool ok;
    return strHex.toInt(&ok,16);
}

//10进制字符串转10进制
int transcoding::StrDecimalToDecimal(QString strDecimal)
{
    bool ok;
    return strDecimal.toInt(&ok,10);
}

//2进制字符串转10进制
int transcoding::StrBinToDecimal(QString strBin)
{
    bool ok;
    return strBin.toInt(&ok,2);
}

//16进制字符串转2进制字符串
QString transcoding::StrHexToStrBin(QString strHex)
{
    uchar decimal=StrHexToDecimal(strHex);
    QString bin=QString::number(decimal,2);

    uchar len=bin.length();
    if (len<8)
    {
        for (int i=0;i<8-len;i++)
        {
            bin="0"+bin;
        }
    }

    return bin;
}

//10进制转2进制字符串一个字节
QString transcoding::DecimalToStrBin1(int decimal)
{
    QString bin=QString::number(decimal,2);

    uchar len=bin.length();
    if (len<=8)
    {
        for (int i=0;i<8-len;i++)
        {
            bin="0"+bin;
        }
    }

    return bin;
}

//10进制转2进制字符串两个字节
QString transcoding::DecimalToStrBin2(int decimal)
{
    QString bin=QString::number(decimal,2);

    uchar len=bin.length();
    if (len<=16)
    {
        for (int i=0;i<16-len;i++)
        {
            bin="0"+bin;
        }
    }

    return bin;
}
