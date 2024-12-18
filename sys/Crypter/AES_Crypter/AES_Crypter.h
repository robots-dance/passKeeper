#ifndef _AES_CRYPTER_H_
#define _AES_CRYPTER_H_

#include "Crypter.h"

class AES_Crypter: public Crypter
{
public:
	AES_Crypter() {}
	
	AES_Crypter( int keySize, cst_str pass, CryptMode mode = Crypter::CBC )
		: Crypter( keySize, pass, mode )
	{}
	
	~AES_Crypter() {}

protected:
	int getBlockSize() {
		return 128;
	}
	
	const EVP_CIPHER *getCipher();
};

#endif // _AES_CRYPTER_H_

