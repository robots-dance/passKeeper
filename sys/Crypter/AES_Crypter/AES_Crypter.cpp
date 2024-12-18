#include "AES_Crypter.h"

const EVP_CIPHER *AES_Crypter::getCipher()
{
	const EVP_CIPHER *result = 0;
	switch ( keySize() )
	{
		case 128:
		{
			switch ( mode() )
			{
				case Crypter::CBC: result = EVP_aes_128_cbc(); break;
				case Crypter::CFB: result = EVP_aes_128_cfb(); break;
				case Crypter::ECB: result = EVP_aes_128_ecb(); break;
				case Crypter::OFB: result = EVP_aes_128_ofb(); break;
			}
		}
		break;
		
		case 192:
		{
			switch ( mode() )
			{
				case Crypter::CBC: result = EVP_aes_192_cbc(); break;
				case Crypter::CFB: result = EVP_aes_192_cfb(); break;
				case Crypter::ECB: result = EVP_aes_192_ecb(); break;
				case Crypter::OFB: result = EVP_aes_192_ofb(); break;
			}
		}
		break;
		
		case 256:
		{
			switch ( mode() )
			{
				case Crypter::CBC: result = EVP_aes_256_cbc(); break;
				case Crypter::CFB: result = EVP_aes_256_cfb(); break;
				case Crypter::ECB: result = EVP_aes_256_ecb(); break;
				case Crypter::OFB: result = EVP_aes_256_ofb(); break;
			}
		}
		break;
	}
	return result;
}
