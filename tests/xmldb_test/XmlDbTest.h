#ifndef _XMLDB_TEST_H_
#define _XMLDB_TEST_H_

#include <QtTest>

class XmlDbTest: public QObject
{
Q_OBJECT

private slots:
	void checkCycle();
};

#endif // _XMLDB_TEST_H_

