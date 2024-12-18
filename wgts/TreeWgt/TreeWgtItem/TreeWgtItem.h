#ifndef _TREEWGTITEM_H_
#define _TREEWGTITEM_H_

#include <QTreeWidgetItem>

class TreeNode;

class TreeWgtItem: public QTreeWidgetItem
{

public:
	enum { CopyPassword, SetPassword, ShowInfo };
	
	TreeWgtItem( QTreeWidget *view ) : QTreeWidgetItem( view ) {}
	
	TreeWgtItem( QTreeWidgetItem *parent ) : QTreeWidgetItem( parent ) {}
	
	TreeNode *node() const
	{
		return m_node;
	}
	
	void setNode( TreeNode *node )
	{
		m_node = node;
	}
	
	TreeWgtItem *parent() const
	{
		return dynamic_cast< TreeWgtItem * >( QTreeWidgetItem::parent() );
	}

private:
	TreeNode *m_node;

signals:

public slots:

};

#endif // _TREEWGTITEM_H_
