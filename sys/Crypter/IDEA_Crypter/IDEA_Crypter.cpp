#include "IDEA_Crypter.h"

const EVP_CIPHER *IDEA_Crypter::getCipher()
{
	const EVP_CIPHER *result = 0;
	switch ( mode() )
	{
		case Crypter::CBC: result = EVP_idea_cbc(); break;
		case Crypter::CFB: result = EVP_idea_cfb(); break;
		case Crypter::ECB: result = EVP_idea_ecb(); break;
		case Crypter::OFB: result = EVP_idea_ofb(); break;
	}
	if ( result ) {
		setKeySize( 128 );
	}
	return result;
}
