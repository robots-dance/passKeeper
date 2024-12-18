#include <QtXml>

#include "KeyEntity.h"
#include "TreeNode.h"
#include "debug.h"

#include "XmlDb.h"

void addTextNode( QDomElement *parent, QString name,
	QString text, QDomDocument *doc )
{
	QDomText textNode = doc->createTextNode( text );
	QDomElement field = doc->createElement( name );
	field.appendChild( textNode );
	parent->appendChild( field );
}

bool getNodeText( const QDomNode &node, QString *output )
// возвращает текст из первого ребенка узла
{
	bool result = false;
	QDomNode child = node.firstChild();
	if ( !child.isNull() )
	{
		QDomText text = child.toText();
		if ( !text.isNull() )
		{
			*output = text.data();
			result = true;
		}
	}
	return result;
}

void XmlDb::createTree( TreeNode *tree, const QDomNode &node )
{
	QDomNodeList children = node.childNodes();
	for ( int childIndex = 0; childIndex < children.count();
		childIndex++ )
	{
		QDomNode child = children.at( childIndex );
		if ( child.nodeName() != "node" )
		{
			continue;
		}
		
		QDomNamedNodeMap attributes = child.attributes();
		if ( !attributes.contains( "name" ) )
		{
			continue;
		}
		
		QString name;
		getNodeText( attributes.namedItem( "name" ), &name );
		if ( name.length() == 0 )
		{
			continue;
		}
		
		QString descr;
		getNodeText( child, &descr );
		
		TreeNode *subTree = new TreeNode();
		subTree->setDescr( descr );
		tree->setChild( name, subTree );
		
		if ( attributes.contains( "key" ) )
		// это ключ
		{
			QString key;
			getNodeText( attributes.namedItem( "key" ), &key );
			if ( key.length() > 0 )
			{
				auto entIter = m_entities.find( key );
				if ( entIter != m_entities.end() )
				{
					subTree->setEntity( entIter->second );
				}
			}
		}
		
		if ( subTree->entity() == NULL )
		// это дерево
		{
			createTree( subTree, child );
		}
	}
}

QString XmlDb::getContent()
{
	QDomDocument *doc = new QDomDocument();
	QString content = "<root></root>";
	doc->setContent( content, ( QString * )NULL, 0, 0 );
	QDomNode root = doc->firstChild();
	
	// добавляем заголовок
	QDomElement header = doc->createElement( "header" );
	root.appendChild( header );
	
	// формируем xml для дерева
	QDomElement treeElem = doc->createElement( "tree" );
	getTreeXml( &treeElem, m_tree, doc );
	root.appendChild( treeElem );
	
	//формируем xml для ключевых сущностей
	QDomElement elements = doc->createElement( "keyents" );
	root.appendChild( elements );
	static const QString definedFields[] = {
		"login",
		"password" };
	auto entIter = m_entities.begin();
	for ( ; entIter != m_entities.end(); entIter++ )
	{
		KeyEntity *ent = entIter->second;
		if ( ent->isDeleted() )
		{
			continue;
		}
		
		QString name;
		
		QDomElement elem = doc->createElement( "ent" );
		elem.setAttribute( "key", entIter->first );
		
		// добавляем предопределенные поля
		for ( unsigned fieldIndex = 0;
			fieldIndex < sizeof( definedFields ) / sizeof( QString );
			fieldIndex++ )
		{
			name = definedFields[ fieldIndex ];
			addTextNode( &elem, name, ent->getf( name ), doc );
		}
		
		// добавляем все остальные поля
		auto childIter = ent->cbegin();
		for ( ; childIter != ent->cend(); ++childIter )
		{
			name = childIter->first;
			addTextNode( &elem, name, ent->getf( name ), doc );
		}
		
		elements.appendChild( elem );
	}
	
	QString result = doc->toString();
	
	delete doc;
	
	return result;
}

void XmlDb::getTreeXml( QDomElement *elem, TreeNode *tree,
	QDomDocument *doc )
{
	auto childIter = tree->cbegin();
	
	for ( ; childIter != tree->cend(); ++childIter )
	{
		QString name = childIter->first; 
		TreeNode *subTree = tree->getNode( name );
		KeyEntity *ent = subTree->entity();
		if ( ent != NULL && ent->isDeleted() )
		{
			continue;
		}
		QDomElement node = doc->createElement( "node" );
		node.setAttribute( "name", name );
		QDomText descr = doc->createTextNode( subTree->descr() );
		node.appendChild( descr );
		if ( ent != NULL )
		{
			node.setAttribute( "key", ent->getf( "key" ) );
		}
		else {
			getTreeXml( &node, subTree, doc );
		}
		elem->appendChild( node );
	}
}

TreeNode *XmlDb::load()
{
	TreeNode *result = NULL;
	
	if ( state() != DbOpened )
	{
		return NULL;
	}
	
	QDomDocument *doc = new QDomDocument();
	if ( !doc->setContent( m_bytes, ( QString * )NULL, 0, 0 ) )
	{
		delete doc;
		return NULL;
	}
	
	#if _XMLDB_DEBUG_
	qDebug() << doc->toString() << "\n";
	#endif
	
	// создаем ключевые сущности
	QString text;
	QDomNodeList elements = doc->elementsByTagName( "ent" );
	for ( int elemIndex = 0; elemIndex < elements.count();
		elemIndex++ )
	{
		QDomNode elem = elements.at( elemIndex );
		QDomNamedNodeMap attributes = elem.attributes();
		if ( !attributes.contains( "key" ) )
		{
			continue;
		}
		
		if ( getNodeText( attributes.namedItem( "key" ), &text ) )
		{
			KeyEntity *ent = new KeyEntity( text );
			QDomNodeList nodes = elem.childNodes();
			for ( int nodeIndex = 0; nodeIndex < nodes.count();
				nodeIndex++ )
			{
				QDomNode node = nodes.at( nodeIndex );
				if ( node.isCharacterData() )
				// пропускаем текст и пробелы между узлами
				{
					continue;
				}
				
				// извлекаем дополнительное поле
				if ( !getNodeText( node, &text ) )
				{
					text = "";
				}
				ent->setf( node.nodeName(), text );
			}
			addEntity( ent );
		}
	}
	
	setState( DbLoaded );
	
	// создаем дерево
	result = new TreeNode();
	QDomNode treeElem = doc->elementsByTagName( "tree" ).at( 0 );
	if ( !treeElem.isNull() )
	{
		createTree( result, treeElem );
	}
	
	m_tree = result;
	
	delete doc;
	
	return result;
}

void XmlDb::unload()
{
	QString content = getContent();
	
	#if _XMLDB_DEBUG_
	qDebug() << content << "\n";
	#endif
	
	m_bytes.clear();
	m_bytes.append( content );
}

