#include "AES_Crypter.h"
#include "Camellia_Crypter.h"
#include "IDEA_Crypter.h"
#include "utils.h"

#include "CrypterTest.h"

#define CHECK_NEW_PASS 0

const QString g_fileName = "./crypter_test.txt";
const QString g_newFileName = "./crypter_test_new.data";
const QString g_password = "0123456789abcdef";

void CrypterTest::cleanupTestCase()
{
	delete m_cr;
}

void CrypterTest::decryptFile()
{
	QVERIFY2( QFile::exists( g_newFileName ), "encrypted file not exists" );
	QVERIFY2( QFile::exists( g_fileName ), "input file not exists" );
	QByteArray data = Utils::readFile( g_newFileName );
	QFile::remove( g_newFileName );
	QVERIFY2( data.size() != 0, "empty encrypted file" );
	QByteArray plainData = Utils::readFile( g_fileName );
	QVERIFY2( plainData.size() != 0, "empty input file" );
	QByteArray decrypted;
	#if CHECK_NEW_PASS
	m_cr->setPass( g_password + "_" );
	#endif
	QVERIFY2( m_cr->decrypt( data, &decrypted ),
		"errors occured during decription" );
	QVERIFY2( plainData == decrypted, "bad decryption operation" );
}

void CrypterTest::encryptFile()
{
	QVERIFY2( QFile::exists( g_fileName ), "input file not exists" );
	QByteArray plainData = Utils::readFile( g_fileName );
	QVERIFY2( plainData.size() != 0, "empty input file" );
	QByteArray out;
	QVERIFY2( m_cr->encrypt( plainData, &out ),
		"errors occured during encryption" );
	QVERIFY2( Utils::writeFile( g_newFileName, out ), "saving error" );
}

void CrypterTest::initTestCase()
{
	switch ( m_alg )
	{
		case AES:
		{
			m_cr = new AES_Crypter( m_keySize, g_password, m_mode );
		}
		break;
		
		case IDEA:
		{
			m_cr = new IDEA_Crypter( m_keySize, g_password, m_mode );
		}
		break;
		
		case Camellia:
		{
			m_cr = new Camellia_Crypter( m_keySize, g_password, m_mode );
		}
		break;
		
		default:
		{
			m_cr = NULL;
		}
	}
	QVERIFY2( m_cr != NULL, "bad crypter" );
}
