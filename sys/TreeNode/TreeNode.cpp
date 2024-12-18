#include <list>
#include "KeyEntity.h"
#include "TreeNode.h"

TreeNode::~TreeNode()
{
	QString name;
	auto childIter = cbegin();
	for ( ; childIter != cend(); ++childIter )
	{
		delete getNode( childIter->first );
	}
}

void TreeNode::cutNode( cst_str name )
{
	m_children.erase( name );
}

TreeNode *TreeNode::getNode( cst_str name )
{
	if ( m_children.find( name ) != m_children.end() )
	{
		return m_children[ name ];
	}
	else {
		return NULL;
	}
}

QString TreeNode::path() const
{
	std::list< QString > parts;
	parts.push_back( m_name );
	
	TreeNode *curParent = m_parent;
	int depth = 1;
	while ( curParent != NULL )
	{
		parts.push_front( curParent->name() );
		depth++;
		curParent = curParent->parent();
	}
	
	QString result = "";
	auto partIter = parts.begin();
	int depthCounter = 0;
	for ( ; partIter != parts.end(); ++partIter )
	{
		result += *partIter;
		if ( depthCounter > 0 && depthCounter < depth - 1 )
		{
			result += "/";
		}
		depthCounter++;
	}
	
	return result;
	
}

void TreeNode::removeNode( cst_str name )
{
	TreeNode *node = getNode( name );
	
	// remove children
	auto childIter = node->cbegin();
	for ( ; childIter != node->cend(); ++childIter )
	{
		node->removeNode( childIter->first );
	}
	
	KeyEntity *ent = node->entity();
	if ( ent != NULL ) {
		ent->setDeleted();
	}
	m_children.erase( node->name() );
	delete node;
}

bool TreeNode::renameNode( cst_str oldName, cst_str newName )
{
	TreeNode *child = getNode( oldName );
	if ( child != NULL && m_children.find( newName ) == m_children.end() )
	{
		m_children.erase( oldName );
		m_children[ newName ] = child;
		return true;
	}
	else {
		return false;
	}
}

void TreeNode::setChild( cst_str name, TreeNode *node )
{
	m_children[ name ] = node;
	node->setName( name );
	node->setParent( this );
}

bool TreeNode::setName( cst_str name )
{
	bool result = false;
	if ( name.length() > 0 && m_name != name )
	{
		if (!m_parent || m_name.length() == 0 )
		{
			m_name = name;
			result = true;
		}
		else
		{
			result = m_parent->renameNode( m_name, name );
			if ( result ) {
				m_name = name;
			}
		}
	}
	return result;
}
