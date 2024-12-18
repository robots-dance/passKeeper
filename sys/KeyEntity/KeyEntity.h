#ifndef _KEYENTITY_H_
#define _KEYENTITY_H_

#include <QString>
#include "types.h"

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

class KeyEntity
{
public:
	KeyEntity(): m_deleted( false ) {}
	
	KeyEntity( cst_str key ): m_key( key ), m_deleted( false ) {}
	
	KeyValMap::const_iterator cbegin() const
	{
		return m_fields.cbegin();
	}
	
	KeyValMap::const_iterator cend() const
	{
		return m_fields.cend();
	}
	
	void delf( cst_str name );
	// удаляет поле с ключом name, если такое есть
	
	QString getf( cst_str name ) throw( FieldNoFound );
	// получает значение поля по имени name
	
	int fieldsCount() const { return m_fields.size(); }
	
	bool isDeleted() const { return m_deleted; }
	
	void setDeleted() { m_deleted = true; }
	// устанавливает флаг удаления
	
	void setf( cst_str name, cst_str value );
	// устанавливает значение value для поля name

private:
	QString m_key; // главный ключ
	QString m_login, m_password;
	
	KeyValMap m_fields;
	// все остальные поля: [имя поля] - значение
	
	bool m_deleted; // флаг к удалению
};

#endif // _KEYENTITY_H_

