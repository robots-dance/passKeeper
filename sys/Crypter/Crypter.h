#ifndef _CRYPTER_H_
#define _CRYPTER_H_

#include <QString>
#include <openssl/evp.h>
#include "types.h"

typedef unsigned char byte;
class QByteArray;

class Crypter
{
public:
	enum CryptMode { ECB = 0, CBC, CFB, OFB };
	
	Crypter(): m_changed( true ), m_iv( 0 ), key( 0 )
	{}
	
	Crypter( int keySize, cst_str pass, CryptMode mode = CBC ):
		m_keySize( keySize ),
		m_mode( mode ),
		m_pass( pass ),
		m_changed( true ),
		m_iv( 0 ),
		key( 0 )
	{}
	
	virtual ~Crypter();
	
	// [ api ]
	bool decrypt( const QByteArray &in, QByteArray *out );
	bool encrypt( const QByteArray &in, QByteArray *out );
	
	// [ getters ]
	int keySize() const {
		return m_keySize;
	}
	
	CryptMode mode() const {
		return m_mode;
	}
	
	cst_str password() const {
		return m_pass;
	}
	
	// [ setters ]
	void setKeySize( int size );
	void setMode( CryptMode mode );
	void setPass( cst_str pass );

protected:
	virtual int getBlockSize() = 0;
	virtual const EVP_CIPHER *getCipher() = 0;

private:
	void contextInit();
	// заново инициализирует контексты шифрования
	
	bool reinit();
	// вызывается если были изменены параметры шифрования
	// возвращает true, если удалось их удачно применить
	
	
	int m_keySize;
	CryptMode m_mode;
	QString m_pass;
	
	bool m_changed;
	
	static const int kRoundsCount = 6;
	
	const EVP_CIPHER *m_cipher;
	EVP_CIPHER_CTX m_encCtxt, m_decCtxt;
	byte *m_iv, *key;
};

#endif // _CRYPTER_H_

