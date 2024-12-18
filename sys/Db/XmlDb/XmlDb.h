#ifndef _XMLDB_H_
#define _XMLDB_H_

#include "Db.h"

class QDomDocument;
class QDomElement;
class QDomNode;

class XmlDb: public Db
{
public:
	XmlDb(): Db() {}
	~XmlDb() {}
	
	QString getContent();
	TreeNode *load();
	void unload();

private:
	void createTree( TreeNode *, const QDomNode & );
	void getTreeXml( QDomElement *, TreeNode *, QDomDocument * );
};

#endif // _XMLDB_H_
