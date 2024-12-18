#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include <QString>
#include <map>
#include "types.h"

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

class KeyEntity;

class TreeNode
{
public:
	typedef std::map< QString, TreeNode* > TreeMap;
	
	TreeNode(): m_ent( 0 ), m_parent( 0 ) {}
	~TreeNode();
	
	void cutNode( cst_str name );
	// вырезает подузел с именем name ( не удаляя его физически )
	
	void removeNode( cst_str name );
	// удаляет подузел с именем name
	
	bool renameNode( cst_str oldName, cst_str newName );
	/*
		Переименовывает подузел с именем oldName в newName.
		Если подузел с таким именем не существует, или
		существует подузел с именем newName, то вернет false.
		Метод не изменяет никакую информацию в подузле -
		пользователю необходимо самому вызвать метод setName
	*/
	
	TreeMap::const_iterator cbegin() const {
		return m_children.cbegin();
	}
	
	TreeMap::const_iterator cend() const {
		return m_children.cend();
	}
	
	// [getters]
	QString descr() const { return m_descr; }
	// получает описание узла
	
	KeyEntity *entity() const
	// получает привязанную к узлу сущность (если таковая есть)
	{
		return m_ent;
	}
	
	TreeNode *getNode( cst_str name );
	// получает подузел (ребенок 1-го уровня) с именем name
	
	QString name() const
	// получает имя текущего узла
	{
		return m_name;
	}
	
	QString path() const;
	
	TreeNode *parent() const
	// получает родителя узла
	{
		return m_parent;
	}

	// [setters]
	void setChild( cst_str name, TreeNode *node );
	/*
		Добавляет ребенка с именем name. Предполагается, что
		это первый ребенок с таким именем, в противном случае
		поведение не определено
	*/
	
	void setDescr( cst_str descr ) { m_descr = descr; }
	// устанавливает описание узла
	
	void setEntity( KeyEntity *ent ) {
		m_ent = ent;
	}
	
	bool setName( cst_str name );
	/*
		Изменяет имя текущего узла. Если оно пустое или не
		изменилось, то вернет false
	*/
	
	void setParent( TreeNode *parent ) {
		m_parent = parent;
	}

private:
	TreeMap m_children; // дети узла
	QString m_descr; // описание узла
	KeyEntity *m_ent; // привязанная сущность
	QString m_name; // имя узла
	TreeNode *m_parent; // родитель узла
};

#endif // _TREE_NODE_H_

