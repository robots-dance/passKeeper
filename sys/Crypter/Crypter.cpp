#include "Crypter.h"

Crypter::~Crypter()
{
	if (m_iv != NULL ) {
		free( m_iv );
	}
	if ( key != NULL ) {
		free( key );
	}
}

void Crypter::contextInit()
{
	// инициализация контекста шифрования 
	EVP_CIPHER_CTX_init( &m_encCtxt );
	EVP_EncryptInit_ex( &m_encCtxt, m_cipher, 0, key, m_iv );
	
	// инициализация контекста дешифрования
	EVP_CIPHER_CTX_init( &m_decCtxt );
	EVP_DecryptInit_ex( &m_decCtxt, m_cipher, 0, key, m_iv );
}

bool Crypter::decrypt( const QByteArray &in, QByteArray *out )
{
	if ( m_changed && !reinit() )
	{
		return false;
	}
	
	contextInit();
	
	const byte *inpData = ( const byte * )in.data();
	int inpLen = in.length();
	
	int plainLen = inpLen;
	int finitLen = 0;
	
	byte *outData = ( byte * )malloc( plainLen );
	
	// расшифровываем данные
	EVP_DecryptUpdate( &m_decCtxt, outData, &plainLen, inpData, inpLen );
	EVP_DecryptFinal_ex( &m_decCtxt, outData + plainLen, &finitLen );
	
	// записываем выходные данные
	plainLen += finitLen;
	*out = QByteArray( ( const char * )outData, plainLen );
	
	// производим очистку
	free( outData );
	EVP_CIPHER_CTX_cleanup( &m_decCtxt );
	
	return true;
}

bool Crypter::encrypt( const QByteArray &in, QByteArray *out )
{
	if ( m_changed && !reinit() )
	{
		return false;
	}
	
	contextInit();
	
	const byte *inpData = ( const byte * )in.data();
	int inpLen = in.length();
	
	int cryptLen = inpLen + getBlockSize();
	int finitLen = 0;
	
	byte *outData = ( byte* )malloc( cryptLen );
	
	// зашифровываем данные
	EVP_EncryptUpdate( &m_encCtxt, outData, &cryptLen, inpData, inpLen );
	EVP_EncryptFinal_ex( &m_encCtxt, outData + cryptLen, &finitLen );
	
	// записываем выходные данные
	cryptLen += finitLen;
	*out = QByteArray( ( const char * )outData, cryptLen );
	
	// производим очистку
	free( outData );
	EVP_CIPHER_CTX_cleanup( &m_encCtxt );
	
	return true;
}

bool Crypter::reinit()
{
	if ( !( m_cipher = getCipher() ) )
	{
		return false;
	}
	
	bool result = false;
	
	// выделяем память для ключа и иниц. вектора
	int bytesCount = m_keySize / 8;
	if ( key != NULL ) {
		free( key );
	}
	key	= ( byte * )malloc( bytesCount );
	if ( m_iv != NULL ) {
		free( m_iv );
	}
	m_iv = ( byte * )malloc( bytesCount );
	
	// странный баг при прямой передаче string.c_str() в функцию
	int passlen = m_pass.length();
	byte *keyData = ( byte* )malloc( passlen );
	keyData[ passlen ] = 0;
	memcpy( keyData, m_pass.toStdString().c_str(), passlen );
	
	result = EVP_BytesToKey( m_cipher,
		EVP_sha1(),
		0,
		keyData,
		passlen,
		kRoundsCount,
		key, m_iv ) == bytesCount;
	
	free( keyData );
	
	m_changed = !result;
	
	return result;
}

void Crypter::setKeySize( int size )
{
	if ( m_keySize != size )
	{
		m_keySize = size;
		m_changed = true;
	}
}

void Crypter::setMode( CryptMode mode )
{
	if ( m_mode != mode )
	{
		m_mode = mode;
		m_changed = true;
	}
}

void Crypter::setPass( cst_str pass )
{
	if ( m_pass != pass )
	{
		m_pass = pass;
		m_changed = true;
	}
}

