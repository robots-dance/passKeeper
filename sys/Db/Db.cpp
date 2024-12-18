#include <QFile>

#include "Crypter.h"
#include "KeyEntity.h"
#include "TreeNode.h"
#include "utils.h"

#include "Db.h"

const QString Db::kKeyAlpha = "0123456789abcdef";

Db::~Db()
{
	if ( isOpened() )
	{
		close();
	}
}

bool Db::open( cst_str path )
{
	bool result = true;
	QFile file( path );
	bool fileOpened = false;
	if ( !( fileOpened = file.open( QIODevice::ReadOnly ) ) )
	{
		result = false;
	}
	
	QByteArray readedBytes = file.readAll();
	if ( m_curCrypter != NULL )
	{
		if ( m_curCrypter->decrypt( readedBytes, &m_bytes ) )
		{
			m_state = DbOpened;
		}
		else {
			result = false;
		}
	}
	else
	{
		m_bytes = readedBytes;
		m_state = DbOpened;
	}
	
	if ( fileOpened ) {
		file.close();
	}
	if ( result ) {
		m_path = path;
	}
	
	return result;
}

TreeNode *Db::create()
{
	m_tree = new TreeNode();
	m_bytes.clear();
	setState( DbChanged );
	m_created = true;
	return m_tree;
}

TreeNode *Db::create( const QByteArray &data )
{
	m_bytes = data;
	setState( DbOpened );
	if ( ( m_tree = load() ) != NULL )
	{
		setState( DbChanged );
		m_created = true;
	}
	else
	{
		setState( DbNone );
		m_bytes = QByteArray();
	}
	return m_tree;
}

bool Db::addEntity( KeyEntity *ent )
{
	bool result = false;
	QString key = ent->getf( "key" );
	if ( m_entities.find( key ) == m_entities.end() )
	{
		result = true;
	}
	else if ( m_entities[ key ]->isDeleted() )
	{
		result = removeEntity( key );
	}
	if ( result ) {
		m_entities[ key ] = ent;
	}
	return result;
}

void Db::addEntity( KeyEntity *ent, int )
{
	bool finded = false;
	QString newKey;
	while ( !finded )
	{
		newKey = Utils::generateString( Db::kKeyAlpha,
			Db::kKeyLen, Db::kKeyLen );
		finded = m_entities.find( newKey ) == m_entities.end();
	}
	ent->setf( "key", newKey );
	m_entities[ newKey ] = ent;
}

bool Db::addEntity( cst_str key )
{
	bool result = false;
	auto entIter = m_entities.find( key );
	if ( entIter != m_entities.end() )
	{
		KeyEntity *ent = entIter->second;
		if ( ent->isDeleted() )
		{
			result = removeEntity( key );
		}
	}
	else {
		result = true;
	}
	if ( result )
	{
		m_entities[ key ] = new KeyEntity( key );
	}
	return result;
}

bool Db::removeEntity( cst_str key )
{
	if ( m_entities.find( key ) != m_entities.end() )
	{
		delete m_entities[ key ];
		m_entities.erase( key );
		return true;
	}
	return false;
}

QString Db::getValue( cst_str key, cst_str paramName )
	throw( FieldNoFound )
{
	QString result;
	if ( m_state >= DbLoaded && m_entities.find( key ) != m_entities.end() )
	{
		KeyEntity *ent = m_entities[ key ];
		result = ent->getf( paramName );
	}
	else {
		throw FieldNoFound();
	}
	return result;
}

bool Db::setValue( cst_str key, cst_str paramName, cst_str value )
{
	bool result = true;
	if ( m_state >= DbLoaded )
	{
		KeyEntity *ent;
		if ( m_entities.find( key ) != m_entities.end() )
		{
			ent = m_entities[ key ];
		}
		else
		{
			ent = new KeyEntity();
			m_entities[ key ] = ent;
		}
		ent->setf( paramName, value );
	}
	else {
		result = false;
	}
	return result;
}

bool Db::save( cst_str path )
{
	bool result = false;
	QString savePath = path != "" ? path : m_path;
	
	// определяем, есть ли изменения
	if ( m_state != DbChanged && savePath == m_path )
	{
		return false;
	}
	
	// пытаемся открыть файл для сохранения
	QFile file( savePath );
	bool fileOpened = false;
	if ( !( fileOpened = file.open( QIODevice::WriteOnly ) ) )
	{
		return false;
	}
	
	// получаем сырые данные и шифруем их (при необходимости)
	unload();
	QByteArray input = m_bytes;
	if ( m_curCrypter != NULL )
	{
		if (m_curCrypter->encrypt( input, &m_bytes ) ) {
			result = true;
		}
	}
	else {
		result = true;
	}
	
	if ( result )
	// сохраняем файл
	{
		if ( file.write( m_bytes ) != m_bytes.size() )
		{
			result = false;
		}
		else
		{
			m_path = savePath;
			m_state = DbLoaded;
			m_created = false;
		}
	}
	
	file.close();
	
	return result;
}

void Db::close()
{
	m_state = DbNone;
	m_bytes.clear();
	m_curCrypter = NULL;
	m_path = "";
	
	// удаляем все сущности
	auto entIter = m_entities.begin();
	while ( entIter != m_entities.end() )
	{
		delete entIter->second;
		entIter++;
	}
	m_entities.clear();
	
	delete m_tree;
	m_tree = NULL;
}

