#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <QByteArray>
#include <QString>
#include <map>
#include "types.h"

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

class Crypter;
class KeyEntity;
class TreeNode;

class Db
{
public:
	enum DbState { DbNone = 0, DbOpened, DbLoaded, DbChanged };

	Db(): m_tree( 0 ), m_created( false ),
		m_curCrypter( 0 ), m_state( DbNone )
	{}
	
	virtual ~Db();
	
	// ==== api ====
	virtual bool open( cst_str path );
	/*
		@Params
			path - путь к файлу базы данных
		
		@Description
			Этот метод пытается открыть файл базы данных и расшифровать
			его, используя объект crypter. Если все прошло успешно,
			поле bytes будет содержать расшифрованные данные
	*/
	
	virtual TreeNode *create();
	/*
		@Description
			Инициализирует минимальное количество структур для нормального
			функционирования объекта ( используется для создания новых
			файлов ). Возвращает указатель на текущее дерево
	*/
	
	virtual TreeNode *create( const QByteArray &data );
	/*
		@Description
			Иницилизирует внутреннее состояние базы из предоставленных
			пользователем данных ( объект data ). Возвращает указатель
			на текущее дерево, если его получилось создать, в противном
			случае - NULL
	*/

	virtual TreeNode *load() = 0;
	/*
		@Description
			Этот метод пытается загрузить сырые данные из поля bytes.
			Каждый наследник класса должен переопределить его
		
		@Notice
			Перед вызовом этого метода должен успешно отработать Db::open
	*/
	
	/*
		эти методы могут использоваться даже если никакой файл не был
		загружен ( для создания новых файлов )
	*/
	virtual bool addEntity( KeyEntity * );
	virtual void addEntity( KeyEntity *, int );
	virtual bool addEntity( cst_str key );
	virtual bool removeEntity( cst_str key );
	
	virtual QString getValue( cst_str key, cst_str paramName )
		throw( FieldNoFound );
	/*
		@Params
			key - искомый ключ
			paramName - имя искомого параметра
		
		@Description
			Этот  метод пытается найти данные, связанные с параметром
			paramName в области ключа key. Если ничего не было найдено,
			выкидывается исключение
		
		@Notice
			Необходимо наличие загруженного файла
	*/
	
	virtual	bool setValue( cst_str key, cst_str paramName, cst_str value );
	/*
		@Params
			key - искомый ключ
			paramName - имя искомого параметра
			value - новое значение параметра
		
		@Description
			Этот метод пытается задать новое значение для составного
			ключа key - paramName. В случае успеха вернет true
		
		@Notices
			Необходимо наличие загруженного файла
	*/
	
	virtual bool save( cst_str path = "" );
	/*
		@Params
			path - новый путь для сохранения ( будет использоваться поле
			path в в случае пустого значения входного параметра )
		
		@Description
			Сохраняет текущие данные в файл. Никаких приготовлений совершать
			не надо - все необходимые действия по выгрузке entities в bytes
			будут выполнены самим методом. Если указать непустой путь, то
			можно сохранить новую базу ( созданную программно, а не
			загруженную из ФС ). В случае успеха вернет true
	*/
	
	virtual void unload() = 0;
	/*
		@Description
			Выгружает entities в bytes для последующего сохранения.
			Этот метод должен переопределяться каждым наследником класса
	*/
	
	virtual void close();
	/*
		@Description
			Закрывает файл базы данных и очищает некоторые ресурсы - сырые
			данные, указатели и др.
	*/
	
	QString getPath() const { return m_path; }
	void setChanged() { if ( isOpened() ) m_state = DbChanged; }
	void setCrypter( Crypter *crypter ) { m_curCrypter = crypter; }
	DbState state() const { return m_state; }
	
	// методы, возвращающие некоторые состояния объекта
	bool isChanged() const { return m_state == DbChanged; }
	bool isCreated() const  {return m_created; }
	bool isOpened() const { return m_state >= DbLoaded; }

protected:
	typedef std::map< QString, KeyEntity * > KEMap;
	
	void setState( DbState state ) { m_state = state; }
	
	QByteArray m_bytes; // сырые данные базы
	KEMap m_entities; // массив ключевых сущностей
	TreeNode *m_tree;

private:
	static const QString kKeyAlpha;
	static const int kKeyLen = 16;
	
	bool m_created; // создан, а не открыт
	Crypter *m_curCrypter; // выбранный шифровальщик
	QString m_path; // путь открытой базы
	DbState m_state; // текущее состояние базы
};

#endif // _DATABASE_H_
