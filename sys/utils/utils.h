#ifndef _UTILS_H_
#define _UTILS_H_

#include <QtCore>
#include <stdexcept>
#include "types.h"

namespace Utils 
{
	// generator functions
	void initGenerator();
	
	int generateNumber( int min, int max )
		throw ( std::invalid_argument );
	
	QString generateString( cst_str alpha, int min, int max );
	
	// file functions
	QByteArray readFile( cst_str path );
	
	bool writeFile( cst_str path, const QByteArray & );
}

#endif // _UTILS_H_

