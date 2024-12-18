#include <QtXml>

#include "XmlDb.h"
#include"utils.h"

#include "XmlDbTest.h"

QString getXml( const QByteArray &data )
{
	QString result = "";
	QDomDocument document;
	if ( document.setContent( data, ( QString* )0, 0, 0 ) )
	{
		result = document.toString( 0 ).replace( "\n", "" );
	}
	return result;
}

void XmlDbTest::checkCycle()
{
	cst_str path = "./xmldb_test.xml";
	cst_str new1Path = "./xmldb_test_new1.xml";
	cst_str new2Path = "./xmldb_test_new2.xml";
	
	XmlDb db;
	
	QVERIFY( db.open( path ) );
	QVERIFY( db.load() );
	QVERIFY( db.save( new1Path ) );
	db.close();
	
	QVERIFY( db.open( new1Path ) );
	QVERIFY( db.load() );
	QVERIFY( db.save( new2Path ) );
	db.close();
	
	QString file1Content = getXml( Utils::readFile( new1Path ) );
	QString file2Content = getXml( Utils::readFile( new2Path ) );
	QFile::remove( new1Path );
	QFile::remove( new2Path );
	
	QCOMPARE( file1Content, file2Content );
}
