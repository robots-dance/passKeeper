#ifndef _IDEA_CRYPTER_H_
#define _IDEA_CRYPTER_H_

#include "Crypter.h"

class IDEA_Crypter: public Crypter
{
public:
	IDEA_Crypter(){}
	
	IDEA_Crypter( int keySize, cst_str pass, CryptMode mode = Crypter::CBC ):
		Crypter( keySize, pass, mode )
	{}
	
	~IDEA_Crypter() {}

protected:
	int getBlockSize() {
		return 64;
	}
	
	const EVP_CIPHER *getCipher();
};

#endif // _IDEA_CRYPTER_H_

