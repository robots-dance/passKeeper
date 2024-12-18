#ifndef _CAMELLIA_CRYPTER_H_
#define _CAMELLIA_CRYPTER_H_

#include "Crypter.h"

class Camellia_Crypter: public Crypter
{
public:
	Camellia_Crypter() {}
	
	Camellia_Crypter( int keySize, cst_str pass,
		CryptMode mode = Crypter::CBC ): Crypter( keySize, pass, mode )
	{}
	
	~Camellia_Crypter(){}

protected:
	int getBlockSize() {
		return 128;
	}
	
	const EVP_CIPHER *getCipher();
};

#endif // _CAMELLIA_CRYPTER_H_

