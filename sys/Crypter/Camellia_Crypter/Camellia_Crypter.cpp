#include "Camellia_Crypter.h"

const EVP_CIPHER *Camellia_Crypter::getCipher()
{
	const EVP_CIPHER *result = 0;
	switch ( keySize() )
	{
		case 128:
		{
			switch ( mode() )
			{
				case Crypter::CBC: result = EVP_camellia_128_cbc(); break;
				case Crypter::CFB: result = EVP_camellia_128_cfb(); break;
				case Crypter::ECB: result = EVP_camellia_128_ecb(); break;
				case Crypter::OFB: result = EVP_camellia_128_ofb(); break;
			}
		}
		break;
		
		case 256:
		{
			switch ( mode() )
			{
				case Crypter::CBC: result = EVP_camellia_256_cbc(); break;
				case Crypter::CFB: result = EVP_camellia_256_cfb(); break;
				case Crypter::ECB: result = EVP_camellia_256_ecb(); break;
				case Crypter::OFB: result = EVP_camellia_256_ofb(); break;
			}
		}
		break;
	}
	
	return result;
}
