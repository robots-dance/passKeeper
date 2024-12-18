#include "KeyEntity.h"

void KeyEntity::delf( cst_str name )
{
	m_fields.erase( name );
}

QString KeyEntity::getf( cst_str name ) throw ( FieldNoFound )
{
	QString result;
	if ( name == "alias" ) {
		throw FieldNoFound();
	}
	if ( name == "key" ) {
		result = m_key;
	}
	else if ( name == "login" ) {
		result = m_login;
	}
	else if ( name == "password" ) {
		result = m_password;
	}
	else
	{
		if ( m_fields.find( name ) != m_fields.end() )
		{
			result = m_fields[ name ];
		}
		else {
			throw FieldNoFound();
		}
	}
	return result;
}

void KeyEntity::setf( cst_str name, cst_str value )
{
	if ( name == "alias" ) {
		// do nothing
	}
	else if ( name == "key" ) {
		m_key = value;
	}
	else if ( name == "login" ) {
		m_login = value;
	}
	else if ( name == "password" ) {
		m_password = value;
	}
	else {
		m_fields[ name ] = value;
	}
}
