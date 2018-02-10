#ifndef SIMPLE_ENCRYPT_H
#define SIMPLE_ENCRYPT_H
#include <QString>
#include <QVector>
#include <QFlags>

class simple_encrypt
{
public:
    /**
      CompressionMode describes if compression will be applied to the data to be
      encrypted.
      */
    enum CompressionMode {
        CompressionAuto,    /*!< Only apply compression if that results in a shorter plaintext. */
        CompressionAlways,  /*!< Always apply compression. Note that for short inputs, a compression may result in longer data */
        CompressionNever    /*!< Never apply compression. */
    };
    /**
      IntegrityProtectionMode describes measures taken to make it possible to detect problems with the data
      or wrong decryption keys.

      Measures involve adding a checksum or a cryptograhpic hash to the data to be encrypted. This
      increases the length of the resulting cypertext, but makes it possible to check if the plaintext
      appears to be valid after decryption.
    */
    enum IntegrityProtectionMode {
        ProtectionNone,    /*!< The integerity of the encrypted data is not protected. It is not really possible to detect a wrong key, for instance. */
        ProtectionChecksum,/*!< A simple checksum is used to verify that the data is in order. If not, an empty string is returned. */
        ProtectionHash     /*!< A cryptographic hash is used to verify the integrity of the data. This method produces a much stronger, but longer check */
    };
    /**
      Error describes the type of error that occured.
      */
    enum Error {
        ErrorNoError,         /*!< No error occurred. */
        ErrorNoKeySet,        /*!< No key was set. You can not encrypt or decrypt without a valid key. */
        ErrorUnknownVersion,  /*!< The version of this data is unknown, or the data is otherwise not valid. */
        ErrorIntegrityFailed /*!< The integrity check of the data failed. Perhaps the wrong key was used. */
    };
public:
   simple_encrypt();

   explicit simple_encrypt(quint64 key);

   QString xorEncDec(const QString &str, const char &key);
   /**
     (Re-) initializes the key with the given @arg key.
     */
   void setKey(quint64 key);
   /**
     Returns true if SimpleCrypt has been initialized with a key.
     */
   bool hasKey() const {return !m_keyParts.isEmpty();}

   void setCompressionMode(CompressionMode mode) {m_compressionMode = mode;}

   CompressionMode compressionMode() const {return m_compressionMode;}

   void setIntegrityProtectionMode(IntegrityProtectionMode mode) {m_protectionMode = mode;}

   IntegrityProtectionMode integrityProtectionMode() const {return m_protectionMode;}

   /**
     Returns the last error that occurred.
     */
   Error lastError() const {return m_lastError;}

   /**
     Encrypts the @arg plaintext string with the key the class was initialized with, and returns
     a cyphertext the result. The result is a base64 encoded version of the binary array that is the
     actual result of the string, so it can be stored easily in a text format.
     */
   QString encryptToString(const QString& plaintext) ;
   /**
     Encrypts the @arg plaintext QByteArray with the key the class was initialized with, and returns
     a cyphertext the result. The result is a base64 encoded version of the binary array that is the
     actual result of the encryption, so it can be stored easily in a text format.
     */
   QString encryptToString(QByteArray plaintext) ;
   /**
     Encrypts the @arg plaintext string with the key the class was initialized with, and returns
     a binary cyphertext in a QByteArray the result.

     This method returns a byte array, that is useable for storing a binary format. If you need
     a string you can store in a text file, use encryptToString() instead.
     */
   QByteArray encryptToByteArray(const QString& plaintext) ;
   /**
     Encrypts the @arg plaintext QByteArray with the key the class was initialized with, and returns
     a binary cyphertext in a QByteArray the result.

     This method returns a byte array, that is useable for storing a binary format. If you need
     a string you can store in a text file, use encryptToString() instead.
     */
   QByteArray encryptToByteArray(QByteArray plaintext) ;

   /**
     Decrypts a cyphertext string encrypted with this class with the set key back to the
     plain text version.

     If an error occured, such as non-matching keys between encryption and decryption,
     an empty string or a string containing nonsense may be returned.
     */
   QString decryptToString(const QString& cyphertext) ;
   /**
     Decrypts a cyphertext string encrypted with this class with the set key back to the
     plain text version.

     If an error occured, such as non-matching keys between encryption and decryption,
     an empty string or a string containing nonsense may be returned.
     */
   QByteArray decryptToByteArray(const QString& cyphertext) ;
   /**
     Decrypts a cyphertext binary encrypted with this class with the set key back to the
     plain text version.

     If an error occured, such as non-matching keys between encryption and decryption,
     an empty string or a string containing nonsense may be returned.
     */
   QString decryptToString(QByteArray cypher) ;
   /**
     Decrypts a cyphertext binary encrypted with this class with the set key back to the
     plain text version.

     If an error occured, such as non-matching keys between encryption and decryption,
     an empty string or a string containing nonsense may be returned.
     */
   QByteArray decryptToByteArray(QByteArray cypher) ;

   //enum to describe options that have been used for the encryption. Currently only one, but
   //that only leaves room for future extensions like adding a cryptographic hash...
   enum CryptoFlag{CryptoFlagNone = 0,
                   CryptoFlagCompression = 0x01,
                   CryptoFlagChecksum = 0x02,
                   CryptoFlagHash = 0x04
                  };
   Q_DECLARE_FLAGS(CryptoFlags, CryptoFlag);
private:

   void splitKey();

   quint64 m_key;
   QVector<char> m_keyParts;
   CompressionMode m_compressionMode;
   IntegrityProtectionMode m_protectionMode;
   Error m_lastError;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(simple_encrypt::CryptoFlags)
#endif // SIMPLE_ENCRYPT_H
