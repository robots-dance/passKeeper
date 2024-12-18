#ifndef _TYPES_H_
#define _TYPES_H_

#include <map>
#include <utility>

typedef const QString & cst_str;
typedef std::pair< QString, QString > KeyVal;
typedef std::map<QString, QString> KeyValMap;

// exception classes
class FieldNoFound {};
class Eof {};

enum CryptAlg {
	AES = 0,
	IDEA,
	Camellia
};

#endif // _TYPES_H_

