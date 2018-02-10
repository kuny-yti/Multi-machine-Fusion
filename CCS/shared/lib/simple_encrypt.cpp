#include "simple_encrypt.h"
#include <QByteArray>
#include <QtDebug>
#include <QtGlobal>
#include <QDateTime>
#include <QDataStream>
#include <QCryptographicHash>

simple_encrypt::simple_encrypt():
    m_key(0),
    m_compressionMode(CompressionAuto),
    m_protectionMode(ProtectionChecksum),
    m_lastError(ErrorNoError)
{
    qsrand(uint(QDateTime::currentMSecsSinceEpoch() & 0xFFFF));
}

simple_encrypt::simple_encrypt(quint64 key):
    m_key(key),
    m_compressionMode(CompressionAuto),
    m_protectionMode(ProtectionChecksum),
    m_lastError(ErrorNoError)
{
    qsrand(uint(QDateTime::currentMSecsSinceEpoch() & 0xFFFF));
    splitKey();
}

QString simple_encrypt::xorEncDec(const QString &str, const char &key)
{
  QByteArray bs = str.toUtf8();

  for(int i=0; i<bs.size(); i++)
  {
    bs[i] = bs[i] ^ key;
  }

  return QString(bs);
}

void simple_encrypt::setKey(quint64 key)
{
    m_key = key;
    splitKey();
}

void simple_encrypt::splitKey()
{
    m_keyParts.clear();
    m_keyParts.resize(8);
    for (int i=0;i<8;i++)
    {
        quint64 part = m_key;
        for (int j=i; j>0; j--)
            part = part >> 8;
        part = part & 0xff;
        m_keyParts[i] = static_cast<char>(part);
    }
}

QByteArray simple_encrypt::encryptToByteArray(const QString& plaintext)
{
    QByteArray plaintextArray = plaintext.toUtf8();
    return encryptToByteArray(plaintextArray);
}

QByteArray simple_encrypt::encryptToByteArray(QByteArray plaintext)
{
    if (m_keyParts.isEmpty())
    {
        qWarning() << "No key set.";
        m_lastError = ErrorNoKeySet;
        return QByteArray();
    }


    QByteArray ba = plaintext;

    CryptoFlags flags = CryptoFlagNone;
    if (m_compressionMode == CompressionAlways)
    {
        ba = qCompress(ba, 9); //maximum compression
        flags |= CryptoFlagCompression;
    }
    else if (m_compressionMode == CompressionAuto)
    {
        QByteArray compressed = qCompress(ba, 9);
        if (compressed.count() < ba.count())
        {
            ba = compressed;
            flags |= CryptoFlagCompression;
        }
    }

    QByteArray integrityProtection;
    if (m_protectionMode == ProtectionChecksum)
    {
        flags |= CryptoFlagChecksum;
        QDataStream s(&integrityProtection, QIODevice::WriteOnly);
        s << qChecksum(ba.constData(), ba.length());
    }
    else if (m_protectionMode == ProtectionHash)
    {
        flags |= CryptoFlagHash;
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(ba);

        integrityProtection += hash.result();
    }

    //prepend a random char to the string
    char randomChar = char(qrand() & 0xFF);
    ba = randomChar + integrityProtection + ba;

    int pos(0);
    char lastChar(0);

    int cnt = ba.count();

    while (pos < cnt)
    {
        ba[pos] = ba.at(pos) ^ m_keyParts.at(pos % 8) ^ lastChar;
        lastChar = ba.at(pos);
        ++pos;
    }

    QByteArray resultArray;
    resultArray.append(char(0x03));  //version for future updates to algorithm
    resultArray.append(char(flags)); //encryption flags
    resultArray.append(ba);

    m_lastError = ErrorNoError;
    return resultArray;
}

QString simple_encrypt::encryptToString(const QString& plaintext)
{
    QByteArray plaintextArray = plaintext.toUtf8();
    QByteArray cypher = encryptToByteArray(plaintextArray);
    QString cypherString = QString::fromUtf8(cypher.toBase64());
    return cypherString;
}

QString simple_encrypt::encryptToString(QByteArray plaintext)
{
    QByteArray cypher = encryptToByteArray(plaintext);
    QString cypherString = QString::fromUtf8(cypher.toBase64());
    return cypherString;
}

QString simple_encrypt::decryptToString(const QString &cyphertext)
{
    QByteArray cyphertextArray = QByteArray::fromBase64(cyphertext.toUtf8());
    QByteArray plaintextArray = decryptToByteArray(cyphertextArray);
    QString plaintext = QString::fromUtf8(plaintextArray, plaintextArray.size());

    return plaintext;
}

QString simple_encrypt::decryptToString(QByteArray cypher)
{
    QByteArray ba = decryptToByteArray(cypher);
    QString plaintext = QString::fromUtf8(ba, ba.size());

    return plaintext;
}

QByteArray simple_encrypt::decryptToByteArray(const QString& cyphertext)
{
    QByteArray cyphertextArray = QByteArray::fromBase64(cyphertext.toUtf8());
    QByteArray ba = decryptToByteArray(cyphertextArray);

    return ba;
}

QByteArray simple_encrypt::decryptToByteArray(QByteArray cypher)
{
    if (m_keyParts.isEmpty()) {
        qWarning() << "No key set.";
        m_lastError = ErrorNoKeySet;
        return QByteArray();
    }

    QByteArray ba = cypher;

    if( cypher.count() < 3 )
        return QByteArray();

    char version = ba.at(0);

    if (version !=3)
    {  //we only work with version 3
        m_lastError = ErrorUnknownVersion;
        qWarning() << "Invalid version or not a cyphertext.";
        return QByteArray();
    }

    CryptoFlags flags = CryptoFlags(ba.at(1));

    ba = ba.mid(2);
    int pos(0);
    int cnt(ba.count());
    char lastChar = 0;

    while (pos < cnt)
    {
        char currentChar = ba[pos];
        ba[pos] = ba.at(pos) ^ lastChar ^ m_keyParts.at(pos % 8);
        lastChar = currentChar;
        ++pos;
    }

    ba = ba.mid(1); //chop off the random number at the start

    bool integrityOk(true);
    if (flags.testFlag(CryptoFlagChecksum))
    {
        if (ba.length() < 2)
        {
            m_lastError = ErrorIntegrityFailed;
            return QByteArray();
        }
        quint16 storedChecksum;
        {
            QDataStream s(&ba, QIODevice::ReadOnly);
            s >> storedChecksum;
        }
        ba = ba.mid(2);
        quint16 checksum = qChecksum(ba.constData(), ba.length());
        integrityOk = (checksum == storedChecksum);
    }
    else if (flags.testFlag(CryptoFlagHash))
    {
        if (ba.length() < 20)
        {
            m_lastError = ErrorIntegrityFailed;
            return QByteArray();
        }
        QByteArray storedHash = ba.left(20);
        ba = ba.mid(20);
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(ba);
        integrityOk = (hash.result() == storedHash);
    }

    if (!integrityOk)
    {
        m_lastError = ErrorIntegrityFailed;
        return QByteArray();
    }

    if (flags.testFlag(CryptoFlagCompression))
        ba = qUncompress(ba);

    m_lastError = ErrorNoError;
    return ba;
}
