#include <stdlib.h>
#include <time.h>
#include "utils.h"

// generator functions
void Utils::initGenerator()
{
	srand( time( 0 ) );
}

int Utils::generateNumber( int min, int max )
	throw ( std::invalid_argument )
{
	if ( min > max ) {
		throw std::invalid_argument( "min greater max" );
	}
	return min + rand() % ( max - min + 1 );
}

QString Utils::generateString( cst_str alpha, int min, int max )
{
	int alphaSize = alpha.length();
	if ( 0 == alphaSize || min > max || max <= 0 || min <= 0 )
	{
		return "";
	}
	QString result = "";
	int len = generateNumber( min, max );
	int lastAlphaIndex = alphaSize - 1;
	for ( int chIndex = 0; chIndex < len; chIndex++ )
	{
		result += alpha[ generateNumber( 0, lastAlphaIndex ) ];
	}
	return result;
}

// file functions
QByteArray Utils::readFile( cst_str path )
{
	QByteArray result;
	QFile input( path );
	if ( input.open( QIODevice::ReadOnly ) )
	{
		result = input.readAll();
		input.close();
	}
	return result;
}

bool Utils::writeFile( cst_str path, const QByteArray &data )
{
	bool result = false;
	QFile output( path );
	if ( output.open( QIODevice::WriteOnly ) )
	{
		result = output.write( data ) == data.size();
	}
	return result;
}
