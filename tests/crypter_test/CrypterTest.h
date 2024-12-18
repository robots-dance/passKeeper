#ifndef _CRYPTER_TEST_H_
#define _CRYPTER_TEST_H_

#include <QtTest>
#include "Crypter.h"
#include "types.h"

class Crypter;

class CrypterTest: public QObject
{
Q_OBJECT

public:
	CrypterTest( CryptAlg alg, int keySize, Crypter::CryptMode mode )
		: m_alg( alg ), m_keySize( keySize ), m_mode( mode )
	{}

private slots:
	void cleanupTestCase();
	void decryptFile();
	void encryptFile();
	void initTestCase();

private:
	CryptAlg m_alg;
	Crypter *m_cr;
	int m_keySize;
	Crypter::CryptMode m_mode;
};

#endif // _CRYPTER_TEST_H_

